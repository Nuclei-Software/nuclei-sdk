# Profiling and Coverage For Nuclei RISC-V CPU In Nuclei Studio IDE

> [!NOTE]
> The profiling middleware uses `%02` format specifier in its printf statements. Ensure your C library supports this format.

Profiling and coverage analysis tools like gprof and gcov play a vital role in identifying performance bottlenecks
and untested regions within source code, ultimately leading to improved software efficiency and quality.

## Gprof (GNU Profiler)

Gprof (GNU Profiler) instruments the compiled binary, adding code to track the entry and exit of each function and record timestamps.
This enables it to measure the exact execution time of each function, distinguishing between self-time (time spent within the function itself) and inclusive time (time spent in the function and its callees).
Additionally, Gprof constructs a call graph based on function call frequencies, visualizing the hierarchical structure of function interactions.

Gprof is primarily used for performance analysis and optimization. By providing detailed timing data and a call graph, it helps developers:

- Locate performance bottlenecks: Identifying functions that consume excessive time or have inefficient implementations.
- Understand program flow: Revealing the relationships between functions and their call frequencies within the program.
- Guide optimization efforts: Offering precise timing information to target specific functions or call patterns for improvement.

> [!NOTE]
> Gprof requires a period interrupt to do program sampling. You must customize the `gprof_stub.c` file to set up the timer interrupt for your specific hardware.

## Gcov (GNU Coverage Analyzer)

Gcov instruments the compiled code to monitor which lines and branches are executed during testing. It records the number of times each line is executed and tracks the coverage of conditional branches, ensuring comprehensive coverage assessment.

Gcov serves as a code coverage analysis tool, primarily used for evaluating test suite thoroughness and enhancing software quality. It helps developers:

- Measure test coverage: Quantifying the percentage of code exercised during testing, including line and branch coverage.
- Identify untested or under-tested code: Generating annotated source code files that highlight covered and uncovered lines and branches, enabling targeted improvement of the test suite.
- Promote comprehensive testing: Encouraging thorough testing practices and the development of a robust test suite by pinpointing areas needing additional coverage.

## Toolchain Support and Compatibility

This profiling component now supports both **GCC** and **LLVM/Clang** toolchains for gprof and gcov functionality:

| Toolchain | gprof Support | gcov Support | gcov Data Format |
|-----------|---------------|--------------|------------------|
| GCC (`riscv64-unknown-elf-gcc`) | ✓ | ✓ | GCC gcov format |
| LLVM/Clang (`riscv64-unknown-elf-clang`) | ✓ | ✓ | LLVM gcov format |

> [!IMPORTANT]
> **GCC and LLVM/Clang generate incompatible gcov data formats.** You must use the corresponding tool to parse the coverage data:
>   - For GCC-compiled code: use `riscv64-unknown-elf-gcov` to parse coverage information
>   - For LLVM/Clang-compiled code: use `llvm-cov gcov` to parse coverage information
>
> **Nuclei Studio IDE Coverage Support:**
>   - Nuclei Studio IDE supports both GCC and LLVM/Clang gcov format coverage data visualization
>   - For command-line analysis, use the corresponding gcov tool for your toolchain:
>     ```bash
>     # For GCC toolchain
>     riscv64-unknown-elf-gcov <your-compiled-file>.gcda
>
>     # For LLVM/Clang toolchain
>     llvm-cov gcov <your-compiled-file>.gcda
>     ```

In this directory, we provide utils source code to do profiling and coverage using gcov and gprof
technology.

For more details about gprof and gcov, refer to the following links:

- gprof: https://sourceware.org/binutils/docs/gprof/index.html
- gcov: https://gcc.gnu.org/onlinedocs/gcc/gcov/introduction-to-gcov.html
- llvm coverage: https://clang.llvm.org/docs/SourceBasedCodeCoverage.html

## Profiling util files

- `gcov.c` & `gcov_api.h`: Collect coverage data after program executed
   - You should add extra `-coverage` compiler option to the source files you want to collect coverage information.
   - **Note:** Both GCC and LLVM/Clang support `-coverage` option, but they generate different gcov data formats
- `gprof.c`, `gprof_api.h` & `gprof_stub.c`: Collect profiling data after program executed
   - You should add extra `-pg` compiler option to the source files you want to collect profiling information.
   - **Modify** `gprof_stub.c` contains some stub functions required to setup a period interrupt to do program sampling required by gprof,
     you **must customize it by yourself**. For details, please read the `gprof_stub.c` file **carefully** by yourself.
   - The sampling period is controlled by `PROF_HZ`(1000 means 1ms, 10000 means 100us) defined in `gprof_api.h`
   - and you should also set correct `PROGRAM_LOWPC` and `PROGRAM_HIGHPC` defined in `gprof_api.h`

- `parse.py`: a python script use to parse gcov and gprof dump log file, and generate gcov or gprof binary files.
  To run this script, need python3 installed in your host pc.

- `dump_gcov.gdb`: gdb script to dump coverage data when you execute `gcov_collect(0);` in your application code.

- `dump_gprof.gdb`: gdb script to dump profiling data when you execute `gprof_collect(0);` in your application code.

You can execute above gdb script in Debug Console like this `source /path/to/dump_gcov.gdb`.

## Example Application

For a complete working example of how to use this profiling component, refer to the [demo_profiling](https://doc.nucleisys.com/nuclei_sdk/design/app.html#demo-profiling) application in Nuclei SDK.

The ``demo_profiling`` example demonstrates:

- How to integrate the profiling component into your project
- How to configure compiler flags (`-coverage` for gcov, `-pg` for gprof)
- How to call `gcov_collect(x)` and `gprof_collect(x)` functions
- How to dump and analyze coverage and profiling data

You can find the example at ``application/baremetal/demo_profiling`` in the Nuclei SDK.

## Key Reminders

> [!IMPORTANT]
> **Apply profiling/coverage flags only to the application being analyzed.**
> Do not enable `-pg` or `-coverage` globally for the whole SDK or unrelated components. These options should be added only to the source files or directories of the target application; otherwise they can introduce unexpected build, link, runtime, or memory issues.

> [!IMPORTANT]
> **Profiling and coverage need a large heap.**
> Both gprof and gcov may allocate a significant amount of runtime memory, especially coverage collection. Make sure your heap is large enough before enabling these features.
>
> The `demo_profiling` example is intended to run with `DOWNLOAD=sram` or `DOWNLOAD=ddr` so that more runtime memory is available for profiling experiments.
> In the linker script, `__HEAP_SIZE` only defines the minimum reserved heap space. The actual heap available to `malloc` is bounded by `__heap_start` and `__heap_end`, so it also depends on the final linker layout and remaining free RAM.
> To reduce integration issues, it is recommended to use a memory layout with enough free RAM and verify the effective heap size in your own project.

> [!TIP]
> **This profiling component can be reused independently.**
> You can copy the profiling library sources into another project and integrate them without moving the whole example application. A practical reference is the Nuclei Studio IDE flow below: add the required profiling sources, configure the library, then enable profiling or coverage compiler flags only for the application code you want to analyze.

## How to Use

### Step 1: Add Profiling Component to Your Project

Add this `profiling` folder into your project or create project based on ``demo_profiling`` example, then configure your build system.

This component is designed to be integrated as a reusable library:

- Add the required profiling sources and headers into your project
- Configure the profiling library as needed for your platform
- Enable `-pg` or `-coverage` only on the application source files you want to analyze
- Do not apply these options globally to the whole SDK or all middleware

#### For Nuclei Studio IDE

1. Right-click on the source file or folder you want to profile/coverage
2. Select **Properties** → **C/C++ Build** → **Settings**
3. Navigate to **GNU RISC-V Cross Assembler/C Compiler/C++ Compiler** → **Miscellaneous** → **Other xxx flags**
4. Add the following flags:
   - `-pg` for profiling
   - `-coverage` for coverage analysis
5. Apply the flags only to the selected application source file or source folder that needs profiling/coverage
6. Click **Apply and Close**, then rebuild your project

![Set Profiling Options in Nuclei Studio](images/profiling_options_in_ide.png)

#### For Nuclei SDK Makefile-based Build System

Edit your ``Makefile`` to configure profiling and coverage support:

```makefile
# Enable profiling middleware
MIDDLEWARE := profiling

# Source and include directories
SRCDIRS = . src
INCDIRS = . inc

# Specify directories that need profiling/coverage compilation flags
# If left empty, APP_COMMON_FLAGS will be applied to all source files
APPDIRS := . src

# Application-specific compilation flags
# -pg enables profiling instrumentation
# -coverage enables coverage analysis (requires more memory)
APP_COMMON_FLAGS := -pg -coverage

# Memory configuration - profiling/coverage require significant memory
# Use 'sram' or 'ddr' download mode
DOWNLOAD ?= sram

# Disable optimization for accurate profiling results
COMMON_FLAGS := -O0

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
```

> [!TIP]
> **Memory Considerations:**
> - Profiling and coverage analysis require significant memory
> - Use ``DOWNLOAD=sram`` or ``DOWNLOAD=ddr`` for sufficient runtime memory
> - When using ``-coverage`` flag, heap space may be insufficient - consider ``DOWNLOAD=ddr``
> - Increase heap size explicitly if your application or test case uses gcov/gprof buffers heavily
> - Avoid `DOWNLOAD=ilm` for such examples unless you have verified the available memory is still sufficient
>
> **Optimization Considerations:**
> - Use ``-O0`` optimization level for accurate profiling results
> - Higher optimization levels can interfere with profiling accuracy
>
> **Extension Compatibility:**
> - When using Zc extension with ``-pg``, note that ``-fomit-frame-pointer`` (enabled by Zc) is incompatible with ``-pg``
> - You may need to adjust compiler flags accordingly
>
> **Scope of Instrumentation:**
> - Set `APPDIRS` or equivalent build rules so that `-pg` and `-coverage` are added only to the target application sources
> - Do not enable these flags for the whole SDK by default

### Step 2: Customize gprof_stub.c (For Profiling Only)

If you want to collect profiling information, you **must customize** `gprof_stub.c`:

- The file contains stub functions required to setup a period interrupt for program sampling
- The sampling period is controlled by `PROF_HZ` (1000 = 1ms, 10000 = 100us) defined in `gprof_api.h`
- You need to set correct `PROGRAM_LOWPC` and `PROGRAM_HIGHPC` in `gprof_api.h`
- Read the `gprof_stub.c` file **carefully** and modify it for your specific hardware

### Step 3: Call Collect Functions in Your Code

Add the following calls after the key program section you want to profile/coverage:

```c
// For profiling
gprof_collect(interface);

// For coverage
gcov_collect(interface);
```

Where `interface` can be:

- **`0`**: Collect data in memory buffer
  - During debugging, use GDB scripts to dump the binary files:
    - In IDE Debug Console or command-line GDB, run:
      `source /path/to/dump_gcov.gdb` (for coverage) or `source /path/to/dump_gprof.gdb` (for profiling)
  - The binary files (`gmon.out`, `*.gcda`) will be generated in your project directory
- **`1`**: Write data directly to files (requires semihosting)
  - Enable semihosting in project settings or during project creation
  - May need to link ``c_nano`` library if using the nano C library
  - Files (`gmon.out`, `*.gcda`) are automatically written to the host filesystem
- **`2`**: Dump data to console output
  - Copy the console output to a log file (e.g., ``prof.log``)
  - Use the ``parse.py`` script to parse and generate binary files:
    `python3 /path/to/parse.py prof.log`
  - In IDE: Select all console output → Right-click → **Parse and generate HexDump**

### Step 4: Build and Run

#### In Nuclei Studio IDE

- Click **Build** (hammer icon) to compile your project
- Click **Debug** or **Run** to execute on hardware or simulator

#### In Command Line

```bash
# Build with GCC toolchain (default)
make TOOLCHAIN=nuclei_gnu -j

# Or build with LLVM/Clang toolchain
# Note: When switching toolchains, clean the build first
make TOOLCHAIN=nuclei_gnu clean
make TOOLCHAIN=nuclei_llvm -j

# Run the application on hardware
make TOOLCHAIN=nuclei_gnu upload

# Run the application on QEMU for evalsoc
make TOOLCHAIN=nuclei_gnu run_qemu

# Run the application on xlmodel for evalsoc
make TOOLCHAIN=nuclei_gnu run_xlmodel
```

> [!TIP]
> **Toolchain Selection:**
> - ``TOOLCHAIN=nuclei_gnu`` - Use GCC toolchain (default)
> - ``TOOLCHAIN=nuclei_llvm`` - Use LLVM/Clang toolchain
> - Remember: GCC and LLVM/Clang generate incompatible gcov formats
> - **Important:** When switching toolchains, always run ``make clean`` first

### Step 5: Analyze the Results

Depending on the `interface` value you used, the data files are generated differently:

#### Interface 0: Using GDB Scripts

```bash
# In GDB console (IDE Debug Console or command line gdb)
source /path/to/dump_gcov.gdb    # For coverage data
source /path/to/dump_gprof.gdb   # For profiling data
# This generates binary files (gmon.out, *.gcda) in your project directory
```

#### Interface 1: Using Semihosting

```bash
# Files are automatically written to the host filesystem
# gmon.out and *.gcda files are created in the working directory
# No additional steps needed - files are ready for analysis
```

#### Interface 2: Using Console Output

```bash
# 1. Save the console output to a log file (e.g., prof.log)
# 2. Parse the log file to generate binary data files
python3 /path/to/Components/profiling/parse.py prof.log
# This generates gmon.out and *.gcda files
```

#### Analyze the Generated Files

Once you have the binary data files (gmon.out, *.gcda), proceed with analysis:

**For Gprof analysis:**

```bash
# Use gprof to analyze the profiling data
riscv64-unknown-elf-gprof <your-elf-file> gmon.out
```

**For Gcov analysis (GCC):**

```bash
# Use riscv64-unknown-elf-gcov to generate annotated source files (.gcov)
riscv64-unknown-elf-gcov *.gcda
```

**For Gcov analysis (LLVM/Clang):**

```bash
# Use llvm-cov gcov to generate annotated source files (.gcov)
llvm-cov gcov *.gcda
```

> [!NOTE]
> **Gcov File Formats:**
> - ``.gcda`` files: Runtime coverage data generated by your program execution (different format for GCC vs LLVM/Clang)
> - ``.gcov`` files: Annotated source files generated by gcov tools showing line-by-line coverage
> - GCC and LLVM/Clang generate incompatible ``.gcda`` formats - use the corresponding toolchain's gcov tool

## FAQ

### Why do I see garbled coverage or profiling dump output, or partial dump logs?

In most cases, this means the runtime memory is insufficient, especially heap space used by gcov or gprof internal buffers.

Typical symptoms include:

- Dump output looks truncated, corrupted, or mixed with unexpected text
- Coverage dump starts but does not complete correctly
- Runtime errors disappear after increasing heap size

Check the following first:

- Make sure the application has enough heap, not only enough stack
- Verify the actual available SRAM size in your project
- Compare your linker script and `_sbrk` implementation with the SDK example, because the effective heap size may not match the number you expected from a macro or comment

The `demo_profiling` example is typically run with `DOWNLOAD=sram` or `DOWNLOAD=ddr` so that more runtime memory is available for profiling experiments.
The SDK linker script only guarantees a minimum heap reservation through `__HEAP_SIZE`. The actual heap that `malloc` can use is the range between `__heap_start` and `__heap_end`, so the effective size also depends on the linker layout, stack placement, and remaining RAM in your project.

### Why are `gmon.out` or `*.gcda` files not generated when I use console or UART output?

If you use `gprof_collect(2)` or `gcov_collect(2)`, the data is dumped to console output instead of being written directly to host files.

That means:

- UART or console output alone does not create `gmon.out` or `*.gcda` files automatically
- You must save the dump log first
- Then run `parse.py` on the host to reconstruct the binary profiling or coverage files

Example:

```bash
python3 /path/to/Components/profiling/parse.py prof.log
```

If you want the files to be written directly to the host filesystem, use interface `1` with semihosting enabled instead.

### What does `_mcleanup: tos overflow` usually mean?

This error typically indicates a profiling configuration problem, and the first items to check are:

- Heap is still too small for profiling runtime allocation
- `-pg` or `-coverage` was applied to the wrong scope
- Too much code was instrumented instead of only the target application

In practice, insufficient heap is a very common cause. If increasing heap resolves earlier dump failures but triggers `_mcleanup: tos overflow`, review both memory size and instrumentation scope together.

### Should I add `-pg` and `-coverage` to the whole SDK or all source files?

No. These flags should only be added to the application code you want to analyze.

Do not enable them globally for:

- The whole SDK
- All middleware
- All BSP or driver code
- Unrelated libraries

Instrumenting too much code increases memory usage and may introduce unexpected profiling behavior. In IDE projects, apply the flags only to the target source file or source folder. In Makefile projects, restrict the flags through `APPDIRS` or equivalent per-application build rules.

### I already configured heap to 2 KB, 20 KB, or 40 KB. Why can it still fail?

Because the configured heap number alone is not enough evidence that the runtime can actually allocate that much memory.

Please verify:

- The linker script really leaves enough heap region at runtime
- `_sbrk` is implemented correctly
- Stack, heap, and other runtime sections are not overlapping
- Your SDK port uses the same memory layout assumptions as the Nuclei SDK example

In one real integration case, profiling still failed at 40 KB heap and started working only after increasing heap to 80 KB. If the usage flow is otherwise correct, treat allocation failure as the first debugging direction.
