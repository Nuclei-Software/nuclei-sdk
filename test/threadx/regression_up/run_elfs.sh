#!/usr/bin/env bash

# Run generated ThreadX UP regression ELF files on QEMU or xlmodel.
#
# Result classification:
# - [PASS]: output contains SUCCESS and no failure pattern
# - [FAIL]: output contains ERROR/MEPC/MCAUSE/MDCAUSE/MTVAL
# - [TIMEOUT]: no PASS/FAIL pattern seen before timeout
# - [RUN-FAIL]: emulator command failed to start or exited unexpectedly
# - [EXCEPTION]: board GDB probe hit trap/exception handling path

set -u

DEFAULT_GDBREMOTE=whss6:22000
EXCEPTION_PATTERN='core_exception_handler|system_default_exception_handler|Exception_DumpFrame|default_intexc_handler|exc_entry|trap_entry'

usage()
{
    cat <<EOF
Usage: ./run_elfs.sh

Environment variables:
  RUNNER        Backend: qemu, xlmodel, or board, default: qemu
  CORE          Target core, default: n900fd
  PARALLEL      Max concurrent runs, default: qemu=4 xlmodel=2 board=1
  TIMEOUT       Per-ELF timeout in seconds, default: qemu=60 xlmodel=240 board=15
  DOWNLOAD      download mode passed to emulator, default: ddr
  ELF_DIR       Directory containing case ELF files, default: elfs/\${CORE}
  CASES         Space-separated case names or ELF paths
  LOG_DIR       Explicit log directory, default: logs/run_elfs_<runner>_<core>_<timestamp>
  GDBREMOTE     board runner uses this remote, default: ${DEFAULT_GDBREMOTE}
  GDB           GDB executable for board runner, default: riscv64-unknown-elf-gdb
  GDB_TIMEOUT   Per-GDB session timeout for board runner, default: 15
  QEMU32        qemu-system-riscv32 executable, default: qemu-system-riscv32
  QEMU64        qemu-system-riscv64 executable, default: qemu-system-riscv64
  XLMODEL       xl_cpumodel executable, default: xl_cpumodel

Examples:
  RUNNER=qemu CORE=n900fd ./run_elfs.sh
  PARALLEL=4 RUNNER=qemu CORE=n900fd ./run_elfs.sh
  RUNNER=xlmodel CORE=nx900fd TIMEOUT=30 ./run_elfs.sh
  RUNNER=board GDBREMOTE=whss6:22000 CORE=n900fd ./run_elfs.sh
  CASES="threadx_thread_simple_sleep_test threadx_mutex_basic_test" ./run_elfs.sh
EOF
}

require_command()
{
    local cmd=$1

    if ! command -v "${cmd}" >/dev/null 2>&1; then
        echo "Missing command: ${cmd}" >&2
        exit 1
    fi
}

now_ms()
{
    date +%s%3N
}

format_ms_seconds()
{
    local ms=$1
    awk -v ms="${ms}" 'BEGIN { printf "%.3fs", ms / 1000 }'
}

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    usage
    exit 0
fi

RUNNER=${RUNNER:-qemu}
CORE=${CORE:-n900fd}
DOWNLOAD=${DOWNLOAD:-ddr}
ELF_DIR=${ELF_DIR:-elfs/${CORE}}
CASES=${CASES:-}
RUN_ID=${RUN_ID:-$(date +%Y%m%d_%H%M%S)}
LOG_DIR=${LOG_DIR:-logs/run_elfs_${RUNNER}_${CORE}_${RUN_ID}}
SUMMARY_LOG=${LOG_DIR}/summary.log
GDBREMOTE=${GDBREMOTE:-${DEFAULT_GDBREMOTE}}
GDB=${GDB:-riscv64-unknown-elf-gdb}
GDB_TIMEOUT=${GDB_TIMEOUT:-15}
QEMU32=${QEMU32:-qemu-system-riscv32}
QEMU64=${QEMU64:-qemu-system-riscv64}
XLMODEL=${XLMODEL:-xl_cpumodel}

if [ -z "${PARALLEL+x}" ]; then
    case "${RUNNER}" in
        qemu)
            PARALLEL=4
            ;;
        xlmodel)
            PARALLEL=2
            ;;
        board)
            PARALLEL=1
            ;;
        *)
            PARALLEL=4
            ;;
    esac
else
    PARALLEL=${PARALLEL}
fi

if [ -z "${TIMEOUT+x}" ]; then
    case "${RUNNER}" in
        qemu)
            TIMEOUT=60
            ;;
        xlmodel)
            TIMEOUT=240
            ;;
        board)
            TIMEOUT=15
            ;;
        *)
            TIMEOUT=60
            ;;
    esac
else
    TIMEOUT=${TIMEOUT}
fi

mkdir -p "${LOG_DIR}"
touch "${SUMMARY_LOG}"

case "${RUNNER}" in
    qemu)
        if [[ "${CORE}" == *x* ]]; then
            EMULATOR=${QEMU64}
        else
            EMULATOR=${QEMU32}
        fi
        ;;
    xlmodel)
        EMULATOR=${XLMODEL}
        ;;
    board)
        EMULATOR=${GDB}
        ;;
    *)
        echo "Unsupported RUNNER: ${RUNNER}" >&2
        exit 1
        ;;
esac

require_command timeout
require_command "${EMULATOR}"
if [ "${RUNNER}" = "board" ]; then
    require_command "${GDB}"
fi

write_case_summary()
{
    local case_name=$1
    local verdict=$2
    local detail=$3

    printf '%s %s %s\n' "${verdict}" "${case_name}" "${detail}" >>"${SUMMARY_LOG}"
}

normalize_case_elf()
{
    local case_item=$1

    case "${case_item}" in
        *.elf)
            printf '%s\n' "${case_item}"
            ;;
        *)
            printf '%s/%s.elf\n' "${ELF_DIR}" "${case_item}"
            ;;
    esac
}

collect_elf_paths()
{
    local selected=()
    local case_item

    if [ -n "${CASES}" ]; then
        for case_item in ${CASES}; do
            selected+=("$(normalize_case_elf "${case_item}")")
        done
    else
        selected=("${ELF_DIR}"/*.elf)
    fi

    printf '%s\n' "${selected[@]}" | awk '!seen[$0]++'
}

build_run_cmd()
{
    local elf_path=$1

    case "${RUNNER}" in
        qemu)
            printf '%s -M nuclei_evalsoc,download=%s -cpu nuclei-%s,ext= -smp 1 -nodefaults -nographic -serial stdio -kernel %s\n' \
                "${EMULATOR}" "${DOWNLOAD}" "${CORE}" "${elf_path}"
            ;;
        xlmodel)
            printf '%s -M nuclei_evalsoc --cpu=%s --download=%s --smp=1 %s\n' \
                "${EMULATOR}" "${CORE}" "${DOWNLOAD}" "${elf_path}"
            ;;
    esac
}

classify_run_log()
{
    local run_log=$1

    if grep -Eq 'ERROR|MEPC|MCAUSE|MDCAUSE|MTVAL' "${run_log}"; then
        echo FAIL
        return
    fi

    if grep -Eq 'SUCCESS!?$|SUCCESS!' "${run_log}"; then
        echo PASS
        return
    fi

    echo TIMEOUT
}

probe_target()
{
    local elf_path=$1
    local gdb_log=$2

    timeout "${GDB_TIMEOUT}" "${GDB}" "${elf_path}" --batch -q \
        -ex "set pagination off" \
        -ex "set confirm off" \
        -ex "set width 0" \
        -ex "set height 0" \
        -ex "set remotetimeout ${GDB_TIMEOUT}" \
        -ex "target remote ${GDBREMOTE}" \
        -ex "interrupt" \
        -ex "info threads" \
        -ex "set \$succ = (unsigned long)test_control_successful_tests" \
        -ex "set \$fail = (unsigned long)test_control_failed_tests" \
        -ex "set \$syserr = (unsigned long)test_control_system_errors" \
        -ex "set \$status = (unsigned long)test_control_return_status" \
        -ex 'printf "GDB_PROBE success=%lu fail=%lu syserr=%lu status=%lu\n", $succ, $fail, $syserr, $status' \
        -ex "thread apply all bt" \
        -ex "detach" \
        -ex "quit" \
        >"${gdb_log}" 2>&1
}

find_exception_thread()
{
    local gdb_log=$1

    awk '
        /^Thread [0-9]+ / {
            thread_id = $2
            sub(/\(.*/, "", thread_id)
        }
        $0 ~ pattern {
            if (thread_id != "") {
                print thread_id
                exit
            }
        }
    ' pattern="${EXCEPTION_PATTERN}" "${gdb_log}"
}

dump_exception_context()
{
    local elf_path=$1
    local thread_id=$2
    local exception_log=$3

    timeout "${GDB_TIMEOUT}" "${GDB}" "${elf_path}" --batch -q \
        -ex "set pagination off" \
        -ex "set confirm off" \
        -ex "set width 0" \
        -ex "set height 0" \
        -ex "set remotetimeout ${GDB_TIMEOUT}" \
        -ex "target remote ${GDBREMOTE}" \
        -ex "interrupt" \
        -ex "thread ${thread_id}" \
        -ex "bt" \
        -ex "info registers mepc mcause mtval mdcause msubm mhartid" \
        -ex "detach" \
        -ex "quit" \
        >"${exception_log}" 2>&1
}

extract_probe_field()
{
    local key=$1
    local line=$2
    sed -n "s/.*${key}=\\([0-9][0-9]*\\).*/\\1/p" <<<"${line}"
}

extract_register_value()
{
    local reg_name=$1
    local reg_log=$2
    awk -v reg="${reg_name}" '$1 == reg { print $2; exit }' "${reg_log}"
}

classify_board_case()
{
    local case_name=$1
    local elf_path=$2
    local gdb_log=$3
    local exception_log=$4
    local probe_line
    local success_count
    local fail_count
    local sys_error_count
    local return_status
    local exception_thread
    local mepc
    local mcause
    local mtval
    local mdcause
    local msubm
    local mhartid

    if grep -Eq "${EXCEPTION_PATTERN}" "${gdb_log}"; then
        exception_thread=$(find_exception_thread "${gdb_log}" || true)
        if [ -n "${exception_thread}" ]; then
            if dump_exception_context "${elf_path}" "${exception_thread}" "${exception_log}"; then
                mepc=$(extract_register_value mepc "${exception_log}")
                mcause=$(extract_register_value mcause "${exception_log}")
                mtval=$(extract_register_value mtval "${exception_log}")
                mdcause=$(extract_register_value mdcause "${exception_log}")
                msubm=$(extract_register_value msubm "${exception_log}")
                mhartid=$(extract_register_value mhartid "${exception_log}")
            fi
            echo "EXCEPTION thread=${exception_thread} mepc=${mepc:-NA} mcause=${mcause:-NA} mtval=${mtval:-NA} mdcause=${mdcause:-NA} msubm=${msubm:-NA} mhartid=${mhartid:-NA}"
        else
            echo "EXCEPTION thread=unknown"
        fi
        return
    fi

    probe_line=$(grep 'GDB_PROBE' "${gdb_log}" | tail -n 1 || true)
    if [ -z "${probe_line}" ]; then
        echo "GDB-FAIL probe-missing"
        return
    fi

    success_count=$(extract_probe_field success "${probe_line}")
    fail_count=$(extract_probe_field fail "${probe_line}")
    sys_error_count=$(extract_probe_field syserr "${probe_line}")
    return_status=$(extract_probe_field status "${probe_line}")

    if [ -z "${success_count}" ] || [ -z "${fail_count}" ] || [ -z "${sys_error_count}" ] || [ -z "${return_status}" ]; then
        echo "PARSE-FAIL ${probe_line}"
        return
    fi

    if [ "${success_count}" -eq 1 ] && [ "${fail_count}" -eq 0 ] && [ "${sys_error_count}" -eq 0 ] && [ "${return_status}" -eq 0 ]; then
        echo "PASS success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
        return
    fi

    if [ "${success_count}" -eq 0 ] && [ "${fail_count}" -eq 0 ] && [ "${sys_error_count}" -eq 0 ]; then
        echo "NOT-FINISHED success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
        return
    fi

    echo "FAIL success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
}

upload_to_board()
{
    local elf_path=$1
    local upload_log=$2

    timeout "${GDB_TIMEOUT}" "${GDB}" "${elf_path}" --batch -q \
        -ex "set remotetimeout 240" \
        -ex "target remote ${GDBREMOTE}" \
        -ex "thread apply all monitor reset halt" \
        -ex "thread apply all info reg pc" \
        -ex "thread 1" \
        -ex "load ${elf_path}" \
        -ex "file ${elf_path}" \
        -ex 'thread apply all set $pc=_start' \
        -ex "thread apply all info reg pc" \
        -ex "thread 1" \
        -ex "monitor resume" \
        -ex "quit" \
        >"${upload_log}" 2>&1
}

stop_run_pid()
{
    local pid=$1
    local pgid
    local i

    pgid=$(ps -o pgid= -p "${pid}" 2>/dev/null | tr -d '[:space:]')
    if [ -n "${pgid}" ]; then
        kill -TERM -"${pgid}" 2>/dev/null || true
    else
        kill -TERM "${pid}" 2>/dev/null || true
    fi

    for i in $(seq 1 20); do
        if ! kill -0 "${pid}" 2>/dev/null; then
            wait "${pid}" 2>/dev/null || true
            return
        fi
        sleep 0.1
    done

    if [ -n "${pgid}" ]; then
        kill -KILL -"${pgid}" 2>/dev/null || true
    else
        kill -KILL "${pid}" 2>/dev/null || true
    fi
    wait "${pid}" 2>/dev/null || true
}

pass_count=0
fail_count=0
timeout_count=0
run_fail_count=0
overall_rc=0
RESULTS_FILE=${LOG_DIR}/results.log
SCRIPT_START_TIME_MS=$(now_ms)

{
    echo "runner=${RUNNER}"
    echo "emulator=${EMULATOR}"
    echo "core=${CORE}"
    echo "parallel=${PARALLEL}"
    echo "timeout=${TIMEOUT}"
    echo "download=${DOWNLOAD}"
    echo "elf_dir=${ELF_DIR}"
    echo "cases=${CASES:-ALL}"
    echo "gdbremote=${GDBREMOTE}"
    echo "gdb=${GDB}"
    echo "gdb_timeout=${GDB_TIMEOUT}"
    echo
} >>"${SUMMARY_LOG}"

run_one_elf()
{
    local elf_path=$1
    local case_name
    local run_log
    local run_cmd
    local run_pid
    local start_time
    local now
    local end_time
    local elapsed_ms
    local elapsed_fmt
    local verdict
    local rc
    local gdb_log
    local exception_log
    local upload_log
    local detail

    [ -e "${elf_path}" ] || return 0

    case_name=$(basename "${elf_path}" .elf)
    run_log="${LOG_DIR}/${case_name}.run.log"
    upload_log="${LOG_DIR}/${case_name}.upload.log"
    gdb_log="${LOG_DIR}/${case_name}.gdb.log"
    exception_log="${LOG_DIR}/${case_name}.exception.log"
    run_cmd=$(build_run_cmd "${elf_path}")

    {
        echo "=== ${case_name} ==="
        echo "elf: ${elf_path}"
        echo "cmd: ${run_cmd}"
        echo
    } >"${run_log}"

    printf 'Run %s on %s (timeout=%ss)\n' "${case_name}" "${RUNNER}" "${TIMEOUT}"

    if [ "${RUNNER}" = "board" ]; then
        {
            echo "=== ${case_name} ==="
            echo "elf: ${elf_path}"
            echo "gdbremote: ${GDBREMOTE}"
        } >"${upload_log}"

        start_time=$(now_ms)
        if ! upload_to_board "${elf_path}" "${upload_log}"; then
            end_time=$(now_ms)
            elapsed_ms=$((end_time - start_time))
            elapsed_fmt=$(format_ms_seconds "${elapsed_ms}")
            echo "[UPLOAD-FAIL] ${case_name} elapsed=${elapsed_fmt}"
            write_case_summary "${case_name}" "[UPLOAD-FAIL]" "elapsed=${elapsed_fmt} see ${upload_log}"
            echo "RUN-FAIL ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
            return
        fi

        sleep "${TIMEOUT}"
        if ! probe_target "${elf_path}" "${gdb_log}"; then
            end_time=$(now_ms)
            elapsed_ms=$((end_time - start_time))
            elapsed_fmt=$(format_ms_seconds "${elapsed_ms}")
            echo "[GDB-FAIL] ${case_name} elapsed=${elapsed_fmt}"
            write_case_summary "${case_name}" "[GDB-FAIL]" "elapsed=${elapsed_fmt} see ${gdb_log}"
            echo "RUN-FAIL ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
            return
        fi

        detail=$(classify_board_case "${case_name}" "${elf_path}" "${gdb_log}" "${exception_log}")
        end_time=$(now_ms)
        elapsed_ms=$((end_time - start_time))
        elapsed_fmt=$(format_ms_seconds "${elapsed_ms}")

        case "${detail}" in
            PASS*)
                echo "[PASS] ${case_name} elapsed=${elapsed_fmt}"
                write_case_summary "${case_name}" "[PASS]" "elapsed=${elapsed_fmt} ${detail}"
                echo "PASS ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
                ;;
            FAIL*)
                echo "[FAIL] ${case_name} elapsed=${elapsed_fmt}"
                write_case_summary "${case_name}" "[FAIL]" "elapsed=${elapsed_fmt} ${detail}"
                echo "FAIL ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
                ;;
            NOT-FINISHED*)
                echo "[TIMEOUT] ${case_name} elapsed=${elapsed_fmt}"
                write_case_summary "${case_name}" "[TIMEOUT]" "elapsed=${elapsed_fmt} ${detail}"
                echo "TIMEOUT ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
                ;;
            EXCEPTION*)
                echo "[EXCEPTION] ${case_name} elapsed=${elapsed_fmt}"
                write_case_summary "${case_name}" "[EXCEPTION]" "elapsed=${elapsed_fmt} ${detail}"
                echo "FAIL ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
                ;;
            GDB-FAIL*|PARSE-FAIL*)
                echo "[RUN-FAIL] ${case_name} elapsed=${elapsed_fmt}"
                write_case_summary "${case_name}" "[RUN-FAIL]" "elapsed=${elapsed_fmt} ${detail}"
                echo "RUN-FAIL ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
                ;;
        esac
        return
    fi

    setsid bash -lc "${run_cmd}" </dev/null >>"${run_log}" 2>&1 &
    run_pid=$!
    start_time=$(now_ms)
    verdict=

    while kill -0 "${run_pid}" 2>/dev/null; do
        verdict=$(classify_run_log "${run_log}")
        if [ "${verdict}" = "PASS" ] || [ "${verdict}" = "FAIL" ]; then
            stop_run_pid "${run_pid}"
            break
        fi

        now=$(now_ms)
        if [ $((now - start_time)) -ge $((TIMEOUT * 1000)) ]; then
            verdict=TIMEOUT
            stop_run_pid "${run_pid}"
            break
        fi

        sleep 0.2
    done

    if [ -z "${verdict}" ]; then
        wait "${run_pid}"
        rc=$?
        verdict=$(classify_run_log "${run_log}")
    else
        wait "${run_pid}" 2>/dev/null || true
        rc=0
    fi

    end_time=$(now_ms)
    elapsed_ms=$((end_time - start_time))
    elapsed_fmt=$(format_ms_seconds "${elapsed_ms}")

    case "${verdict}" in
        PASS)
            echo "[PASS] ${case_name} elapsed=${elapsed_fmt}"
            write_case_summary "${case_name}" "[PASS]" "runner=${RUNNER} elapsed=${elapsed_fmt} elf=${elf_path}"
            echo "PASS ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
            ;;
        FAIL)
            echo "[FAIL] ${case_name} elapsed=${elapsed_fmt}"
            write_case_summary "${case_name}" "[FAIL]" "runner=${RUNNER} elapsed=${elapsed_fmt} elf=${elf_path}"
            echo "FAIL ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
            ;;
        TIMEOUT)
            if [ "${rc}" -ne 124 ] && [ "${rc}" -ne 0 ]; then
                echo "[RUN-FAIL] ${case_name} elapsed=${elapsed_fmt}"
                write_case_summary "${case_name}" "[RUN-FAIL]" "runner=${RUNNER} rc=${rc} elapsed=${elapsed_fmt} elf=${elf_path}"
                echo "RUN-FAIL ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
            else
                echo "[TIMEOUT] ${case_name} elapsed=${elapsed_fmt}"
                write_case_summary "${case_name}" "[TIMEOUT]" "runner=${RUNNER} timeout=${TIMEOUT}s elapsed=${elapsed_fmt} elf=${elf_path}"
                echo "TIMEOUT ${case_name} ${elapsed_ms}" >>"${RESULTS_FILE}"
            fi
            ;;
    esac
}

while IFS= read -r elf_path; do
    [ -e "${elf_path}" ] || continue

    run_one_elf "${elf_path}" &
    while [ "$(jobs -pr | wc -l)" -ge "${PARALLEL}" ]; do
        wait -n || true
    done
done < <(collect_elf_paths)

wait

SCRIPT_END_TIME_MS=$(now_ms)
TOTAL_ELAPSED_MS=$((SCRIPT_END_TIME_MS - SCRIPT_START_TIME_MS))
SUM_CASE_MS=$(awk '{sum += $3} END {print sum + 0}' "${RESULTS_FILE}" 2>/dev/null || echo 0)
SUM_CASE_ELAPSED_FMT=$(format_ms_seconds "${SUM_CASE_MS}")
TOTAL_ELAPSED_FMT=$(format_ms_seconds "${TOTAL_ELAPSED_MS}")

{
    pass_count=$(grep -c '^PASS ' "${RESULTS_FILE}" 2>/dev/null || true)
    fail_count=$(grep -c '^FAIL ' "${RESULTS_FILE}" 2>/dev/null || true)
    timeout_count=$(grep -c '^TIMEOUT ' "${RESULTS_FILE}" 2>/dev/null || true)
    run_fail_count=$(grep -c '^RUN-FAIL ' "${RESULTS_FILE}" 2>/dev/null || true)
    if [ "${fail_count}" -ne 0 ] || [ "${timeout_count}" -ne 0 ] || [ "${run_fail_count}" -ne 0 ]; then
        overall_rc=1
    fi

    echo
    echo "sum_case_elapsed=${SUM_CASE_ELAPSED_FMT}"
    echo "total_elapsed=${TOTAL_ELAPSED_FMT}"
    echo "totals pass=${pass_count} fail=${fail_count} timeout=${timeout_count} run_fail=${run_fail_count}"
} >>"${SUMMARY_LOG}"

echo "Summary: pass=${pass_count} fail=${fail_count} timeout=${timeout_count} run_fail=${run_fail_count}"
echo "Elapsed: case_sum=${SUM_CASE_ELAPSED_FMT} total=${TOTAL_ELAPSED_FMT}"
echo "Logs: ${LOG_DIR}"

exit "${overall_rc}"
