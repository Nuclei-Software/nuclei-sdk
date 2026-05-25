#!/usr/bin/env bash

# ThreadX regression 批量测试脚本。
#
# 对每个 `cases/*.c` 用例，脚本按以下流程执行：
# - `make clean`
# - `make upload`
# - 等待目标板运行一段时间
# - 使用 GDB 连接远端并读取 `testcontrol.c` 中的统计变量
# - 如果检测到异常线程，则额外抓取该线程的回溯和关键寄存器
#
# 输出结论说明：
# - `[PASS]`：success=1 fail=0 syserr=0 status=0
# - `[FAIL]`：统计变量已更新，但结果表明测试失败
# - `[NOT-FINISHED]`：等待超时后统计变量仍未更新
# - `[EXCEPTION]`：GDB 回溯中命中 trap/exception 处理路径
# - `[CLEAN-FAIL]` / `[UPLOAD-FAIL]` / `[GDB-FAIL]` / `[PARSE-FAIL]`
#
# 常见用法：
#   ./test.sh <host:port>
#   WAIT_SECONDS=25 ./test.sh whss6:22000
#   BUILD_ONLY=1 CORE=n900fd SMP=4 ./test.sh
#   CASES="threadx_smp_time_slice_test" ./test.sh whss6:22000
#   CASE_LIST_FILE=logs/<run>/summary.log CASE_SET=fail ./test.sh whss6:22000

set -u

DEFAULT_GDBREMOTE=whss6:22000
EXCEPTION_PATTERN='core_exception_handler|system_default_exception_handler|Exception_DumpFrame|default_intexc_handler|exc_entry|trap_entry'

usage()
{
    cat <<EOF
用法: ./test.sh [host:port]

可用环境变量：
  GDBREMOTE      远端 GDB server 地址，默认: ${DEFAULT_GDBREMOTE}
  GDB            GDB 可执行文件，默认: riscv64-unknown-elf-gdb
  CORE           传给 make 的 CORE 值，默认: nx900fd
  SMP            传给 make 的 SMP 值，默认: 4
  BUILD_ONLY     设为 1 时只构建，不 upload、不做 GDB 探测，默认: 0
  WAIT_SECONDS   upload 后等待目标运行时间，默认: 15
  GDB_TIMEOUT    每次 GDB 会话超时时间，默认: 15
  TARGET_ELF     GDB 使用的 ELF，默认: threadx_regression_smp.elf
  ELF_OUTPUT_DIR 每个 case 的 ELF 归档目录，默认: elfs/\${CORE}_smp\${SMP}
  LOG_DIR        显式指定日志目录
  CASES          空格分隔的 case 名称或路径
  CASE_LIST_FILE 包含 case 列表的文件，或历史 summary.log
  CASE_SET       从 summary.log 中筛选: not-finished/fail/exception/skip/pass
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

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    usage
    exit 0
fi

GDBREMOTE=${1:-"${GDBREMOTE:-${DEFAULT_GDBREMOTE}}"}
GDB=${GDB:-riscv64-unknown-elf-gdb}
CORE=${CORE:-nx900fd}
SMP=${SMP:-4}
BUILD_ONLY=${BUILD_ONLY:-0}
WAIT_SECONDS=${WAIT_SECONDS:-15}
GDB_TIMEOUT=${GDB_TIMEOUT:-15}
TARGET_ELF=${TARGET_ELF:-threadx_regression_smp.elf}
ELF_OUTPUT_DIR=${ELF_OUTPUT_DIR:-elfs/${CORE}_smp${SMP}}
RUN_ID=${RUN_ID:-$(date +%Y%m%d_%H%M%S)}
LOG_BASE_DIR=${LOG_BASE_DIR:-logs}
LOG_DIR=${LOG_DIR:-${LOG_BASE_DIR}/${RUN_ID}}
SUMMARY_LOG=${LOG_DIR}/summary.log
CASES=${CASES:-}
CASE_LIST_FILE=${CASE_LIST_FILE:-}
CASE_SET=${CASE_SET:-}

mkdir -p "${LOG_DIR}"
mkdir -p "${ELF_OUTPUT_DIR}"
touch "${SUMMARY_LOG}"

if [ "${BUILD_ONLY}" != "1" ]; then
    require_command "${GDB}"
    require_command timeout
fi

copy_case_elf()
{
    local case_name=$1
    local upload_log=$2
    local case_elf=${ELF_OUTPUT_DIR}/${case_name}.elf

    if [ ! -f "${TARGET_ELF}" ]; then
        echo "[ELF-MISSING] ${case_name} expected ${TARGET_ELF}" | tee -a "${upload_log}" >&2
        return 1
    fi

    cp -f "${TARGET_ELF}" "${case_elf}"
    echo "copied_elf: ${case_elf}" >>"${upload_log}"
}

probe_target()
{
    local gdb_log=$1

    timeout "${GDB_TIMEOUT}" "${GDB}" "${TARGET_ELF}" --batch -q \
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
    local thread_id=$1
    local exception_log=$2

    timeout "${GDB_TIMEOUT}" "${GDB}" "${TARGET_ELF}" --batch -q \
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

write_case_summary()
{
    local case_name=$1
    local verdict=$2
    local detail=$3

    printf '%s %s %s\n' "${verdict}" "${case_name}" "${detail}" >>"${SUMMARY_LOG}"
}

classify_case()
{
    local case_name=$1
    local gdb_log=$2
    local exception_log=$3
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

    CASE_VERDICT="[FAIL]"

    if grep -Eq "${EXCEPTION_PATTERN}" "${gdb_log}"; then
        CASE_VERDICT="[EXCEPTION]"
        exception_thread=$(find_exception_thread "${gdb_log}" || true)
        if [ -n "${exception_thread}" ]; then
            if dump_exception_context "${exception_thread}" "${exception_log}"; then
                mepc=$(extract_register_value mepc "${exception_log}")
                mcause=$(extract_register_value mcause "${exception_log}")
                mtval=$(extract_register_value mtval "${exception_log}")
                mdcause=$(extract_register_value mdcause "${exception_log}")
                msubm=$(extract_register_value msubm "${exception_log}")
                mhartid=$(extract_register_value mhartid "${exception_log}")
            fi
            echo "[EXCEPTION] ${case_name} thread=${exception_thread} mepc=${mepc:-NA} mcause=${mcause:-NA} mtval=${mtval:-NA} mdcause=${mdcause:-NA} msubm=${msubm:-NA} mhartid=${mhartid:-NA}"
            write_case_summary "${case_name}" "[EXCEPTION]" "thread=${exception_thread} mepc=${mepc:-NA} mcause=${mcause:-NA} mtval=${mtval:-NA} mdcause=${mdcause:-NA} msubm=${msubm:-NA} mhartid=${mhartid:-NA}"
        else
            echo "[EXCEPTION] ${case_name} thread=unknown"
            write_case_summary "${case_name}" "[EXCEPTION]" "thread=unknown"
        fi
        return 1
    fi

    probe_line=$(grep 'GDB_PROBE' "${gdb_log}" | tail -n 1 || true)
    if [ -z "${probe_line}" ]; then
        CASE_VERDICT="[GDB-FAIL]"
        echo "[GDB-FAIL] ${case_name}"
        write_case_summary "${case_name}" "[GDB-FAIL]" "probe-missing"
        return 1
    fi

    success_count=$(extract_probe_field success "${probe_line}")
    fail_count=$(extract_probe_field fail "${probe_line}")
    sys_error_count=$(extract_probe_field syserr "${probe_line}")
    return_status=$(extract_probe_field status "${probe_line}")

    if [ -z "${success_count}" ] || [ -z "${fail_count}" ] || [ -z "${sys_error_count}" ] || [ -z "${return_status}" ]; then
        CASE_VERDICT="[PARSE-FAIL]"
        echo "[PARSE-FAIL] ${case_name} ${probe_line}"
        write_case_summary "${case_name}" "[PARSE-FAIL]" "${probe_line}"
        return 1
    fi

    if [ "${success_count}" -eq 1 ] && [ "${fail_count}" -eq 0 ] && [ "${sys_error_count}" -eq 0 ] && [ "${return_status}" -eq 0 ]; then
        CASE_VERDICT="[PASS]"
        echo "[PASS] ${case_name} success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
        write_case_summary "${case_name}" "[PASS]" "success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
        return 0
    fi

    if [ "${success_count}" -eq 0 ] && [ "${fail_count}" -eq 0 ] && [ "${sys_error_count}" -eq 0 ]; then
        CASE_VERDICT="[NOT-FINISHED]"
        echo "[NOT-FINISHED] ${case_name} success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
        write_case_summary "${case_name}" "[NOT-FINISHED]" "success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
        return 1
    fi

    CASE_VERDICT="[FAIL]"
    echo "[FAIL] ${case_name} success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
    write_case_summary "${case_name}" "[FAIL]" "success=${success_count} fail=${fail_count} syserr=${sys_error_count} status=${return_status}"
    return 1
}

case_needs_core0_only()
{
    local case_name=$1

    case "${case_name}" in
        cases/threadx_smp_*)
            return 1
            ;;
        *)
            return 0
            ;;
    esac
}

case_wait_seconds()
{
    local case_name=$1
    local wait_seconds=${WAIT_SECONDS}

    case "${case_name}" in
        threadx_event_flag_isr_wait_abort_test|threadx_thread_wait_abort_test)
            if [ "${wait_seconds}" -lt 30 ]; then
                wait_seconds=30
            fi
            ;;
    esac

    # 统一在这里管理等待时间，后续如果需要针对慢用例单独放宽，
    # 只改这里即可，不需要动主执行流程。
    echo "${wait_seconds}"

    : "${case_name}"
}

case_skip_reason()
{
    local case_name=$1

    case "${case_name}" in
        threadx_initialize_kernel_setup_test)
            echo "standalone kernel-setup case defines its own main/tx_application_define and is not compatible with testcontrol CTEST mode"
            ;;
        threadx_trace_basic_test)
            echo "trace case still contains win32/linux-specific test hooks and is not portable to the nuclei baremetal CTEST harness"
            ;;
        *)
            return 1
            ;;
    esac
}

normalize_case_path()
{
    local case_item=$1

    case_item=${case_item#./}
    case_item=${case_item#cases/}
    case_item=${case_item%.c}
    printf 'cases/%s.c\n' "${case_item}"
}

read_cases_from_list_file()
{
    local list_file=$1
    local verdict_token=

    case "${CASE_SET}" in
        "")
            ;;
        not-finished)
            verdict_token='[NOT-FINISHED]'
            ;;
        fail)
            verdict_token='[FAIL]'
            ;;
        exception)
            verdict_token='[EXCEPTION]'
            ;;
        skip)
            verdict_token='[SKIP]'
            ;;
        pass)
            verdict_token='[PASS]'
            ;;
        *)
            echo "Unsupported CASE_SET: ${CASE_SET}" >&2
            exit 1
            ;;
    esac

    if [ -n "${verdict_token}" ]; then
        awk -v verdict="${verdict_token}" '
            $1 == verdict {
                case_name = $2
                if (case_name != "") {
                    print "cases/" case_name ".c"
                }
            }
        ' "${list_file}"
        return
    fi

    awk '
        /^[[:space:]]*$/ { next }
        /^[[:space:]]*#/ { next }
        {
            line = $0
            sub(/^[[:space:]]+/, "", line)
            sub(/[[:space:]]+$/, "", line)
            if (line ~ /^\[[A-Z-]+\][[:space:]]+/) {
                split(line, fields, /[[:space:]]+/)
                if (fields[2] != "") {
                    print "cases/" fields[2] ".c"
                }
            } else {
                sub(/^cases\//, "", line)
                sub(/\.c$/, "", line)
                print "cases/" line ".c"
            }
        }
    ' "${list_file}"
}

collect_case_paths()
{
    local selected_cases=()
    local case_item

    if [ -n "${CASES}" ]; then
        for case_item in ${CASES}; do
            selected_cases+=("$(normalize_case_path "${case_item}")")
        done
    elif [ -n "${CASE_LIST_FILE}" ]; then
        if [ ! -f "${CASE_LIST_FILE}" ]; then
            echo "Missing CASE_LIST_FILE: ${CASE_LIST_FILE}" >&2
            exit 1
        fi
        while IFS= read -r case_item; do
            [ -n "${case_item}" ] || continue
            selected_cases+=("${case_item}")
        done < <(read_cases_from_list_file "${CASE_LIST_FILE}")
    else
        selected_cases=(cases/*.c)
    fi

    printf '%s\n' "${selected_cases[@]}" | awk '!seen[$0]++'
}

overall_rc=0
pass_count=0
fail_count_total=0
exception_count=0
skip_count=0

{
    echo "run_id=${RUN_ID}"
    echo "gdbremote=${GDBREMOTE}"
    echo "gdb=${GDB}"
    echo "core=${CORE}"
    echo "smp=${SMP}"
    echo "build_only=${BUILD_ONLY}"
    echo "wait_seconds=${WAIT_SECONDS}"
    echo "gdb_timeout=${GDB_TIMEOUT}"
    echo "target_elf=${TARGET_ELF}"
    echo "elf_output_dir=${ELF_OUTPUT_DIR}"
    echo "cases=${CASES:-ALL}"
    echo "case_list_file=${CASE_LIST_FILE:-}"
    echo "case_set=${CASE_SET:-}"
    echo
} >>"${SUMMARY_LOG}"

while IFS= read -r case_path; do
    [ -e "${case_path}" ] || continue

    case_name=$(basename "${case_path}" .c)
    upload_log="${LOG_DIR}/${case_name}.upload.log"
    gdb_log="${LOG_DIR}/${case_name}.gdb.log"
    exception_log="${LOG_DIR}/${case_name}.exception.log"
    skip_reason=$(case_skip_reason "${case_name}" || true)
    case_wait=$(case_wait_seconds "${case_name}")
    if case_needs_core0_only "${case_name}"; then
        threadx_core0_only=1
    else
        threadx_core0_only=0
    fi
    build_cmd_clean="make GDBREMOTE=${GDBREMOTE} CORE=${CORE} SMP=${SMP} THREADX_CORE0_ONLY=${threadx_core0_only} SILENT=1 clean"
    if [ "${BUILD_ONLY}" = "1" ]; then
        build_cmd_action="make CORE=${CORE} SMP=${SMP} THREADX_CORE0_ONLY=${threadx_core0_only} CTEST=${case_path} SILENT=1 -j all"
    else
        build_cmd_action="make GDBREMOTE=${GDBREMOTE} CORE=${CORE} SMP=${SMP} THREADX_CORE0_ONLY=${threadx_core0_only} CTEST=${case_path} SILENT=1 -j upload"
    fi

    printf 'Test on %s (timeout=%ss)\n' "${case_path}" "${case_wait}"
    {
        echo "=== ${case_name} ==="
        echo "clean_cmd: ${build_cmd_clean}"
        echo "action_cmd: ${build_cmd_action}"
        echo "wait_seconds: ${case_wait}"
        echo "threadx_core0_only: ${threadx_core0_only}"
    } >"${upload_log}"

    if [ -n "${skip_reason}" ]; then
        echo "[SKIP] ${case_name} ${skip_reason}"
        write_case_summary "${case_name}" "[SKIP]" "${skip_reason}"
        skip_count=$((skip_count + 1))
        continue
    fi

    if ! make GDBREMOTE="${GDBREMOTE}" CORE="${CORE}" SMP="${SMP}" THREADX_CORE0_ONLY="${threadx_core0_only}" SILENT=1 clean >>"${upload_log}" 2>&1; then
        echo "[CLEAN-FAIL] ${case_name}"
        write_case_summary "${case_name}" "[CLEAN-FAIL]" "see ${upload_log}"
        fail_count_total=$((fail_count_total + 1))
        overall_rc=1
        continue
    fi

    if [ "${BUILD_ONLY}" = "1" ]; then
        if ! make CORE="${CORE}" SMP="${SMP}" THREADX_CORE0_ONLY="${threadx_core0_only}" CTEST="${case_path}" SILENT=1 -j all >>"${upload_log}" 2>&1; then
            echo "[BUILD-FAIL] ${case_name}"
            write_case_summary "${case_name}" "[BUILD-FAIL]" "see ${upload_log}"
            fail_count_total=$((fail_count_total + 1))
            overall_rc=1
            continue
        fi
    elif ! make GDBREMOTE="${GDBREMOTE}" CORE="${CORE}" SMP="${SMP}" THREADX_CORE0_ONLY="${threadx_core0_only}" CTEST="${case_path}" SILENT=1 -j upload >>"${upload_log}" 2>&1; then
        echo "[UPLOAD-FAIL] ${case_name}"
        write_case_summary "${case_name}" "[UPLOAD-FAIL]" "see ${upload_log}"
        fail_count_total=$((fail_count_total + 1))
        overall_rc=1
        continue
    fi

    if ! copy_case_elf "${case_name}" "${upload_log}"; then
        write_case_summary "${case_name}" "[ELF-MISSING]" "expected ${TARGET_ELF}"
        fail_count_total=$((fail_count_total + 1))
        overall_rc=1
        continue
    fi

    if [ "${BUILD_ONLY}" = "1" ]; then
        echo "[BUILD-ONLY] ${case_name} elf=${ELF_OUTPUT_DIR}/${case_name}.elf"
        write_case_summary "${case_name}" "[BUILD-ONLY]" "elf=${ELF_OUTPUT_DIR}/${case_name}.elf"
        pass_count=$((pass_count + 1))
        continue
    fi

    sleep "${case_wait}"

    if ! probe_target "${gdb_log}"; then
        echo "[GDB-FAIL] ${case_name}"
        write_case_summary "${case_name}" "[GDB-FAIL]" "see ${gdb_log}"
        fail_count_total=$((fail_count_total + 1))
        overall_rc=1
        continue
    fi

    if classify_case "${case_name}" "${gdb_log}" "${exception_log}"; then
        pass_count=$((pass_count + 1))
    else
        if [ "${CASE_VERDICT}" = "[EXCEPTION]" ]; then
            exception_count=$((exception_count + 1))
        else
            fail_count_total=$((fail_count_total + 1))
        fi
        overall_rc=1
    fi
done < <(collect_case_paths)

{
    echo
    echo "totals pass=${pass_count} fail=${fail_count_total} exception=${exception_count} skip=${skip_count}"
} >>"${SUMMARY_LOG}"

echo "Summary: pass=${pass_count} fail=${fail_count_total} exception=${exception_count} skip=${skip_count}"
echo "Logs: ${LOG_DIR}"

exit "${overall_rc}"
