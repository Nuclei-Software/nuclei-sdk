# Profiling For RISC-V

Profiling and coverage analysis tools like gprof and gcov play a vital role in identifying performance bottlenecks
and untested regions within source code, ultimately leading to improved software efficiency and quality. 

Gprof (GNU Profiler) instruments the compiled binary, adding code to track the entry and exit of each function and record timestamps.
This enables it to measure the exact execution time of each function, distinguishing between self-time (time spent within the function itself) and inclusive time (time spent in the function and its callees).
Additionally, Gprof constructs a call graph based on function call frequencies, visualizing the hierarchical structure of function interactions.

Gprof is primarily used for performance analysis and optimization. By providing detailed timing data and a call graph, it helps developers:

- Locate performance bottlenecks: Identifying functions that consume excessive time or have inefficient implementations.
- Understand program flow: Revealing the relationships between functions and their call frequencies within the program.
- Guide optimization efforts: Offering precise timing information to target specific functions or call patterns for improvement.
Gcov (GNU Coverage Analyzer)

Gcov instruments the compiled code to monitor which lines and branches are executed during testing. It records the number of times each line is executed and tracks the coverage of conditional branches, ensuring comprehensive coverage assessment.

Gcov serves as a code coverage analysis tool, primarily used for evaluating test suite thoroughness and enhancing software quality. It helps developers:

- Measure test coverage: Quantifying the percentage of code exercised during testing, including line and branch coverage.
- Identify untested or under-tested code: Generating annotated source code files that highlight covered and uncovered lines and branches, enabling targeted improvement of the test suite.
- Promote comprehensive testing: Encouraging thorough testing practices and the development of a robust test suite by pinpointing areas needing additional coverage.

For more details, refer to the following links:

- gprof: https://sourceware.org/binutils/docs/gprof/index.html
- gcov: https://gcc.gnu.org/onlinedocs/gcc/gcov/introduction-to-gcov.html

In this directory, we provide utils source code to do profiling and coverage using gcov and gprof
technology.

## Profiling util files

- `gcov.c` & `gcov_api.h`: Collect coverage data after program executed
   - You should add extra `-coverage` compiler option
   to the source files you want to collect coverage information.
- `gprof.c`, `gprof_api.h` & `gprof_stub.c`: Collect profiling data after program executed
   - You should add extra `-pg` compiler option to the source files you want to collect profiling information.
   - `gprof_stub.c` contains some stub functions required to setup a period interrupt to do program sampling required by gprof,
   you need to customize it by yourself. For details, please check the `gprof_stub.c` file by yourself.
   - The sampling period is controlled by `PROF_HZ`(1000 means 1ms, 10000 means 100us) defined in `gprof_api.h`
   - and you should also set correct `PROGRAM_LOWPC` and `PROGRAM_HIGHPC`
   defined in `gprof_api.h`

- `parse.py`: a python script use to parse gcov and gprof dump log file, and generate gcov or gprof binary files.
  To run this script, need python3 installed in your host pc.

## How to use

Add this `profiling` folder into your project, and do steps below:

- Modify `gprof_stub.c` and `gprof_api.h` if you want to collect profiling information,
  and add extra `-pg` compiler option to the source files you want to profile.

- Add extra `-coverage` option to the source files you want to coverage.

- Call `gprof_collect(interface)` after the key program you want to profile

- Call `gcov_collect(interface)` after the key program you want to coverage

- `interface` can be `0`, `1`, `2`

  - `0`: collect gprof or gcov data in buffer, you can use gdb script to dump gcov or gprof binary files when you are debug the program.
  - `1`: require semihosting or file open/close is supported! It will directly write the gprof or gcov data into files.
  - `2`: dump gcov or gprof data directly in console, and you can copy all the log and save it in a file, such as `prof.log`, and then
    you can use the `parse.py` script to analyze the log file and dump it as binary into your project folder, such as `python3 /path/to/parse.py prof.log`

In Nuclei Studio, for `interface == 2`, you can directly choose the console window, and select all the log, and right click `Parse and generate HexDump`,
for `interface == 0`, in debug mode, you can select a thread, and right click on it, and you can click `Dump Gcov` or `Dump Gprof` to generate binary files.

Adding extra compiler options can be done in Nuclei Studio via steps below:

In Nuclei Studio, you can click the source file or source folder you want to add extra options,
and right click `Properities`, and add flags in `C/C++ Build -> Settings -> GNU RISC-V Cross Assembler/C Compiler/C++ Compiler -> Miscellaneous -> Other xxx flags`,
and then click `Apply and Close`, and then rebuild this program.

![Set Profiling Options in Nuclei Studio](images/profiling_options_in_ide.png)
