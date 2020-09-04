.. _design_overview:

Overview
========

Nuclei SDK is developed based on **NMSIS**, all the SoCs supported in it
are following the `NMSIS-Core Device Templates Guidance`_.

So this Nuclei SDK can be treated as a software guide for how to use NMSIS.

The build system we use in Nuclei SDK is ``Makefile``, it support both Windows
and Linux, and when we develop Nuclei SDK build system, we keep it simple, so
it make developer can easily port this Nuclei SDK software code to other IDEs.

Click :ref:`overview` to learn more about the Nuclei SDK project overview.

For example, we have ported Nuclei SDK to use Segger embedded Studio and PlatformIO.


Directory Structure
-------------------

To learn deeper about Nuclei SDK project, the directory structure is a good start point.

Below, we will describe our design about the Nuclei SDK directory structure:

Here is the directory structure for this Nuclei SDK.

.. code-block::

    $NUCLEI_SDK_ROOT
    ├── application
    │   ├── baremetal
    │   ├── freertos
    │   ├── ucosii
    │   └── rtthread
    ├── Build
    │   ├── gmsl
    │   ├── Makefile.base
    │   ├── Makefile.conf
    │   ├── Makefile.core
    │   ├── Makefile.files
    │   ├── Makefile.files.gd32vf103
    │   ├── Makefile.files.hbird
    │   ├── Makefile.global
    │   ├── Makefile.misc
    │   ├── Makefile.rtos
    │   ├── Makefile.rtos.FreeRTOS
    │   ├── Makefile.rtos.UCOSII
    │   ├── Makefile.rtos.RTThread
    │   ├── Makefile.rules
    │   ├── Makefile.soc
    │   ├── Makefile.soc.gd32vf103
    │   └── Makefile.soc.hbird
    ├── doc
    │   ├── build
    │   ├── source
    │   ├── Makefile
    │   └── requirements.txt
    ├── NMSIS
    │   ├── Core
    │   ├── DSP
    │   ├── NN
    │   └── Library
    ├── OS
    │   ├── FreeRTOS
    │   ├── UCOSII
    │   └── RTThread
    ├── SoC
    │   ├── gd32vf103
    │   └── hbird
    ├── test
    │   ├── core
    │   ├── ctest.h
    │   ├── LICENSE
    │   └── README.md
    ├── LICENSE
    ├── Makefile
    ├── NMSIS_VERSION
    ├── package.json
    ├── SConscript
    ├── README.md
    ├── setup.bat
    └── setup.sh


* **application**

  This directory contains all the application softwares for this Nuclei SDK.

  The application code can be divided into mainly 4 parts, which are:

  - **Baremetal** applications, which will provide baremetal applications without any OS usage, these applications will be placed in *application/baremetal/* folder.
  - **FreeRTOS** applications, which will provide FreeRTOS applications using FreeRTOS RTOS, placed in *application/freertos/* folder.
  - **UCOSII** applications, which will provide UCOSII applications using UCOSII RTOS, placed in *application/ucosii/* folder.
  - **RTThread** applications, which will provide RT-Thread applications using RT-Thread RTOS, placed in *application/rtthread/* folder.

* **SoC**

  This directory contains all the supported SoCs for this Nuclei SDK, the directory name for SoC and its boards should always in lower case.

  Here we mainly support Nuclei processor cores running in Hummingbird FPGA evaluation board, the support package placed in *SoC/hbird/*.

  In each SoC's include directory, *nuclei_sdk_soc.h* must be provided, and include the soc header file, for example, *SoC/hbird/Common/Include/nuclei_sdk_soc.h*.

  In each SoC Board's include directory, *nuclei_sdk_hal.h* must be provided, and include the board header file, for example, *SoC/hbird/Board/hbird_eval/Include/nuclei_sdk_hal.h*.

* **Build**

  This directory contains the key part of the build system based on Makefile for Nuclei SDK.

* **NMSIS**

  This directory contains the NMSIS header files, which is widely used in this Nuclei SDK,
  you can check the *NMSIS_VERSION* file to know the current *NMSIS* version used in **Nuclei-SDK**.

  We will also sync the changes in `NMSIS project`_ when it provided a new release.

* **OS**

  This directory provided three RTOS package we suppported which are **FreeRTOS**, **UCOSII** and **RT-Thread**.

* **LICENSE**

  Nuclei SDK license file.

* **NMSIS_VERSION**

  NMSIS Version file. It will show current NMSIS version used in Nuclei SDK.

* **package.json**

  PlatformIO package json file for Nuclei SDK, used in `Nuclei Plaform for PlatformIO`_.

* **SConscript**

  RT-Thread package scons build script, used in `RT-Thread package development`_.

* **Makefile**

  An external Makefile just for build, run, debug application without cd to any coresponding application directory, such as *application/baremetal/helloworld/*.

* **setup.sh**

  Nuclei SDK environment setup script for **Linux**. You need to create your own `setup_config.sh`.

  .. code-block:: shell

     NUCLEI_TOOL_ROOT=/path/to/your_tool_root

  In the **$NUCLEI_TOOL_ROOT** for **Linux**, you need to have Nuclei RISC-V GNU GCC toolchain and OpenOCD installed as below.

  .. code-block:: console

     $NUCLEI_TOOL_ROOT
     ├── gcc
     │   ├── bin
     │   ├── include
     │   ├── lib
     │   ├── libexec
     │   ├── riscv-nuclei-elf
     │   └── share
     └── openocd
         ├── bin
         ├── contrib
         ├── distro-info
         ├── OpenULINK
         ├── scripts
         └── share

* **setup.bat**

  Nuclei SDK environment setup bat script for **Windows**. You need to create your own `setup_config.bat`.

  .. code-block:: bat

     set NUCLEI_TOOL_ROOT=\path\to\your_tool_root

  In the **%NUCLEI_TOOL_ROOT%** for **Windows**, you need to have Nuclei RISC-V GNU GCC toolchain, necessary Windows build tools and OpenOCD installed as below.

  .. code-block:: console

     %NUCLEI_TOOL_ROOT%
     ├── build-tools
     │   ├── bin
     │   ├── gnu-mcu-eclipse
     │   └── licenses
     ├── gcc
     │   ├── bin
     │   ├── include
     │   ├── lib
     │   ├── libexec
     │   ├── riscv-nuclei-elf
     │   └── share
     └── openocd
         ├── bin
         ├── contrib
         ├── distro-info
         ├── OpenULINK
         ├── scripts
         └── share


Project Components
------------------

This Nuclei SDK project components is list as below:

* :ref:`design_nuclei`: How Nuclei Processor Core is used in Nuclei SDK
* :ref:`design_soc`: How Nuclei processor code based SoC device is supported in Nuclei SDK
* :ref:`design_board`: How Nuclei based SoC's Board is supported in Nuclei SDK
* :ref:`design_peripheral`: How to use the peripheral driver in Nuclei SDK
* :ref:`design_rtos`: What RTOSes are supported in Nuclei SDK
* :ref:`design_app`: How to use pre-built applications in Nuclei SDK


.. _NMSIS-Core Device Templates Guidance: https://doc.nucleisys.com/nmsis/core/core_templates.html
.. _Nuclei Plaform for PlatformIO: https://platformio.org/platforms/nuclei/
.. _NMSIS project: https://github.com/Nuclei-Software/NMSIS
.. _RT-Thread package development: https://www.rt-thread.org/document/site/development-guide/package/package/
