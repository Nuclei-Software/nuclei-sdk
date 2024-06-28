# N100 Software Development Kit

> [!NOTE]
> If you want to develop and run program on Nuclei **200/300/600/900/100**0 series RISC-V CPU, please switch to **develop** or **master** branch.

This **N100 SDK** is modified based on the **Nuclei SDK** framework, **NMSIS** is also a modified version to support limited feature provided by Nuclei 100 series processor.

We also intergated some RTOSes into Nuclei SDK, which are **FreeRTOS**, **UCOSII** and **RTThread**, you can easily find it in the *OS* folder.

Both Nuclei riscv gcc and IAR compiler are supported in Nuclei N100 SDK, for IAR compiler support, see [ideprojects/iar/README.md](ideprojects/iar/README.md).

## Quick Startup

Wanner to a try with **Nuclei N100 SDK**, click [Quick Start with Nuclei N100 SDK](https://doc.nucleisys.com/nuclei_n100_sdk/quickstart.html) to start up.

## Requirements

* Ubuntu Linux >=20.04 LTS or Windows >=7
  - **Linux**: GNU Make >= 3.82
  - **Windows**: [Windows Build Tools](https://nucleisys.com/download.php)
* [Nuclei Studio >= 2024.06](https://nucleisys.com/download.php)
  - Nuclei Studio can support import Nuclei SDK NPK package to provide IDE project wizard create/build/debug.
  - Nuclei Studio also contains same release of Nuclei Toolchain/QEMU/OpenOCD, so just download IDE is enough.

## How to use

1. Create and modify your own setup config, see comments in `setup.bat` or `setup.sh`
   * For **Linux**, create `setup_config.sh` in **$NUCLEI_SDK_ROOT**.
   * For **Windows**, create `setup_config.bat` in **%NUCLEI_SDK_ROOT%**.
2. Source the environment script right in **NUCLEI_SDK_ROOT**
   * For **Linux**: `source setup.sh`
   * For **Windows**: `setup.bat`
3. Build and run application.
   * **Note:** By default, the SoC and Board is set to ``evalsoc`` and ``nuclei_fpga_eval``,
     if you don't pass any **SOC** and **BOARD** variable in Make command,
     it will use the default SoC and Board.
   * Assume that you will run this application -> *application/baremetal/helloworld/*.
   * cd *application/baremetal/helloworld/*
   * you can run *make help* to show help message.
   * We provided different Nuclei Core configurations(CORE=<your_core>) we supported, see *Build/Makefile.core*.
     - such as `CORE=n100m`
   * We support several download modes(DOWNLOAD=<mode>) for different applications.
     - **ilm**: Program will be download into ilm/ram and run directly in ilm/ram, program lost when poweroff
     - **flashxip**: Program will to be download into flash and run directly in Flash
   * For example, if you want to build your application for *CORE=n100m DOWNLOAD=ilm*, you can easily run this command:
     ~~~shell
     make CORE=n100m DOWNLOAD=ilm all
     ~~~
   * If you want to upload your application for *CORE=n100m DOWNLOAD=ilm*, you can easily run this command:
     ~~~shell
     make CORE=n100m DOWNLOAD=ilm upload
     ~~~
   * (Option 1)If you want to debug your application for *CORE=n100m DOWNLOAD=ilm*:
     - First open a new terminal in the same application folder and run: `make CORE=n100m DOWNLOAD=ilm run_openocd`
     - Then run this command `make CORE=n100m DOWNLOAD=ilm run_gdb` in the existing terminal, then you can debug it using gdb,
       if you want to load your program, you need to type `load` to achieve it.
     - **Notice**: Since version 0.2.4, you can also pass extra `GDB_PORT=<portno>`, to change to use new gdb port other than default
       `3333`, for example, `make CORE=n100m DOWNLOAD=ilm GDB_PORT=3344 run_openocd` and `make CORE=n100m DOWNLOAD=ilm GDB_PORT=3344 run_gdb`
   * (Option 2)If you want to debug your application for *CORE=n100m DOWNLOAD=ilm*:
     ~~~shell
     make CORE=n100m DOWNLOAD=ilm debug
     ~~~
   * If you want to use UART terminal tool to view the UART message, you can choose `screen` or `minicom` in Linux, `teraterm` in Windows, the default UART baudrate we use is `115200`.

## Knowledge book

* N100 SDK only support **Nuclei RISC-V Toolchain >= 2024.06**, and toolchain prefix is `riscv64-unknown-elf-`.

* N100 SDK is modified based on Nuclei SDK, so the user experience are similiar to Nuclei SDK, and directory structure is also similar,
  if you have experience in using Nuclei SDK, it will be easy to use N100 SDK.

