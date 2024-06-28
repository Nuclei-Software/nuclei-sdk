.. _quickstart:

Quick Startup
=============

.. _quickstart_using_nside:

Use Nuclei SDK in Nuclei Studio
-------------------------------

.. caution::

   If you are looking for Nuclei 100 series such as N100 support, you need to switch to
   **master_n100** or **nuclei_n100** branch of this repository to try it out.

   If you are evaluating Nuclei CPU, in future released **nuclei_gen**, you will be able
   to use the generated Nuclei SDK, please see :ref:`design_soc_evalsoc_usage`.

   For **Nuclei SDK 0.6.0** version and later ones, please use `Nuclei Studio 2024.06`_ or
   Nuclei RISC-V Toolchain/OpenOCD/QEMU 2024.06.

From `Nuclei Toolchain 2023.10`_, both gnu and llvm toolchain are provided, and toolchain
prefix changed from ``riscv-nuclei-elf-`` to ``riscv64-unknown-elf-``, and 0.5.0 SDK release
will only support this ``2023.10`` or later toolchain.

If you want to learn about how to use Nuclei Tools, see https://doc.nucleisys.com/nuclei_tools.

If you want to report issues when using Nuclei Tools or Nuclei Studio, please open issue in
https://github.com/Nuclei-Software/nuclei-studio.

Now the nuclei-sdk **released** versions are deeply integrated with Nuclei Studio IDE via menu
**RV-Tools -> NPK Package Management**, and you can directly create nuclei-sdk project in Nuclei Studio IDE
Menu **File -> New Nuclei RISC-V C/C++ Project**.

You can download **Nuclei Studio IDE** from |nuclei_download_center|, and follow
`Nuclei_Studio_User_Guide.pdf`_ to learn how to use it.

But if you want to use latest source code of Nuclei SDK, please follow the rest
part of this guide to build and run using Nuclei SDK Build System in Makefile.

.. _quickstart_setup_tools_env:

Setup Tools and Environment
---------------------------

To start to use Nuclei SDK, you need to install the following tools:

From **2020.10** release version of Nuclei Studio, you can directly use the prebuilt tools
provided in Nuclei Studio(**strongly suggested**), please following :ref:`quickstart_setup_tools_env_nside`.

.. _quickstart_setup_tools_env_nside:

Use Prebuilt Tools in Nuclei Studio
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since **2020.10** release version of Nuclei Studio, you just need to download the
**Nuclei Studio IDE** from |nuclei_download_center| for your development OS, and
no need to do the following steps below, the prebuilt tools are already included.

For example:

* In Windows, if you have extracted the Nuclei Studio IDE to ``D:\Software\NucleiStudio_IDE_202406``,
  then you can find the prebuilt tools in ``D:\Software\NucleiStudio_IDE_202406\NucleiStudio\toolchain``.
* In Linux, if you have extracted the Nuclei Studio IDE to ``/home/labdev/NucleiStudio_IDE_202406``,
  then you can find the prebuilt tools in ``/home/labdev/NucleiStudio_IDE_202406/NucleiStudio/toolchain``.

You can also update tools located in the Nuclei Studio prebuilt tools ``toolchain`` by downloading newer version
from `Nuclei Tools`_ and replace it.

.. _quickstart_get_set_nuclei_sdk:

Get and Setup Nuclei SDK
------------------------

The source code of Nuclei SDK is maintained in |github| and |gitee|.

* We mainly maintained github version, and gitee version is mirrored, just for fast access in China.
* Check source code in |github_nuclei_sdk| or |gitee_nuclei_sdk| according to your network status.
* Stable version of Nuclei SDK is maintained in **master** version, if you want release version of **Nuclei SDK**,
  please check in |github_nuclei_sdk_release|.

**Here are the steps to clone the latest source code from Github:**

* Make sure you have installed Git tool, see https://git-scm.com/download/
* Then open your terminal, and make sure git command can be accessed
* Run ``git clone https://github.com/Nuclei-Software/nuclei-sdk nuclei-sdk`` to clone source code
  into ``nuclei-sdk`` folder

  .. note::

      - If you have no access to github.com, you can also use command
        ``git clone https://gitee.com/Nuclei-Software/nuclei-sdk nuclei-sdk`` to clone from gitee.
      - If you have no internet access, you can also use pre-downloaded ``nuclei-sdk`` code,
        and use it.
      - If the backup repo is not up to date, you can import github repo in gitee by yourself, see
        https://gitee.com/projects/import/url

* Create tool environment config file for Nuclei SDK

  .. note::

    If you want to use **Terapines ZCC** toolchain, you can download it from https://www.terapines.com/, or use
    **Nuclei Studio >= 2024.06**, a **Terapines ZCC Lite** version is integrated in **<NucleiStudio>/toolchain/zcc** folder,
    and you also need to add extra **PATH** into your environment, like this:

    - **Windows**: execute ``set PATH=\path\to\zcc\bin;%PATH%`` in windows cmd terminal before run Nuclei SDK
    - **Linux**: execute ``set PATH=/path/to/zcc/bin:$PATH`` in linux shell terminal before build Nuclei SDK

  - **Windows**
     If you want to use Nuclei SDK in **Windows Command Prompt** terminal, you need to
     create ``setup_config.bat`` in ``nuclei-sdk`` folder, and open this file
     your editor, and paste the following content, assuming you followed
     :ref:`quickstart_setup_tools_env`, and prebuilt tools located in
     ``D:\Software\NucleiStudio_IDE_202406\NucleiStudio\toolchain``,
     otherwise please use your correct tool root path.

     .. code-block:: bat

        set NUCLEI_TOOL_ROOT=D:\Software\NucleiStudio_IDE_202406\NucleiStudio\toolchain

     If you want to use Nuclei SDK in **Windows PowerShell** terminal, you need to create
     a ``setup_config.ps1`` in ``nuclei-sdk`` folder, and edit this file with content below
     if your prebuilt tools are located in ``D:\Software\NucleiStudio_IDE_202406\NucleiStudio\toolchain``:

     .. code-block:: ps1

        $NUCLEI_TOOL_ROOT="D:\Software\NucleiStudio_IDE_202406\NucleiStudio\toolchain"


  - **Linux**
     Create ``setup_config.sh`` in ``nuclei-sdk`` folder, and open this file
     your editor, and paste the following content, assuming you followed
     :ref:`quickstart_setup_tools_env` and prebuilt tools located in
     ``/home/labdev/NucleiStudio_IDE_202406/NucleiStudio/toolchain``,
     otherwise please use your correct tool root path.

     .. code-block:: shell

        NUCLEI_TOOL_ROOT=/home/labdev/NucleiStudio_IDE_202406/NucleiStudio/toolchain


.. _quickstart_buildrundebug_app:

Build, Run and Debug Sample Application
---------------------------------------

Assume you have followed steps in :ref:`quickstart_get_set_nuclei_sdk` to clone
source code and create files below:

* ``setup_config.bat`` for run in **Windows Command Prompt** terminal
* ``setup_config.ps1`` for run in **Windows PowerShell** terminal
* ``setup_config.sh`` for run in **Linux Bash** terminal

To build, run and debug application, you need to open command terminal in ``nuclei-sdk``
folder.

* For **Windows** users, you can open **Windows Command Prompt** terminal and cd to ``nuclei-sdk`` folder,
  then run the following commands to setup build environment for Nuclei SDK, the output will
  be similar as this screenshot :ref:`figure_quickstart_6`:

  .. code-block:: bat
     :linenos:

     setup.bat
     echo %PATH%
     where riscv64-unknown-elf-gcc openocd make rm
     make help

  .. _figure_quickstart_6:

  .. figure:: /asserts/images/nuclei_sdk_open_cmd.png
     :width: 80 %
     :align: center
     :alt: Setup Build Environment for Nuclei SDK in Windows Command Prompt

     Setup Build Environment for Nuclei SDK in Windows Command Prompt

* For **Linux** users, you can open **Linux Bash** terminal and cd to ``nuclei-sdk`` folder,
  then run the following commands to setup build environment for Nuclei SDK, the output will
  be similar as this screenshot :ref:`figure_quickstart_7`:

  .. code-block:: shell
     :linenos:

     source setup.sh
     echo $PATH
     which riscv64-unknown-elf-gcc openocd make rm
     make help

  .. _figure_quickstart_7:

  .. figure:: /asserts/images/nuclei_sdk_open_shell.png
     :width: 80 %
     :align: center
     :alt: Setup Build Environment for Nuclei SDK in Linux Bash

     Setup Build Environment for Nuclei SDK in Linux Bash


.. note::

    * Only first line ``setup.bat`` or ``source setup.sh`` are required before build, run or debug application.
      The ``setup.bat`` and ``setup.sh`` are just used to append Nuclei RISC-V GCC Toolchain, OpenOCD and Build-Tools
      binary paths into environment variable **PATH**
    * line 2-4 are just used to check whether build environment is setup correctly, especially the **PATH** of
      Nuclei Tools are setup correctly, so we can use the ``riscv64-unknown-elf-xxx``, ``openocd``, ``make`` and ``rm``
      tools
    * If you know how to append Nuclei RISC-V GCC Toolchain, OpenOCD and Build-Tools binary paths to **PATH** variable
      in your OS environment, you can also put the downloaded Nuclei Tools as you like, and no need to run ``setup.bat``
      or ``source setup.sh``
    * If you want to run in **Windows PowerShell**, please run ``. .\setup.ps1`` instead of ``setup.bat``, and
      ``setup_config.ps1`` must be created as described in :ref:`quickstart_get_set_nuclei_sdk`.


Here for a quick startup, this guide will take board :ref:`design_board_gd32vf103v_rvstar`
for example to demostrate how to setup hardware, build run and debug application in Windows.

The demo application, we will take ``application/baremetal/helloworld`` for example.

First of all, please reuse previously build environment command terminal.

Run ``cd application/baremetal/helloworld`` to cd the ``helloworld`` example folder.

Hardware Preparation
~~~~~~~~~~~~~~~~~~~~

Please check :ref:`design_board` and find your board's page, and follow **Setup** section
to setup your hardware, mainly **JTAG debugger driver setup and on-board connection setup**.

* Power on the :ref:`design_board_gd32vf103v_rvstar` board, and use USB Type-C data cable to connect the board and your PC,
  make sure you have setup the JTAG driver correctly, and you can see JTAG port and serial port.
* Open a UART terminal tool such as |teraterm| or |minicom|, and minitor the serial port of the Board,
  the UART baudrate is *115200 bps*
* If you are building example for your own SoC and Board, please pass correct :ref:`develop_buildsystem_var_soc` and
  :ref:`develop_buildsystem_var_board` make variable. eg. If you SoC is ``evalsoc`` and Board is ``nuclei_fpga_eval``,
  just pass ``SOC=evalsoc BOARD=nuclei_fpga_eval`` to make instead of the one mentioned below. If your default board
  for this ``evalsoc`` is ``nuclei_fpga_eval``, then you don't need to pass ``BOARD=nuclei_fpga_eval``.
* If you don't pass any SOC or BOARD via make, ``evalsoc`` and ``nuclei_fpga_eval`` are default SoC and Board.


If you just want to try on **Nuclei Evaluation SoC**, no need to pass **SOC** or **BOARD**, the default value is that,
you just need to pass correct :ref:`develop_buildsystem_var_core`, :ref:`develop_buildsystem_var_archext` and :ref:`develop_buildsystem_var_download`

Build Application
~~~~~~~~~~~~~~~~~

We need to build application for this board :ref:`design_board_gd32vf103v_rvstar` using this command line:

.. note::

    * If you want to run on Nuclei Evaluation SoC, see :ref:`design_soc_evalsoc`
    * Since below steps are taking gd32vf103 SoC based board **gd32vf103v_rvstar** to do demostration,
      and when you pass ``SOC=gd32vf103``, the default BOARD will be ``gd32vf103v_rvstar``, so
      do you don't need to pass ``BOARD=gd32vf103v_rvstar``
    * You can check default SOC/BOARD/CORE information passed by using make target ``info``, eg.
      ``make SOC=gd32vf103 info``, for more information, please check :ref:`develop_buildsystem_make_targets`.

.. code-block:: shell

    # clean application if build in other application before or build for other board
    make SOC=gd32vf103 BOARD=gd32vf103v_rvstar clean
    # first build choice: using full command line
    make SOC=gd32vf103 BOARD=gd32vf103v_rvstar all
    # second build choice: using simple command line, since when SOC=gd32vf103, default BOARD is gd32vf103v_rvstar
    make SOC=gd32vf103 all

Here is the sample output of this command:

.. code-block::

    # NOTICE: You can check this configuration whether it matched your desired configuration
    Current Configuration: RISCV_ARCH=rv32imac RISCV_ABI=ilp32 SOC=gd32vf103 BOARD=gd32vf103v_rvstar CORE=n205 DOWNLOAD=flashxip
    "Assembling : " ../../../SoC/gd32vf103/Common/Source/GCC/intexc_gd32vf103.S
    "Assembling : " ../../../SoC/gd32vf103/Common/Source/GCC/startup_gd32vf103.S
    "Compiling  : " ../../../SoC/gd32vf103/Board/gd32vf103v_rvstar/Source/gd32vf103v_rvstar.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_adc.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_bkp.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_can.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_crc.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_dac.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_dbg.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_dma.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_exmc.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_exti.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_fmc.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_fwdgt.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_gpio.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_i2c.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_pmu.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_rcu.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_rtc.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_spi.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_timer.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_usart.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Drivers/gd32vf103_wwdgt.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Stubs/close.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Stubs/fstat.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Stubs/gettimeofday.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Stubs/isatty.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Stubs/lseek.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Stubs/read.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Stubs/sbrk.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/Stubs/write.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/gd32vf103_soc.c
    "Compiling  : " ../../../SoC/gd32vf103/Common/Source/system_gd32vf103.c
    "Compiling  : " hello_world.c
    "Linking    : " hello_world.elf
    text    data     bss     dec     hex filename
    13022     112    2290   15424    3c40 hello_world.elf


As you can see, that when the application is built successfully, the elf will be generated
and will also print the size information of the ``hello_world.elf``.

.. note::

    * In order to make sure that there is no application build before, you can run
      ``make SOC=gd32vf103 BOARD=gd32vf103v_rvstar clean`` to clean previously built
      objects and build dependency files.
    * About the make variable or option(**SOC**, **BOARD**) passed to make command, please refer
      to :ref:`develop_buildsystem`.

Run Application
~~~~~~~~~~~~~~~

If the application is built successfully for this board :ref:`design_board_gd32vf103v_rvstar`,
then you can run it using this command line:

.. code-block:: shell

    make SOC=gd32vf103 BOARD=gd32vf103v_rvstar upload

Here is the sample output of this command:

.. code-block::

    "Download and run hello_world.elf"
    riscv64-unknown-elf-gdb hello_world.elf -ex "set remotetimeout 240" \
            -ex "target remote | openocd -c \"gdb_port pipe; log_output openocd.log\" -f ../../../SoC/gd32vf103/Board/gd32vf103v_rvstar/openocd_gd32vf103.cfg" \
            --batch -ex "monitor halt" -ex "monitor halt" -ex "monitor flash protect 0 0 last off" -ex "load" -ex "monitor resume" -ex "monitor shutdown" -ex "quit"
    D:\Software\Nuclei\gcc\bin\riscv64-unknown-elf-gdb.exe: warning: Couldn't determine a path for the index cache directory.
    Nuclei OpenOCD, 64-bit Open On-Chip Debugger 0.10.0+dev-00014-g0eae03214 (2019-12-12-07:43)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    _start0800 () at ../../../SoC/gd32vf103/Common/Source/GCC/startup_gd32vf103.S:359
    359         j 1b
    cleared protection for sectors 0 through 127 on flash bank 0

    Loading section .init, size 0x266 lma 0x8000000
    Loading section .text, size 0x2e9c lma 0x8000280
    Loading section .rodata, size 0x1f0 lma 0x8003120
    Loading section .data, size 0x70 lma 0x8003310
    Start address 0x800015c, load size 13154
    Transfer rate: 7 KB/sec, 3288 bytes/write.
    shutdown command invoked
    A debugging session is active.

            Inferior 1 [Remote target] will be detached.

    Quit anyway? (y or n) [answered Y; input not from terminal]
    [Inferior 1 (Remote target) detached]

As you can see the application is uploaded successfully using ``openocd`` and ``gdb``,
then you can check the output in your UART terminal, see :ref:`figure_quickstart_8`.

.. _figure_quickstart_8:

.. figure:: /asserts/images/nuclei_app_helloworld_uart.png
    :width: 80 %
    :align: center
    :alt: Nuclei SDK Hello World Application UART Output

    Nuclei SDK Hello World Application UART Output


Debug Application
~~~~~~~~~~~~~~~~~

If the application is built successfully for this board :ref:`design_board_gd32vf103v_rvstar`,
then you can debug it using this command line:

.. code-block:: shell

    make SOC=gd32vf103 BOARD=gd32vf103v_rvstar debug


1. The program is not loaded automatically when you enter to debug state, just in case you want to
   debug the program running on the board.

   .. code-block::

      "Download and debug hello_world.elf"
      riscv64-unknown-elf-gdb hello_world.elf -ex "set remotetimeout 240" \
              -ex "target remote | openocd -c \"gdb_port pipe; log_output openocd.log\" -f ../../../SoC/gd32vf103/Board/gd32vf103v_rvstar/openocd_gd32vf103.cfg"
      D:\Software\Nuclei\gcc\bin\riscv64-unknown-elf-gdb.exe: warning: Couldn't determine a path for the index cache directory.
      GNU gdb (GDB) 8.3.0.20190516-git
      Copyright (C) 2019 Free Software Foundation, Inc.
      License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
      This is free software: you are free to change and redistribute it.
      There is NO WARRANTY, to the extent permitted by law.
      Type "show copying" and "show warranty" for details.
      This GDB was configured as "--host=i686-w64-mingw32 --target=riscv64-unknown-elf".
      Type "show configuration" for configuration details.
      For bug reporting instructions, please see:
      <http://www.gnu.org/software/gdb/bugs/>.
      Find the GDB manual and other documentation resources online at:
          <http://www.gnu.org/software/gdb/documentation/>.
      --Type <RET> for more, q to quit, c to continue without paging--

      For help, type "help".
      Type "apropos word" to search for commands related to "word"...
      Reading symbols from hello_world.elf...
      Remote debugging using | openocd -c \"gdb_port pipe; log_output openocd.log\" -f ../../../SoC/gd32vf103/Board/gd32vf103v_rvstar/openocd_gd32vf103.cfg
      Nuclei OpenOCD, 64-bit Open On-Chip Debugger 0.10.0+dev-00014-g0eae03214 (2019-12-12-07:43)
      Licensed under GNU GPL v2
      For bug reports, read
              http://openocd.org/doc/doxygen/bugs.html
      _start0800 () at ../../../SoC/gd32vf103/Common/Source/GCC/startup_gd32vf103.S:359
      359         j 1b

2. If you want to load the built application, you can type ``load`` to load the application.

   .. code-block::

      (gdb) load
      Loading section .init, size 0x266 lma 0x8000000
      Loading section .text, size 0x2e9c lma 0x8000280
      Loading section .rodata, size 0x1f0 lma 0x8003120
      Loading section .data, size 0x70 lma 0x8003310
      Start address 0x800015c, load size 13154
      Transfer rate: 7 KB/sec, 3288 bytes/write.

3. If you want to set a breakpoint at *main*, then you can type ``b main`` to set a breakpoint.

   .. code-block::

      (gdb) b main
      Breakpoint 1 at 0x8001b04: file hello_world.c, line 85.

4. If you want to set more breakpoints, you can do as you like.

5. Then you can type ``c``, then the program will stop at **main**

   .. code-block::

      (gdb) c
      Continuing.
      Note: automatically using hardware breakpoints for read-only addresses.

      Breakpoint 1, main () at hello_world.c:85
      85          srand(__get_rv_cycle()  | __get_rv_instret() | __RV_CSR_READ(CSR_MCYCLE));


6. Then you can step it using ``n`` (short of next) or ``s`` (short of step)

   .. code-block::

      (gdb) n
      86          uint32_t rval = rand();
      (gdb) n
      87          rv_csr_t misa = __RV_CSR_READ(CSR_MISA);
      (gdb) s
      89          printf("MISA: 0x%lx\r\n", misa);
      (gdb) n
      90          print_misa();
      (gdb) n
      92          printf("Hello World!\r\n");
      (gdb) n
      93          printf("Hello World!\r\n");

7. If you want to quit debugging, then you can press ``CTRL - c``, and type ``q`` to quit debugging.

   .. code-block::

      (gdb) Quit
      (gdb) q
      A debugging session is active.

              Inferior 1 [Remote target] will be detached.

      Quit anyway? (y or n) y
      Detaching from program: D:\workspace\Sourcecode\nuclei-sdk\application\baremetal\helloworld\hello_world.elf, Remote target
      Ending remote debugging.
      [Inferior 1 (Remote target) detached]

.. note::

   * More about how to debug using gdb, you can refer to the `GDB User Manual`_.
   * If you want to debug using Nuclei Studio, you can open Nuclei Studio, and create a debug configuration,
     and choose the application elf, and download and debug in IDE.


Create helloworld Application
-----------------------------

If you want to create your own ``helloworld`` application, it is also very easy.

There are several ways to achieve it, see as below:

* **Method 1:** You can find a most similar sample application folder and copy it, such as
  ``application/baremetal/helloworld``, you can copy and rename it as ``application/baremetal/hello``

  - Open the ``Makefile`` in ``application/baremetal/hello``

     1. Change ``TARGET = hello_world`` to ``TARGET = hello``

  - Open the ``hello_world.c`` in ``application/baremetal/hello``, and replace the content using code below:

     .. code-block:: c
        :linenos:

        // See LICENSE for license details.
        #include <stdio.h>
        #include <time.h>
        #include <stdlib.h>
        #include "nuclei_sdk_soc.h"

        int main(void)
        {
            printf("Hello World from Nuclei RISC-V Processor!\r\n");
            return 0;
        }

  - Save all the changes, and then you can follow the steps described
    in :ref:`quickstart_buildrundebug_app` to run or debug this new application.

* **Method 2:** You can also do it from scratch, with just create simple ``Makefile`` and ``main.c``

  - Create new folder named ``hello`` in ``application/baremetal``
  - Create two files named ``Makefile`` and ``main.c``
  - Open ``Makefile`` and edit the content as below:

     .. code-block:: makefile
        :linenos:

        TARGET = hello

        NUCLEI_SDK_ROOT = ../../..

        SRCDIRS = .

        INCDIRS = .

        include $(NUCLEI_SDK_ROOT)/Build/Makefile.base

  - Open ``main.c`` and edit the content as below:

     .. code-block:: c
        :linenos:

        // See LICENSE for license details.
        #include <stdio.h>
        #include <time.h>
        #include <stdlib.h>
        #include "nuclei_sdk_soc.h"

        int main(void)
        {
            printf("Hello World from Nuclei RISC-V Processor!\r\n");
            return 0;
        }

  - Save all the changes, and then you can follow the steps described
    in :ref:`quickstart_buildrundebug_app` to run or debug this new application.

.. note::

    * If your are looking for how to run for other boards, please ref to :ref:`design_board`.
    * Please refer to :ref:`develop_appdev` and :ref:`develop_buildsystem` for more information.
    * If you want to access SoC related APIs, please use ``nuclei_sdk_soc.h`` header file.
    * If you want to access SoC and board related APIs, please use ``nuclei_sdk_hal.h`` header file.
    * For simplified application development, you can use ``nuclei_sdk_hal.h`` directly.

.. _quickstart_advanced_usage:

Advanced Usage
--------------

For more advanced usage, please follow the items as below:

* Click :ref:`design` to learn about Nuclei SDK Design and Architecture, Board and SoC support documentation.
* Click :ref:`develop` to learn about Nuclei SDK Build System and Application Development.
* Click :ref:`design_app` to learn about each application usage and expected output.

.. note::

    * If you met some issues in using this guide, please check :ref:`faq`,
      if still not solved, please :ref:`contribute_submit_issue`.

    * If you are trying to **develop Nuclei SDK application in IDE**, now you have three choices:

      1. **Recommended**: Since Nuclei Studio 2020.08, Nuclei SDK will be deeply integrated with
         Nuclei Studio IDE, you can easily create a Nuclei SDK Project in Nuclei Studio through IDE
         Project Wizard, and easily configure selected Nuclei SDK project using SDK Configuration Tool,
         for more details, please click `Nuclei Tools`_ to download Nuclei Studio IDE, and refer to
         the `Nuclei_Studio_User_Guide.pdf`_ for how to use it.

      2. Now **Terapines ZCC Lite** is deeply integrated in **Nuclei Studio >= 2024.06**, so you just need to
         follow :ref:`quickstart_get_set_nuclei_sdk` to setup PATH for Terapines ZCC, and in Nuclei SDK,
         you can just pass **TOOCHAIN=terapines** during make to take a try with Terapines ZCC, and in future
         version of Nuclei SDK, you will be able to create project in Nuclei SDK using Terapines ZCC.

      3. You can take a try using IAR workbench, we provided prebuilt projects directly in Nuclei SDK,
         just check the `ideprojects/iar/README.md`_ to learn about it.

      4. You can take a try using Segger embedded studio, we provided prebuilt projects using Nuclei
         SDK release version, click `Segger embedded studio projects for Nuclei SDK`_ to learn about it

      5. You can also take a try with the Cross-platform PlatformIO IDE, we provided our Nuclei platform
         and Nuclei SDK release version in PlatformIO, click `Platform Nuclei in PlatformIO`_ to learn
         more about it, or you can visit `Light on onboard LED of RVSTAR board using PlatformIO(Chinese)`_
         to play with PlatformIO for Nuclei.

      6. You can also use source code in Nuclei SDK as base, and easily integrate with other IDE tools,
         such as ZStudio IDE, Compiler-IDE and others.


.. _GDB User Manual: https://www.gnu.org/software/gdb/documentation/
.. _Nuclei Tools: https://nucleisys.com/download.php
.. _Segger embedded studio projects for Nuclei SDK: https://github.com/riscv-mcu/ses_nuclei_sdk_projects
.. _Platform Nuclei in PlatformIO: https://platformio.org/platforms/nuclei
.. _Nuclei_Studio_User_Guide.pdf: https://www.nucleisys.com/upload/files/doc/nucleistudio/Nuclei_Studio_User_Guide.pdf
.. _Light on onboard LED of RVSTAR board using PlatformIO(Chinese): https://www.rvmcu.com/community-topic-id-310.html
.. _Nuclei Toolchain 2023.10: https://github.com/riscv-mcu/riscv-gnu-toolchain/releases/tag/nuclei-2023.10
.. _ideprojects/iar/README.md: https://github.com/Nuclei-Software/nuclei-sdk/blob/master/ideprojects/iar/README.md
.. _Nuclei Studio 2024.06: https://github.com/Nuclei-Software/nuclei-studio/releases/tag/2024.06
