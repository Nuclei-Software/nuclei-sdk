# ThreadX UP Regression 使用说明

这个目录提供 Nuclei SDK 的 ThreadX UP regression 工程，用于批量验证
`OS/ThreadX/ports/nuclei` 以及相关 ThreadX 配置在目标板上的行为。

当前目录的主要内容如下：

- `cases/*.c`
  ThreadX regression 测试用例，每次通过 `CTEST=<case>` 选择一个 case。
- `testcontrol.c`
  统一的测试统计变量，`test.sh` 通过 GDB 读取这些变量判断结果。
- `test.sh`
  批量构建、下载、等待、GDB 探测和日志归档脚本。
- `run_elfs.sh`
  对已生成的 per-case ELF 做本地批量运行和结果归档。

## 适用场景

- 修改 `OS/ThreadX/ports/nuclei` 后做回归验证
- 修改 ThreadX 相关中断、调度、时间片或内存对象路径后做上板测试
- 需要对单个 case 或整套 regression 做批量验证和日志保存

## 构建单个 Case

在本目录下可以直接构建指定 case：

```bash
make CORE=n900fd clean
make CORE=n900fd CTEST=cases/threadx_block_memory_basic_test.c -j all
```

如果要直接下载并运行指定 case：

```bash
make CORE=n900fd GDBREMOTE=whss3:22600 CTEST=cases/threadx_block_memory_basic_test.c upload
```

说明：

- `CORE` 需要和目标板匹配
- 当前工程默认要求 `ECLIC` 和 `SYSTIMER`
- `threadx_initialize_kernel_setup_test.c` 是独立 kernel setup 用例，不适合当前统一 `CTEST` 框架，默认不参与批量运行

## 批量上板测试

最常用方式是直接跑 `test.sh`：

```bash
./test.sh whss3:22600
```

常用环境变量：

- `CORE`
  传给 `make` 的 core 名称，默认 `n900fd`
- `WAIT_SECONDS`
  `upload` 后等待目标运行的时间，默认 `15`
- `GDB_TIMEOUT`
  每次 GDB 会话超时时间，默认 `15`
- `CASES`
  空格分隔的 case 名称或路径，只跑指定用例
- `BUILD_ONLY`
  设为 `1` 时只构建和归档 ELF，不做 `upload` 和 GDB 探测
- `CASE_LIST_FILE`
  从文件读取 case 列表，或复用历史 `summary.log`
- `CASE_SET`
  从历史 `summary.log` 中筛选 `pass` / `fail` / `exception` / `skip` / `not-finished`
- `LOG_DIR`
  显式指定日志目录

例子：

```bash
WAIT_SECONDS=15 ./test.sh whss3:22600
CASES="threadx_thread_simple_sleep_test threadx_mutex_basic_test" ./test.sh whss3:22600
BUILD_ONLY=1 CORE=n900fd ./test.sh
CASE_LIST_FILE=logs/20260518_104419/summary.log CASE_SET=fail ./test.sh whss3:22600
```

## 结果判定

`test.sh` 通过 GDB 读取 `testcontrol.c` 里的统计变量，按下面规则判定：

- `[PASS]`
  `success=1 fail=0 syserr=0 status=0`
- `[FAIL]`
  统计变量已更新，但结果不满足预期
- `[NOT-FINISHED]`
  等待超时后统计变量仍未更新
- `[EXCEPTION]`
  GDB 回溯命中 trap/exception 处理路径
- `[CLEAN-FAIL]` / `[BUILD-FAIL]` / `[UPLOAD-FAIL]` / `[GDB-FAIL]` / `[PARSE-FAIL]`
  分别表示 clean、构建、下载、GDB 探测或解析失败
- `[SKIP]`
  脚本已知不适合当前裸机 CTEST 框架的用例

当前默认跳过的 case 包括：

- `threadx_initialize_kernel_setup_test`
- `threadx_trace_basic_test`

## 生成的文件

每次运行会生成：

- `elfs/<core>/<case>.elf`
  每个 case 对应的 ELF
- `logs/<run_id>/`
  运行日志目录

日志目录下常见文件包括：

- `summary.log`
  汇总每个 case 的结果
- `<case>.upload.log`
  clean / build / upload 过程日志
- `<case>.gdb.log`
  GDB 探测日志
- `<case>.exception.log`
  异常 case 的额外回溯日志

## 本地 ELF 批量运行

如果已经有 `elfs/<core>/` 下的 case ELF，可以直接运行：

```bash
RUNNER=qemu CORE=n900fd ./run_elfs.sh
```

也可以只跑部分 case：

```bash
CASES="threadx_thread_simple_sleep_test threadx_mutex_basic_test" \
RUNNER=qemu CORE=n900fd ./run_elfs.sh
```

`run_elfs.sh` 支持：

- `RUNNER=qemu|xlmodel`
- `CORE`
- `PARALLEL`
- `TIMEOUT`
- `CASES`
- `ELF_DIR`
- `LOG_DIR`

## 运行注意事项

- 上板测试需要一个可访问的 `GDBREMOTE=<host:port>`
- `test.sh` 和 `run_elfs.sh` 都依赖 GDB 或仿真器可正常启动
- 如果更换 `CORE`，建议先执行一次 `clean`
- 当前目录生成的 `elfs/`、`logs/`、`*.elf`、`*.map`、`*.o`、`*.o.d` 已通过本目录 `.gitignore` 忽略
