# ThreadX SMP Regression 使用说明

这个目录提供 Nuclei SDK 上的 ThreadX SMP regression 例子，用于批量验证
`OS/ThreadX/common_smp` 和 `OS/ThreadX/ports_smp/nuclei` 在目标板上的行为是否正常。

当前目录的核心内容：

- `cases/*.c`
  ThreadX regression 测试用例，每次构建通过 `CTEST=<case>` 选择其中一个用例。
- `testcontrol.c`
  提供统一的测试统计变量，`test.sh` 通过 GDB 读取这些变量判断结果。
- `test.sh`
  批量执行脚本，负责 clean、upload、等待、GDB 探测、日志归档和异常抓取。
- `run_elfs.sh`
  本地运行脚本，直接对已生成的 per-case ELF 在 QEMU 或 xlmodel 上做批量执行和结果归档。

## 适用场景

适合以下场景：

- 修改 `OS/ThreadX/common_smp` 后做回归验证
- 修改 `OS/ThreadX/ports_smp/nuclei` 后验证 SMP 调度、锁、时间片和中断路径
- 在远端板卡上批量执行 ThreadX regression 并保留日志

## 构建和运行

### 1. 直接构建当前 regression 工程

在本目录下执行：

```bash
make CORE=n900fd DOWNLOAD=ddr clean
make CORE=n900fd DOWNLOAD=ddr -j all
```

说明：

- SMP 场景默认使用 `ddr`，因为这套 regression case 内存消耗较大
- 如果目标平台没有可用 DDR，再考虑切换到 `sram`
- `CORE`、`TOOLCHAIN`、`ARCH_EXT` 等参数可以按你的板级配置调整

### 2. 运行单个用例

例如：

```bash
make GDBREMOTE=whss3:22600 SMP=4 CTEST=cases/threadx_smp_time_slice_test.c upload
```

这里 `CTEST` 指定要运行的 case，`GDBREMOTE` 必须是有效且可达的远端调试地址。
`Makefile` 默认不设置 `CTEST`（即运行全量 regression）。如果需要进行单 case 验证，可以手动指定 `CTEST` 变量（例如指定 `CTEST=cases/threadx_smp_time_slice_test.c`），此时编译系统只会编译并运行所指定的单个测试用例。

### 3. 批量执行整套 regression

最常见方式：

```bash
WAIT_SECONDS=15 ./test.sh whss3:22600
```

`Makefile` 默认即走全量 `cases`（整合成单个 all-in-one ELF 运行），可以直接执行以下命令编译并下载运行：

```bash
make GDBREMOTE=whss3:22600 SMP=4 upload
```

此模式已验证在 n900fd 和 nx900fd SMP=4 场景下稳定运行并全部通过。

也可以只跑指定用例：

```bash
CASES="threadx_smp_time_slice_test threadx_smp_preemption_threshold_test" ./test.sh whss3:22600
```

或者从历史结果中筛选失败项重跑：

```bash
CASE_LIST_FILE=logs/20260507_160102/summary.log CASE_SET=fail ./test.sh whss3:22600
```

### 4. 只构建并归档每个 case 的 ELF

例如：

```bash
BUILD_ONLY=1 CORE=n900fd SMP=4 ./test.sh
```

默认会把每个用例构建出来的 ELF 复制到：

```text
elfs/<core>_smp<smp>/<case>.elf
```

例如：

```text
elfs/n900fd_smp4/threadx_smp_time_slice_test.elf
```

### 5. 在 QEMU 或 xlmodel 上直接运行已生成的 ELF

例如在 QEMU 上跑整套：

```bash
RUNNER=qemu CORE=n900fd SMP=4 ./run_elfs.sh
```

例如限制并行度并只跑部分 case：

```bash
PARALLEL=4 RUNNER=qemu CORE=n900fd SMP=4 \
CASES="threadx_smp_time_slice_test threadx_mutex_basic_test" \
./run_elfs.sh
```

例如在 xlmodel 上跑：

```bash
RUNNER=xlmodel CORE=nx900fd SMP=4 ./run_elfs.sh
```

`run_elfs.sh` 支持：

- `CORE` / `SMP`
- `RUNNER=qemu|xlmodel`
- `PARALLEL`
- `TIMEOUT`
- `CASES`

脚本会根据串口输出判断：

- `SUCCESS` -> `[PASS]`
- `ERROR` / `MEPC` / `MCAUSE` / `MDCAUSE` / `MTVAL` -> `[FAIL]`
- 超时无明确结果 -> `[TIMEOUT]`

同时会记录：

- 每个 case 的实际运行耗时
- 整体 wall-clock 总耗时
- 每个 case 的原始运行日志

## `test.sh` 工作方式

对每个测试用例，脚本会执行：

1. `make clean`
2. `make upload`
3. 等待 `WAIT_SECONDS` 秒
4. 使用 GDB 连接 `GDBREMOTE`
5. 读取以下统计变量：

```c
test_control_successful_tests
test_control_failed_tests
test_control_system_errors
test_control_return_status
```

6. 根据统计值输出 `[PASS]`、`[FAIL]`、`[NOT-FINISHED]`
7. 如果回溯中命中异常处理符号，则输出 `[EXCEPTION]`，并额外抓取：

- `mepc`
- `mcause`
- `mtval`
- `mdcause`
- `msubm`
- `mhartid`

## 输出结果说明

- `[PASS]`
  用例正常结束，且统计值满足 `success=1 fail=0 syserr=0 status=0`
- `[FAIL]`
  用例执行结束，但返回状态或计数结果不符合预期
- `[NOT-FINISHED]`
  等待超时后计数仍未更新，通常表示卡住、死循环、跑飞或执行时间不足
- `[EXCEPTION]`
  GDB 回溯中出现异常处理路径，通常表示 trap、非法访问或上下文损坏
- `[CLEAN-FAIL]`
  `make clean` 失败
- `[UPLOAD-FAIL]`
  `make upload` 失败
- `[GDB-FAIL]`
  GDB 无法完成连接或探测
- `[PARSE-FAIL]`
  GDB 已返回，但脚本无法解析统计输出
- `[SKIP]`
  已知不适合当前裸机 CTEST 框架的用例

## 日志文件

每次运行默认生成：

```text
logs/<时间戳>/
```

常见日志包括：

- `summary.log`
  记录运行参数、每个 case 的结果和总计统计
- `<case>.upload.log`
  记录 clean 和 upload 的输出
- `<case>.gdb.log`
  记录 GDB 探测、线程信息和 `thread apply all bt`
- `<case>.exception.log`
  仅在异常用例生成，记录异常线程回溯和寄存器

## 已知限制

### 1. 必须依赖远端 GDB 调试服务

这个例子不是纯本地自跑模式。`test.sh` 需要一个可访问的 `GDBREMOTE=<host:port>`，
脚本不会猜测远端地址，使用前必须确认 remote 有效且可达。

### 2. 结果判定依赖 GDB 和符号可见性

脚本通过 GDB 读取 `testcontrol.c` 中的全局变量判断结果，因此要求：

- ELF 与实际下载镜像匹配
- GDB 能读取目标内存
- 远端 stub 支持 `interrupt`、线程信息和基础寄存器读取

如果这些条件不满足，可能看到 `[GDB-FAIL]` 或 `[PARSE-FAIL]`。

### 3. 等待时间需要按目标速度调整

默认 `WAIT_SECONDS=15` 适合当前大多数 regression 场景，但如果：

- 目标频率较低
- 下载链路较慢
- 某些 case 本身执行时间更长

则需要适当增大 `WAIT_SECONDS`，否则可能误判为 `[NOT-FINISHED]`。

### 4. 并非所有上游用例都适合当前裸机 CTEST 框架

当前脚本会跳过至少以下两类：

- `threadx_initialize_kernel_setup_test`
  该用例自带 `main/tx_application_define`，不适合当前统一测试入口
- `threadx_trace_basic_test`
  该用例仍包含 win32/linux 相关 trace hook，不适合当前 nuclei baremetal CTEST 环境

### 5. 不是只支持 4 核，但 4 核覆盖更完整

这个 regression 工程不是只适用于 `SMP=4`。

- `test.sh` 中的 `SMP` 是可配置的，默认值是 4
- 工程 `Makefile` 中的默认 `SMP` 是 2
- ThreadX 端口层的 `TX_THREAD_SMP_MAX_CORES` 会跟随 `SMP_CPU_CNT` 配置

因此，这个例子可以用于 2 核或 4 核等大于 1 核的 SMP 场景。

但需要注意：

- 这套 regression 的目标是验证 SMP 行为，不适合作为普通单核示例
- 部分上游测试的设计背景更偏向 4 核压力场景
- 如果使用较少核数，例如 `SMP=2`，通常仍可运行，但对多核竞争、公平性和调度路径的覆盖会弱于 `SMP=4`

如果作为完整回归基线，建议优先使用 `SMP=4`；如果目标平台只有 2 核，也可以运行这套 regression，只是覆盖面会相对缩小。

### 6. SMP 场景对端口层实现较敏感

ThreadX SMP regression 对以下基础能力比较敏感：

- `OS/ThreadX/ports_smp/nuclei` 中 SMP protect 锁实现是否公平
- 中断屏蔽、上下文切换和线程调度路径是否一致

如果这些基础实现存在问题，即使测试用例本身不复杂，也可能表现为：

- 某个 CPU 长时间独占锁
- 其他 CPU 饥饿
- `tx_thread_identify()` 之类的简单热点路径触发回归失败

### 7. 当前目录是 regression 工程，不是通用 ThreadX 示例入口

这里的目标是回归验证，不是展示 ThreadX API 的最小用法。
如果要学习普通应用开发流程，建议同时参考 `application/threadx/smpdemo` 等示例。

### 8. 本地模拟器结果不能完全替代硬件结果

`run_elfs.sh` 适合作为快速筛查，但当前不能把 QEMU/xlmodel 结果直接视为和实板完全等价。

已观测到的具体例子是：

- `threadx_smp_random_resume_suspend_exclusion_test`

该 case 在 QEMU 上可能出现偶发 `[TIMEOUT]`，但相同 ELF 在实板上重复运行可以正常通过。

因此当前建议是：

- 本地模拟器结果主要用于快速回归和缩小问题范围
- 最终结论仍以实板运行结果为准，尤其是 SMP 调度、随机竞争和时间敏感类 case

## 推荐使用方式

建议在以下修改后跑一遍整套 regression：

- `OS/ThreadX/common_smp` 公共内核逻辑修改
- `OS/ThreadX/ports_smp/nuclei` 端口层、上下文切换、SMP 锁和调度修改

建议至少保留以下信息，便于后续复现和定位问题：

- 测试命令
- `GDBREMOTE`
- `WAIT_SECONDS`
- `summary.log`
- 异常 case 的 `*.exception.log`
