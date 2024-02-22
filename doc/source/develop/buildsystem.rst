.. _develop_buildsystem:

Build System based on Makefile
==============================

Nuclei SDK's build system is based on Makefile, user can build,
run ordebug application in Windows and Linux.

.. _develop_buildsystem_structure:

Makefile Structure
------------------

Nuclei SDK's Makefiles mainly placed in **<NUCLEI_SDK_ROOT>/Build** directory and
an extra *Makefile* located in **<NUCLEI_SDK_ROOT>/Makefile**.

This extra **<NUCLEI_SDK_ROOT>/Makefile** introduce a new Make variable called
**PROGRAM** to provide the ability to build or run application in **<NUCLEI_SDK_ROOT>**.

For example, if you want to *rebuild and upload* application **application/baremetal/timer_test**,
you can run ``make PROGRAM=application/baremetal/timer_test clean upload`` to achieve it.


The **<NUCLEI_SDK_ROOT>/Build** directory content list as below:

.. code-block:: text

    gmsl/
    toolchain/
    Makefile.base
    Makefile.conf
    Makefile.core
    Makefile.components
    Makefile.files
    Makefile.global  -> Created by user
    Makefile.misc
    Makefile.rtos
    Makefile.rules
    Makefile.soc

The file or directory is used explained as below:

.. _develop_buildsystem_makefile_base:

Makefile.base
~~~~~~~~~~~~~

This **Makefile.base** file is used as Nuclei SDK build system entry file,
application's Makefile need to include this file to use all the features of
Nuclei SDK build system.

It will expose Make variables or options such as **BOARD** or **SOC** passed
by ``make`` command, click :ref:`develop_buildsystem_exposed_make_vars`
to learn more.

This file will include optional :ref:`develop_buildsystem_makefile_global`
and :ref:`develop_buildsystem_makefile_local` which allow user to set custom
global Makefile configurations and local application Makefile configurations.

This file will include the following makefiles:

* :ref:`develop_buildsystem_gmsl`: additional library functions provided via gmsl
* :ref:`develop_buildsystem_toolchain`: additional library functions provided via gmsl
* :ref:`develop_buildsystem_makefile_misc`: misc functions and OS check helpers
* :ref:`develop_buildsystem_makefile_conf`: main Makefile configuration entry
* :ref:`develop_buildsystem_makefile_rules`: make rules of this build system

.. _develop_buildsystem_gmsl:

gmsl
~~~~

The **gmsl** directory consist of the `GNU Make Standard Library (GMSL)`_,
which is an a library of functions to be used with GNU Make's $(call) that
provides functionality not available in standard GNU Make.

We use this **gmsl** tool to make sure we help us achieve some linux command
which is only supported in Linux.

.. _develop_buildsystem_toolchain:

toolchain
~~~~~~~~~

The **toolchain** directory contains different toolchain support makefiles,
such as Nuclei GNU toolchain, Nuclei LLVM toolchain and Terapines toolchain,
if you want to add a different toolchain support, you also need to add a new
toolchain makefile in it, you can refer to existing ones.

Since different toolchain support is added, in application Makefile, if your
toolchain options are not compatiable with others, to provide a compatiable
application for different toolchain, we recommend you to add ``toolchain_$(TOOLCHAIN).mk``
file in your application folder, and in application Makefile include this file,
you can refer to ``application/baremetal/benchmark/coremark`` to see example usage.

.. _develop_buildsystem_makefile_misc:

Makefile.misc
~~~~~~~~~~~~~

This **Makefile.misc** file mainly provide these functions:

* Define **get_csrcs**, **get_asmsrcs**, **get_cxxsrcs** and **check_item_exist** make functions

  - **get_csrcs**: Function to get ``*.c or *.C`` source files from a list of directories, no ability to
    do recursive match. e.g. ``$(call get_csrcs, csrc csrc/abc)`` will return c source files in
    ``csrc`` and ``csrc/abc`` directories.
  - **get_asmsrcs**: Function to get ``*.s or *.S`` source files from a list of directories, no ability to
    do recursive match. e.g. ``$(call get_asmsrcs, asmsrc asmsrc/abc)`` will return asm source files in
    ``asmsrc`` and ``asmsrc/abc`` directories.
  - **get_cxxsrcs**: Function to get ``*.cpp or *.CPP`` source files from a list of directories, no ability
    to do recursive match. e.g. ``$(call get_cxxsrcs, cppsrc cppsrc/abc)`` will return cpp source files in
    ``cppsrc`` and ``cppsrc/abc`` directories.
  - **check_item_exist**: Function to check if item existed in a set of items. e.g.
    ``$(call check_item_exist, flash, flash ilm flashxip)`` will check ``flash`` whether existed in
    ``flash ilm flashxip``, if existed, return ``flash``, otherwise return empty.

* Check and define OS related functions, and also a set of trace print functions.

.. _develop_buildsystem_makefile_conf:

Makefile.conf
~~~~~~~~~~~~~

This **Makefile.conf** file will define the following items:

* Toolchain related variables used during compiling
* Debug related variables
* Include :ref:`develop_buildsystem_makefile_files` and :ref:`develop_buildsystem_makefile_rtos`
* Collect all the C/C++/ASM compiling and link options

.. _develop_buildsystem_makefile_rules:

Makefile.rules
~~~~~~~~~~~~~~

This **Makefile.rules** file will do the following things:

* Collect all the sources during compiling
* Define all the rules used for building, uploading and debugging
* Print help message for build system


.. _develop_buildsystem_makefile_files:

Makefile.files
~~~~~~~~~~~~~~

This **Makefile.files** file will do the following things:

* Define common C/C++/ASM source and include directories
* Define common C/C++/ASM macros

.. _develop_buildsystem_makefile_soc:

Makefile.soc
~~~~~~~~~~~~

This **Makefile.soc** will include valid makefiles located in
**<NUCLEI_SDK_ROOT>/SoC/<SOC>/build.mk** according to
the :ref:`develop_buildsystem_var_soc` makefile variable setting.

It will define the following items:

* **DOWNLOAD** and **CORE** variables

  - For :ref:`design_soc_evalsoc`, we can support all the modes defined in
    :ref:`develop_buildsystem_var_download`, and **CORE** list defined in
    :ref:`develop_buildsystem_makefile_core`
  - For :ref:`design_soc_gd32vf103`, The **CORE** is fixed to N205, since
    it is a real SoC chip, and only **FlashXIP** download mode is supported

* Linker script used according to the **DOWNLOAD** mode settings
* OpenOCD debug configuration file used for the SoC and Board
* Some extra compiling or debugging options

A valid SoC should be organized like this, take ``evalsoc`` as example:

.. code-block::

    SoC/evalsoc
    ├── Board
    │   └── nuclei_fpga_eval
    │       ├── Include
    │       │   ├── board_nuclei_fpga_eval.h
    │       │   └── nuclei_sdk_hal.h
    │       ├── Source
    │       │   ├── IAR
    │       │   └── GCC
    │       └── openocd_evalsoc.cfg
    ├── build.mk
    └── Common
        ├── Include
        │   ├── evalsoc.h
        │   ├── ... ...
        │   ├── evalsoc_uart.h
        │   ├── nuclei_sdk_soc.h
        │   └── system_evalsoc.h
        └── Source
            ├── Drivers
            │   ├── ... ...
            │   └── evalsoc_uart.c
            ├── GCC
            │   ├── intexc_evalsoc.S
            │   ├── intexc_evalsoc_s.S
            │   └── startup_evalsoc.S
            ├── IAR
            │   ├── intexc_evalsoc.S
            │   ├── intexc_evalsoc_s.S
            │   └── startup_evalsoc.c
            ├── Stubs
            │   ├── newlib
            │   ├── libncrt
            │   └── iardlib
            ├── evalsoc_common.c
            └── system_evalsoc.c


.. _develop_buildsystem_makefile_rtos:

Makefile.rtos
~~~~~~~~~~~~~

This **Makefile.rtos** will include **<NUCLEI_SDK_ROOT>/OS/<RTOS>/build.mk**
according to our :ref:`develop_buildsystem_var_rtos` variable.

A valid rtos should be organized like this, take ``UCOSII`` as example:

.. code-block::

    OS/UCOSII/
    ├── arch
    ├── build.mk
    ├── license.txt
    ├── readme.md
    └── source


If no :ref:`develop_buildsystem_var_rtos` is chosen, then RTOS
code will not be included during compiling, user will develop
baremetal application.

If **FreeRTOS**, **UCOSII** or **RTThread** RTOS is chosen, then FreeRTOS
UCOSII, or RTThread source code will be included during compiling, and extra
compiler option ``-DRTOS_$(RTOS_UPPER)`` will be passed, then user can develop RTOS application.

For example, if ``FreeRTOS`` is selected, then ``-DRTOS_FREERTOS`` compiler option
will be passed.

.. _develop_buildsystem_makefile_components:

Makefile.components
~~~~~~~~~~~~~~~~~~~

This **Makefile.components** will include ``build.mk`` Makefiles of selected components defined
via makefile variable :ref:`develop_buildsystem_var_middleware`, the Makefiles are placed in
the sub-folders of **<NUCLEI_SDK_ROOT>/Components/**.

A valid middleware component should be organized like this, take ``fatfs`` as example :

.. code-block::

    Components/fatfs/
    ├── build.mk
    ├── documents
    ├── LICENSE.txt
    └── source


For example, if there are two valid middleware components in **<NUCLEI_SDK_ROOT>/Components/**, called
``fatfs`` and ``tjpgd``, and you want to use them in your application, then you can set ``MIDDLEWARE``
like this ``MIDDLEWARE := fatfs tjpgd``, then the application will include these two middlewares into
build process.

.. _develop_buildsystem_makefile_core:

Makefile.core
~~~~~~~~~~~~~

This **Makefile.core** is used to define the RISC-V ARCH and ABI used during
compiling of the CORE list supported.

If you want to add a new **CORE**, you need to add a new line before **SUPPORTED_CORES**,
and append the new **CORE** to **SUPPORTED_CORES**.

For example, if you want to add a new **CORE** called **n308**, and the **n308**'s
**ARCH** and **ABI** are ``rv32imafdc`` and ``ilp32d``, then you can add a new line
like this ``N308_CORE_ARCH_ABI = rv32imafdc ilp32d``, and append **n308** to **SUPPORTED_CORES**
like this ``SUPPORTED_CORES = n201 n201e n203 n203e n205 n205e n305 n307 n307fd n308 nx600``

.. note::

   * The appended new **CORE** need to lower-case, e.g. *n308*
   * The new defined variable **N308_CORE_ARCH_ABI** need to be all upper-case.


.. _develop_buildsystem_makefile_global:

Makefile.global
~~~~~~~~~~~~~~~

This **Makefile.global** file is an optional file, and will not be tracked by git,
user can create own **Makefile.global** in **<NUCLEI_SDK_ROOT>/Build** directory.

In this file, user can define custom **SOC**, **BOARD**, **DOWNLOAD** options to
overwrite the default configuration.

For example, if you will use only the :ref:`design_board_gd32vf103v_rvstar`, you can
create the **<NUCLEI_SDK_ROOT>/Build/Makefile.global** as below:

.. code-block:: Makefile

    SOC ?= gd32vf103
    BOARD ?= gd32vf103v_rvstar
    DOWNLOAD ?= flashxip

.. note::

    * If you add above file, then you can build, run, debug application without passing
      **SOC**, **BOARD** and **DOWNLOAD** variables using make command for
      :ref:`design_board_gd32vf103v_rvstar` board, e.g.

      - Build and run application for :ref:`design_board_gd32vf103v_rvstar`: ``make run``
      - Debug application for :ref:`design_board_gd32vf103v_rvstar`: ``make debug``

    * The :ref:`design_board_gd32vf103v_rvstar` only support ``FlashXIP`` download mode.
    * If you create the **Makefile.global** like above sample code, you will also be able
      to use Nuclei SDK build system as usually, it will only change the default **SOC**,
      **BOARD** and **DOWNLOAD**, but you can still override the default variable using
      make command, such as ``make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm``

.. _develop_buildsystem_makefile_local:

Makefile.local
~~~~~~~~~~~~~~

As the :ref:`develop_buildsystem_makefile_global` is used to override the default Makefile
configurations, and the **Makefile.local** is used to override application level Makefile
configurations, and also this file will not be tracked by git.

User can create ``Makefile.local`` file in any of the application folder, placed together with
the application Makefile, for example, you can create ``Makefile.local`` in ``application/baremetal/helloworld``
to override default make configuration for this **helloworld** application.

If you want to change the default board for **helloworld** to use :ref:`design_board_gd32vf103v_rvstar`,
you can create ``application/baremetal/helloworld/Makefile.local`` as below:

.. code-block:: Makefile

    SOC ?= gd32vf103
    BOARD ?= gd32vf103v_rvstar
    DOWNLOAD ?= flashxip

.. note::

    * This local make configuration will override global and default make configuration.
    * If you just want to change only some applications' makefile configuration, you can
      add and update ``Makefile.local`` for those applications.


.. _develop_buildsystem_make_targets:

Makefile targets of make command
--------------------------------

Here is a list of the :ref:`table_dev_buildsystem_4`.

.. _table_dev_buildsystem_4:

.. list-table:: Make targets supported by Nuclei SDK Build System
   :widths: 20 80
   :header-rows: 1
   :align: center

   * - target
     - description
   * - help
     - display help message of Nuclei SDK build system
   * - info
     - display selected configuration information
   * - showflags
     - display asm/c/cxx/ld flags and other info
   * - showtoolver
     - display toolchain/qemu/openocd version
   * - all
     - build application with selected configuration
   * - clean
     - clean application with selected configuration
   * - dasm
     - build and dissemble application with selected configuration
   * - bin
     - build and generate application binary with selected configuration
   * - upload
     - build and upload application with selected configuration
   * - run_openocd
     - run openocd server with selected configuration, and wait for gdb at port specified by $(GDB_PORT)
   * - run_gdb
     - build and start gdb process with selected configuration, and connect to localhost:$(GDB_PORT)
   * - debug
     - build and debug application with selected configuration
   * - run_qemu
     - run application on qemu machine with selected configuration
   * - run_xlspike
     - run application on xlspike with selected configuration
   * - size
     - show program size

.. note::

   * The selected configuration is controlled by
     :ref:`develop_buildsystem_exposed_make_vars`
   * For ``run_openocd`` and ``run_gdb`` target, if you want to
     change a new gdb port, you can pass the variable
     :ref:`develop_buildsystem_var_gdb_port`
   * For ``run_qemu``, only ``SOC=evalsoc`` supported,
     when do this target, you can pass ``SIMU=qemu`` to support auto-exit,
     project recompiling is required.
   * For ``run_xlspike``, only ``SOC=evalsoc`` supported,
     when do this target, you can pass ``SIMU=xlspike`` to support auto-exit,
     project recompiling is required.

.. _develop_buildsystem_exposed_make_vars:

Makefile variables passed by make command
-----------------------------------------

In Nuclei SDK build system, we exposed the following Makefile variables
which can be passed via make command.

* :ref:`develop_buildsystem_var_soc`
* :ref:`develop_buildsystem_var_board`
* :ref:`develop_buildsystem_var_variant`
* :ref:`develop_buildsystem_var_toolchain`
* :ref:`develop_buildsystem_var_download`
* :ref:`develop_buildsystem_var_core`
* :ref:`develop_buildsystem_var_archext`
* :ref:`develop_buildsystem_var_cpu_series`
* :ref:`develop_buildsystem_var_simulation`
* :ref:`develop_buildsystem_var_semihost`
* :ref:`develop_buildsystem_var_gdb_port`
* :ref:`develop_buildsystem_var_v`
* :ref:`develop_buildsystem_var_silent`

.. note::

   * These variables can also be used and defined in application Makefile
   * If you just want to fix your running board of your application, you can
     just define these variables in application Makefile, if defined, then
     you can simply use ``make clean``, ``make upload`` or ``make debug``, etc.


.. _develop_buildsystem_var_soc:

SOC
~~~

**SOC** variable is used to declare which SoC is used in application during compiling.

**evalsoc** is the default SoC, if no **SOC** passed or environment variable set, you can check
default settings by run ``make info``, it will will show default settings without any overriding
make variable.

You can easily find the supported SoCs in the **<NUCLEI_SDK_ROOT>/SoC** directory.

Currently we support the following SoCs, see :ref:`table_dev_buildsystem_1`.

.. _table_dev_buildsystem_1:

.. list-table:: Supported SoCs
   :widths: 10, 60
   :header-rows: 1
   :align: center

   * - **SOC**
     - Reference
   * - gd32vf103
     - :ref:`design_soc_gd32vf103`
   * - evalsoc
     - :ref:`design_soc_evalsoc`

.. note::

   If you are our SoC subsystem customer, in the SDK delivered to you, you can find your soc name
   in this **<NUCLEI_SDK_ROOT>/SoC** directory, take ``gd32vf103`` SoC as example, when ``SOC=gd32vf103``,
   the SoC source code in **<NUCLEI_SDK_ROOT>/SoC/gd32vf103/Common** will be used.

   This documentation just document the open source version of Nuclei SDK's supported SOC and Board.

.. _develop_buildsystem_var_board:

BOARD
~~~~~

**BOARD** variable is used to declare which Board is used in application during compiling.

The **BOARD** variable should match the supported boards of chosen **SOC**.
You can easily find the supported Boards in the **<NUCLEI_SDK_ROOT>/<SOC>/Board/** directory.

* :ref:`table_dev_buildsystem_2`
* :ref:`table_dev_buildsystem_3`


Currently we support the following SoCs.

.. _table_dev_buildsystem_2:

.. list-table:: Supported Boards when SOC=gd32vf103
   :widths: 20, 60
   :header-rows: 1
   :align: center

   * - **BOARD**
     - Reference
   * - gd32vf103v_rvstar
     - :ref:`design_board_gd32vf103v_rvstar`
   * - gd32vf103c_dlink
     - :ref:`design_board_gd32vf103c_dlink`
   * - gd32vf103v_eval
     - :ref:`design_board_gd32vf103v_eval`
   * - gd32vf103c_longan_nano
     - :ref:`design_board_sipeed_longan_nano`
   * - gd32vf103c_t_display
     - :ref:`design_board_sipeed_longan_nano`

.. _table_dev_buildsystem_3:

.. list-table:: Supported Boards when SOC=evalsoc
   :widths: 10 60
   :header-rows: 1
   :align: center

   * - **BOARD**
     - Reference
   * - nuclei_fpga_eval
     - :ref:`design_board_nuclei_fpga_eval`

.. note::

    * If you only specify **SOC** variable in make command, it will use default **BOARD**
      and **CORE** option defined in **<NUCLEI_SDK_ROOT>/SoC/<SOC>/build.mk**
    * If you are our SoC subsystem customer, in the SDK delivered to you, you can check
      the board supported list in **<NUCLEI_SDK_ROOT>/<SOC>/Board/**, take ``SOC=gd32vf103 BOARD=gd32vf103v_rvstar``
      as example, the board source code located **<NUCLEI_SDK_ROOT>/gd32vf103/Board/gd32vf103v_rvstar** will be used.

.. _develop_buildsystem_var_variant:

VARIANT
~~~~~~~

**VARIANT** variable is used to declare which variant of board is used in application during compiling.

It might only affect on only small piece of board, and this is SoC and Board dependent.

This variable only affect the selected board or soc, and it is target dependent.


.. _develop_buildsystem_var_toolchain:

TOOLCHAIN
~~~~~~~~~

.. note::

    This variable is added in 0.5.0 release.

This variable is used to select different toolchain to compile application.
Currently we support 3 toolchain in Nuclei SDK.

* **nuclei_gnu**: default, it will choose nuclei gnu toolchain, distributed with Nuclei Toolchain.
* **nuclei_llvm**: still in experiment, nuclei customized extensions not yet supported, distributed with Nuclei Toolchain.
* **terapines**: still in experiment, it depends on the toolchain vendor about the supported extensions, if you want to take a try with it, just visit https://www.terapines.com/ and request an terapines toolchain evaluation.

For **nuclei_gnu/nuclei_llvm** toolchain both newlib and libncrt library are supported,
but nuclei_llvm toolchain multilib selection mechanism is not as good as gnu toolchain,
you need to take care of the arch isa string order, please see ``riscv64-unknown-unknown-elf-clang -v`` output for supported multilib and its isa string order.

And IAR compiler support is also done in Nuclei SDK, you can take a try with it
via ``ideprojects/iar`` folder provided prebuilt ide projects.

If you want to use old Nuclei GNU Toolchain <= 2022.12 in Nuclei SDK 0.5.0, you need to pass extra ``COMPILE_PREFIX=riscv-nuclei-elf-`` when build any application, such as ``make CORE=n307fd COMPILE_PREFIX=riscv-nuclei-elf-  STDCLIB=libncrt_small clean all``, but this is not recommended, and will be deprecated in future any time.

.. _develop_buildsystem_var_download:

DOWNLOAD
~~~~~~~~

**DOWNLOAD** variable is used to declare the download mode of the application,
currently it has these modes supported as described in table
:ref:`table_dev_buildsystem_5`

.. _table_dev_buildsystem_5:

.. list-table:: Supported download modes
   :widths: 10 70
   :header-rows: 1
   :align: center

   * - **DOWNLOAD**
     - Description
   * - ilm
     - | Program will be download into ilm/ram and
       | run directly in ilm/ram, program will lost when poweroff
   * - flash
     - | Program will be download into flash, when running,
       | program will be copied to ilm/ram and run in ilm/ram
   * - flashxip
     - Program will to be download into flash and run directly in flash
   * - ddr
     - | Program will to be download into ddr and
       | run directly in ddr, program will lost when poweroff
   * - sram
     - | Program will to be download into sram and
       | run directly in sram, program will lost when poweroff

.. note::

    * This variable now target dependent, and its meaning depending on how this
      variable is implemented in SoC's build.mk
    * :ref:`design_soc_gd32vf103` only support **DOWNLOAD=flashxip**
    * **flashxip** mode in :ref:`design_soc_evalsoc` is very slow due to
      the CORE frequency is very slow, and flash execution speed is slow
    * **ddr** mode is introduced in release ``0.2.5`` of Nuclei SDK
    * macro ``DOWNLOAD_MODE`` and ``DOWNLOAD_MODE_STRING`` will be defined in Makefile,
      eg. when ``DOWNLOAD=flash``, macro will be defined as ``-DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASH``,
      and ``-DDOWNLOAD_MODE_STRING=\"flash\"``, the ``flash`` will be in upper case,
      currently ``DOWNLOAD_MODE_STRING`` macro is used in ``system_<Device>.c`` when
      banner is print.
    * This download mode is also used to clarify whether in the link script,
      your eclic vector table is placed in ``.vtable_ilm`` or ``.vtable`` section, eg.
      for evalsoc, when ``DOWNLOAD=flash``, vector table is placed in ``.vtable_ilm`` section,
      and an extra macro called ``VECTOR_TABLE_REMAPPED`` will be passed in Makefile.
      When ``VECTOR_TABLE_REMAPPED`` is defined, it means vector table's LMA and VMA are
      different, it is remapped.
    * From release ``0.3.2``, this ``DOWNLOAD_MODE`` should not be used, and macros
      ``DOWNLOAD_MODE_ILM``, ``DOWNLOAD_MODE_FLASH``, ``DOWNLOAD_MODE_FLASHXIP`` and
      ``DOWNLOAD_MODE_DDR`` previously defined in ``riscv_encoding.h`` now are moved to
      ``<Device.h>`` such as ``evalsoc.h``, and should be deprecated in future.
      Now we are directly using ``DOWNLOAD_MODE_STRING`` to pass the download mode string,
      no longer need to define it in source code as before.
    * From release ``0.3.2``, you can define **DOWNLOAD** not just the download mode list above,
      you can use other download mode names specified by your customized SoC.
    * For SRAM download mode, for 200/300, it don't has DDR, so sram is a external ram outside of cpu,
      for 600/900, it has DDR, so sram is the ddr ram

.. _develop_buildsystem_var_core:

CORE
~~~~

**CORE** variable is used to declare the Nuclei processor core
of the application.

Currently it has these cores supported as described in table
:ref:`table_dev_buildsystem_6`.

.. _table_dev_buildsystem_6:

.. table:: Supported Nuclei Processor cores
   :widths: 20 20 20
   :align: center

   ========  ========== =======  =================
   **CORE**  **ARCH**   **ABI**       **TUNE**
   n201      rv32iac    ilp32    nuclei-200-series
   n201e     rv32eac    ilp32e   nuclei-200-series
   n203      rv32imac   ilp32    nuclei-200-series
   n203e     rv32emac   ilp32e   nuclei-200-series
   n205      rv32imac   ilp32    nuclei-200-series
   n205e     rv32emac   ilp32e   nuclei-200-series
   n300      rv32imac   ilp32    nuclei-300-series
   n300f     rv32imafc  ilp32f   nuclei-300-series
   n300fd    rv32imafdc ilp32d   nuclei-300-series
   n305      rv32imac   ilp32    nuclei-300-series
   n307      rv32imafc  ilp32f   nuclei-300-series
   n307fd    rv32imafdc ilp32d   nuclei-300-series
   n600      rv32imac   ilp32    nuclei-600-series
   n600f     rv32imafc  ilp32f   nuclei-600-series
   n600fd    rv32imafdc ilp32d   nuclei-600-series
   nx600     rv64imac   lp64     nuclei-600-series
   nx600f    rv64imafc  lp64f    nuclei-600-series
   nx600fd   rv64imafdc lp64d    nuclei-600-series
   ux600     rv64imac   lp64     nuclei-600-series
   ux600f    rv64imafc  lp64f    nuclei-600-series
   ux600fd   rv64imafdc lp64d    nuclei-600-series
   n900      rv32imac   ilp32    nuclei-900-series
   n900f     rv32imafc  ilp32f   nuclei-900-series
   n900fd    rv32imafdc ilp32d   nuclei-900-series
   u900      rv32imac   ilp32    nuclei-900-series
   u900f     rv32imafc  ilp32f   nuclei-900-series
   u900fd    rv32imafdc ilp32d   nuclei-900-series
   nx900     rv64imac   lp64     nuclei-900-series
   nx900f    rv64imafc  lp64f    nuclei-900-series
   nx900fd   rv64imafdc lp64d    nuclei-900-series
   ux900     rv64imac   lp64     nuclei-900-series
   ux900f    rv64imafc  lp64f    nuclei-900-series
   ux900fd   rv64imafdc lp64d    nuclei-900-series
   ========  ========== =======  =================

When **CORE** is selected, the **ARCH**, **ABI** and **TUNE** (optional) are set,
and it might affect the compiler options in combination with :ref:`develop_buildsystem_var_archext`
depended on the implementation of SoC build.mk.

Take ``SOC=evalsoc`` as example.

- If **CORE=n205 ARCH_EXT=**, then ``ARCH=rv32imac, ABI=ilp32 TUNE=nuclei-200-series``. 
  riscv arch related compile and link options will be passed, for this case, it will be
  ``-march=rv32imac -mabi=ilp32 -mtune=nuclei-200-series``.

- If **CORE=n205 ARCH_EXT=_zba_zbb_zbc_zbs**, it will be ``-march=rv32imac_zba_zbb_zbc_zbs -mabi=ilp32 -mtune=nuclei-200-series``.

For riscv code model settings, the ``RISCV_CMODEL`` variable will be set to medlow
for RV32 targets, otherwise it will be medany.

The some SoCs, the CORE is fixed, so the ARCH and ABI will be fixed, such as
``gd32vf103`` SoC, in build system, the CORE is fixed to n205, and ARCH=rv32imac, ABI=ilp32.

.. _develop_buildsystem_var_archext:

ARCH_EXT
~~~~~~~~

**ARCH_EXT** variable is used to select extra RISC-V arch extensions supported by Nuclei
RISC-V Processor, except the ``iemafdc``.

.. note::

   `Nuclei Toolchain 2023.10`_ now bump gcc version from gcc 10 to gcc 13, which introduced
   incompatiable ``-march`` option, so ``ARCH_EXT`` usage is also incompatiable now.

   About the incompatiable march option change, please see https://github.com/riscv-non-isa/riscv-toolchain-conventions/pull/26, which is already present in latest gcc and clang release.

When using gcc 13 or clang 17 toolchain in 2023.10 toolchain release, you need to use it like this in 0.5.0 sdk release or later version.

Here are several examples when using ARCH_EXT for Nuclei RISC-V Processors:

* If you want to use just `B 1.0 extension`_, you can pass **ARCH_EXT=_zba_zbb_zbc_zbs**
* If you want to use just Nuclei implemented `P 0.5.4 extension`_ and N1/N2/N3 customized extension

  - Xxldsp: means P 0.5.4 + Nuclei default enabled additional 8 expd instructions for both RV32 and RV64, you can pass **ARCH_EXT=_xxldsp**
  - Xxldspn1x: means Xxldsp + Nuclei N1 additional instructions for RV32 only, you can pass **ARCH_EXT=_xxldspn1x**
  - Xxldspn2x: means Xxldspn1x + Nuclei N2 additional instructions for RV32 only, you can pass **ARCH_EXT=_xxldspn2x**
  - Xxldspn3x: means Xxldspn1x + Nuclei N3 additional instructions for RV32 only, you can pass **ARCH_EXT=_xxldspn3x**
* If you want to use `K 1.0 extension`_, you can pass **ARCH_EXT=_zk_zks**
* If you want to use `V 1.0 extension`_

  - For rv32 without f/d extension, you can pass **ARCH_EXT=_zve32x**
  - For rv32 with f/d extension, you can pass **ARCH_EXT=_zve32f**
  - For rv64 without f/d extension, you can pass **ARCH_EXT=_zve64x**
  - For rv64 with f extension, you can pass **ARCH_EXT=_zve64f**
  - For rv64 with fd extension, you can pass **ARCH_EXT=v**

* If you want to use F16(zfh/zvfh) extension, you can follow below steps

  - For case without vector extension, you can add extra ``_zfh`` to **ARCH_EXT**, eg, **ARCH_EXT=_zfh**
  - For case with vector extension, you can add extra ``_zfh_zvfh`` to **ARCH_EXT**, eg, **ARCH_EXT=_zfh_zvfh**
  - And the prebuilt NMSIS DSP library also provide F16 support with prebuilt F16 library, you can check library name with ``zfh``, such as ``NMSIS/Library/DSP/GCC/libnmsis_dsp_rv32imafc_zfh_zvfh_zve32f.a``
  - Spec about `zfh extension`_ and `zvfh extension`_

* If you want to use `Zc 1.0 extension`_

  - You can use it together with C extension, which means it should be concat with isa string like ``rv32imafd_zca_zcb_zcf_zcmp_zcmt``
  - In Nuclei SDK, the isa string processing is done in build system
  - If you want to use with n300/n900, you can pass **ARCH_EXT=_zca_zcb_zcmp_zcmt**
  - If you want to use with n300f/n900f, you can pass **ARCH_EXT=_zca_zcb_zcf_zcmp_zcmt**
  - If you want to use with n300fd/n900fd, you can pass **ARCH_EXT=_zca_zcb_zcf_zcmp_zcmt**
  - If you want to use with n300fd/n900fd without zcmp/zcmt, you can pass **ARCH_EXT=_zca_zcb_zcf_zcd**
  - If you want to use with extra Nuclei Code Size Reduction extension called Xxlcz, you can add extra ``_xxlcz`` in **ARCH_EXT**, eg. for n300, you can pass **ARCH_EXT=_zca_zcb_zcmp_zcmt_xxlcz**

* When using customized extensions such as Xxldsp/Xxldspn1x/Xxldspn2x/Xxldspn3x/Xxlcz, the isa string must be placed after all ``_z`` started isa strings, here is an legal string such as ``rv32imafd_zca_zcb_zcf_zcmp_zcmt_zba_zbb_zbc_zbs_zk_zks_xxlcz_xxldspn3x`` for rv32 with imafd + Zc + B + K + Xxldspn3x + Xxlcz

* You need to handle this **ARCH_EXT** carefully, expecially using with demo_dsp demo since it will default search library match the whole arch name but you can pass :ref:`develop_buildsystem_var_nmsis_lib_arch` variable in Makefile to choose your desired library arch.

* LLVM Clang in Nuclei RISC-V Toolchain 2023.10 don't support ``Xxldsp`` and ``Xxlcz`` extension now, please take care.

* When using llvm clang compiler, the isa string order must be treat carefully, it is not handled very good when searching different multilib.

* You can check prebuilt multilib for gcc and clang using ``riscv64-unknown-elf-gcc --print-multi-lib`` and ``riscv64-unknown-elf-clang --print-multi-lib``

Here below are for using gcc 10 toolchain, you can use it like this below in old nuclei sdk release before 0.5.0.

Currently, valid arch extension combination should match the order of ``bpv``.

Here is a list of valid arch extensions:

* **ARCH_EXT=b**: RISC-V bitmanipulation extension.
* **ARCH_EXT=p**: RISC-V packed simd extension.
* **ARCH_EXT=v**: RISC-V vector extension.
* **ARCH_EXT=bp**: RISC-V bitmanipulation and packed simd extension.
* **ARCH_EXT=pv**: RISC-V packed simd and vector extension.
* **ARCH_EXT=bpv**: RISC-V bitmanipulation, packed simd and vector extension.

It is suggested to use this ARCH_EXT with other arch options like this, can be found in
``SoC/evalsoc/build.mk``:

.. code-block:: makefile

    # Set RISCV_ARCH and RISCV_ABI
    CORE_UPPER := $(call uc, $(CORE))
    CORE_ARCH_ABI := $($(CORE_UPPER)_CORE_ARCH_ABI)
    RISCV_ARCH ?= $(word 1, $(CORE_ARCH_ABI))$(ARCH_EXT)
    RISCV_ABI ?= $(word 2, $(CORE_ARCH_ABI))


.. _develop_buildsystem_var_cpu_series:

CPU_SERIES
~~~~~~~~~~

.. note::

    * This variable is used to control different compiler options for different Nuclei CPU series such
      as 200/300/600/900.

This variable will be auto set if your CORE variable match the following rules:

* **200**: CORE start with *20*, the CPU_SERIES will be 200.
* **300**: CORE start with *30*, the CPU_SERIES will be 300.
* **600**: CORE start with *60*, the CPU_SERIES will be 600.
* **900**: CORE start with *90*, the CPU_SERIES will be 900.
* **0**: CORE start with others, the CPU_SERIES will be 0.

It can also be defined in Makefile itself directly or passed via make command.

It will also define an macro called **CPU_SERIES**, eg. for CPU_SERIES=200, it will define macro CPU_SERIES=200.

This variable is currently used in benchmark cases, and require application Makefile changes.

.. _develop_buildsystem_var_semihost:

SEMIHOST
~~~~~~~~

If **SEMIHOST=1**, it means it will enable semihost support using openocd.

From 0.5.0, both newlib and libncrt support semihosting feature, and when using semihost,
no need to implement the clib stub functions, which is done by newlib or libncrt semihosting
library.

And for qemu 2023.10 verison, you can also use semihosting feature, simple usage is like below for qemu:

.. code-block:: shell

    cd application/baremetal/helloworld
    # clean project first
    make SOC=evalsoc SEMIHOST=1 clean
    make SOC=evalsoc SEMIHOST=1 all
    # run on qemu, SEMIHOST=1 is required to pass when run qemu
    make SOC=evalsoc SEMIHOST=1 run_qemu

When using semihosting feature with openocd, debug message will print via openocd console.

You need to use it like this(assume you are run on evalsoc, CORE=n300):

In terminal 1, open openocd and monitor the output:

.. code-block:: shell

    cd application/baremetal/helloworld
    make SOC=evalsoc CORE=n300 run_openocd
    # when terminal 2 has download program and start to run, you will be able to see output here

In terminal 2, gdb connect to the openocd exposed gdb port and load program, and run

.. code-block:: shell

    # in normal shell terminal
    cd application/baremetal/helloworld
    make SOC=evalsoc CORE=n300 SEMIHOST=1 clean
    make SOC=evalsoc CORE=n300 SEMIHOST=1 run_gdb

    # now in gdb command terminal, run the following command
    monitor reset halt
    load
    ## when run continue, you will be able to see output in previous terminal 1 running openocd
    continue

.. _develop_buildsystem_var_simulation:

SIMULATION
~~~~~~~~~~

If **SIMULATION=1**, it means the program is optimized for hardware simulation environment.

Currently if **SIMULATION=1**, it will pass compile option **-DCFG_SIMULATION**,
application can use this **CFG_SIMULATION** to optimize program for hardware
simulation environment.

.. note::

   * Currently the benchmark applications in **application/baremetal/benchmark** used this optimization

.. _develop_buildsystem_var_gdb_port:

GDB_PORT
~~~~~~~~

.. note::

    * This new variable **GDB_PORT** is added in Nuclei SDK since version ``0.2.4``

This variable is not used usually, by default the **GDB_PORT** variable is ``3333``.

If you want to change a debug gdb port for openocd and gdb when run ``run_openocd`` and
``run_gdb`` target, you can pass a new port such as ``3344`` to this variable.

For example, if you want to debug application using run_openocd and
run_gdb and specify a different port other than ``3333``.

You can do it like this, take ``nuclei_fpga_eval`` board for example, such as port ``3344``:

* Open openocd server: ``make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=n307 GDB_PORT=3344 run_openocd``

* connect gdb with openocd server: ``make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=n307 GDB_PORT=3344 run_gdb``

.. _develop_buildsystem_var_jtagsn:

JTAGSN
~~~~~~

.. note::

   * This new variable **JTAGSN** is added in ``0.4.0`` release

This variable is used specify jtag adapter serial number in openocd configuration, need to be supported in
openocd configuration file and makefile, currently **evalsoc** is supported.
It is used by openocd ``adapter serial``.

Assume you have a jtag adapter, serial number is ``FT6S9RD6``, and you want to download program through
this jtag to a fpga with ux900 bitstream on it, you can do it like this.

For windows, you need to pass extra ``A``, eg. ``JTAGSN=FT6S9RD6A``

.. code-block:: shell

    # cd to helloworld
    cd application/baremetal/helloworld
    # clean program
    make SOC=evalsoc CORE=ux900 JTAGSN=FT6S9RD6 clean
    # upload program
    make SOC=evalsoc CORE=ux900 JTAGSN=FT6S9RD6 upload

.. _develop_buildsystem_var_banner:

BANNER
~~~~~~

If **BANNER=0**, when program is rebuilt, then the banner message print in console will not be print,
banner print is default enabled via ``NUCLEI_BANNER=1`` in ``nuclei_sdk_hal.h``.

when ``BANNER=0``, an macro ``-DNUCLEI_BANNER=0`` will be passed in Makefile.

The banner message looks like this:

.. code-block:: c

    Nuclei SDK Build Time: Jul 23 2021, 10:22:50
    Download Mode: ILM
    CPU Frequency 15999959 Hz


.. _develop_buildsystem_var_v:

V
~

If **V=1**, it will display compiling message in verbose including compiling options.

By default, no compiling options will be displayed in make console message just to print
less message and make the console message cleaner. If you want to see what compiling option
is used, please pass **V=1** in your make command.

.. _develop_buildsystem_var_silent:

SILENT
~~~~~~

If **SILENT=1**, it will not display any compiling messsage.

If you don't want to see any compiling message, you can pass **SILENT=1** in your make command.

.. _develop_buildsystem_app_make_vars:

Makefile variables used only in Application Makefile
----------------------------------------------------

The following variables should be used in application Makefile at your demand,
e.g. ``application/baremetal/demo_timer/Makefile``.

* :ref:`develop_buildsystem_var_target`
* :ref:`develop_buildsystem_var_nuclei_sdk_root`
* :ref:`develop_buildsystem_var_middleware`
* :ref:`develop_buildsystem_var_rtos`
* :ref:`develop_buildsystem_var_stdclib`
* :ref:`develop_buildsystem_var_nmsis_lib`
* :ref:`develop_buildsystem_var_nmsis_lib_arch`
* :ref:`develop_buildsystem_var_riscv_arch`
* :ref:`develop_buildsystem_var_riscv_abi`
* :ref:`develop_buildsystem_var_riscv_cmodel`
* :ref:`develop_buildsystem_var_riscv_tune`
* :ref:`develop_buildsystem_var_nogc`
* :ref:`develop_buildsystem_var_rtthread_msh`

.. _develop_buildsystem_var_target:

TARGET
~~~~~~

This is a necessary variable which must be defined in application Makefile.

It is used to set the name of the application, it will affect the generated
target filenames.

.. warning::

    * Please don't put any spaces in TARGET variable
    * The variable shouldn't contain any space

    .. code-block:: Makefile

        # invalid case 1
        TARGET ?= hello world
        # invalid case 2
        TARGET ?= helloworld # before this # there is a extra space


.. _develop_buildsystem_var_nuclei_sdk_root:

NUCLEI_SDK_ROOT
~~~~~~~~~~~~~~~

This is a necessary variable which must be defined in application Makefile.

It is used to set the path of Nuclei SDK Root, usually it should be set as
relative path, but you can also set absolute path to point to Nuclei SDK.

.. _develop_buildsystem_var_rtos:

RTOS
~~~~

**RTOS** variable is used to choose which RTOS will be used in this application.

You can easily find the supported RTOSes in the **<NUCLEI_SDK_ROOT>/OS** directory.

* If **RTOS** is not defined, then baremetal service will be enabled with this application.
  See examples in ``application/baremetal``.
* If **RTOS** is set the the following values, RTOS service will be enabled with this application.

  - ``FreeRTOS``: FreeRTOS service will be enabled, extra macro ``RTOS_FREERTOS`` will be defined,
    you can include FreeRTOS header files now, and use FreeRTOS API, for ``FreeRTOS`` application,
    you need to have an ``FreeRTOSConfig.h`` header file prepared in you application.
    See examples in ``application/freertos``.
  - ``UCOSII``: UCOSII service will be enabled, extra macro ``RTOS_UCOSII`` will be defined,
    you can include UCOSII header files now, and use UCOSII API, for ``UCOSII`` application,
    you need to have ``app_cfg.h``, ``os_cfg.h`` and ``app_hooks.c`` files prepared in you application.
    See examples in ``application/ucosii``.
  - ``RTThread``: RT-Thread service will be enabled, extra macro ``RTOS_RTTHREAD`` will be defined,
    you can include RT-Thread header files now, and use RT-Thread API, for ``UCOSII`` application,
    you need to have an ``rtconfig.h`` header file prepared in you application.
    See examples in ``application/rtthread``.

.. _develop_buildsystem_var_middleware:

MIDDLEWARE
~~~~~~~~~~

**MIDDLEWARE** variable is used to select which middlewares should be used in this application.

You can easily find the available middleware components in the **<NUCLEI_SDK_ROOT>/Components** directory.

* If **MIDDLEWARE** is not defined, not leave empty, no middlware package will be selected.
* If **MIDDLEWARE** is defined with more than 1 string, such as ``fatfs tjpgd``, then these two
  middlewares will be selected.

.. _develop_buildsystem_var_nmsis_lib:

NMSIS_LIB
~~~~~~~~~

**NMSIS_LIB** variable is used to select which NMSIS libraries should be used in this application.

Currently you can select the following libraries:

* **nmsis_dsp**: NMSIS DSP prebuilt library.
* **nmsis_nn**: NMSIS NN prebuilt library.

You can select more than libraries of NMSIS. For example, if you want to use NMSIS NN library,
NMSIS DSP library is also required. so you need to set **NMSIS_LIB** like this ``NMSIS_LIB := nmsis_nn nmsis_dsp``

.. _develop_buildsystem_var_nmsis_lib_arch:

NMSIS_LIB_ARCH
~~~~~~~~~~~~~~

This variable is used to select real nmsis dsp/nn library arch used, if not set, it will use **RISCV_ARCH** passed.

The **NMSIS_LIB_ARCH** need to match the prebuilt libraries located in **NMSIS/Library/DSP/GCC** and **NMSIS/Library/NN/GCC**, eg. ``NMSIS_LIB_ARCH := rv32imafc_zfh_zvfh_zve32f_zba_zbb_zbc_zbs_xxldspn1x`` will select ``libnmsis_dsp_rv32imafc_zfh_zvfh_zve32f_zba_zbb_zbc_zbs_xxldspn1x.a`` if ``NMSIS_LIB := nmsis_dsp``

This is useful when you want to specify a different arch for library.

eg. When your cpu arch is ``rv32imafdc_zba_zbb_zbc_zbs_zk_zks_xxldspn3x``, and you want to use ``rv32imafdc_zba_zbb_zbc_zbs_xxldspn1x``, then you can set **NMSIS_LIB_ARCH=rv32imafdc_zba_zbb_zbc_zbs_xxldspn1x** in Makefile, otherwise it will use the real cpu arch passed by **CORE** and **ARCH_EXT** or directly via **RISCV_ARCH**.

.. _develop_buildsystem_var_stdclib:

STDCLIB
~~~~~~~

**STDCLIB** variable is used to select which standard c runtime library will be used.
If not defined, the default value will be ``newlib_nano``.

In Nuclei GNU Toolchain, we destributed newlib/newlib-nano/Nuclei c runtime library,
so user can select different c runtime library according to their requirement.

Newlib is a simple ANSI C library, math library, available for both RV32 and RV64.

Nuclei C runtime library is a highly optimized c library designed for deeply embedded user cases,
can provided smaller code size and highly optimized floating point support compared to Newlib.

From 0.5.0 release, to support both gcc and clang compiler, we decided not to use ``--specs=`` option to
select system library, instead of that, we start to use ``--nodefaultlibs`` options, and link the required
system libraries by the ``STDCLIB`` variable choice, so need to link desired libraries such as:

* ``-lgcc``: a standard library (linked by default, excluded by -nodefaultlibs) that provides internal subroutines to overcome shortcomings of particular machines, see https://gcc.gnu.org/onlinedocs/gccint/Libgcc.html.
* ``-lgcov``: a library used to test coverage program, known as ``gcov/gprof``, see https://gcc.gnu.org/onlinedocs/gcc/Gcov.html
* ``-lc/-lc_nano``: newlib c library or newlib nano c library, see https://sourceware.org/newlib/docs.html
* ``-lm``: newlib math library, see https://sourceware.org/newlib/libm.html
* ``-lstdc++``: gnu standard c++ library, see https://gcc.gnu.org/onlinedocs/libstdc++
* ``-lsemihost``: riscv semihosting library which implement a set of standard I/O and file I/O operations, see https://github.com/riscv-mcu/riscv-newlib/tree/nuclei/newlib-4.3.0/libgloss/riscv
* ``-lnosys``: a set of stub functions which implement a set of standard I/O operations but does nothing, and when link with it, it will throw link warning, see https://github.com/riscv-mcu/riscv-newlib/blob/nuclei/newlib-4.3.0/libgloss/libnosys
* ``-lncrt_pico/-lncrt_nano/-lncrt_small/-lncrt_balanced/-lncrt_fast``: Nuclei libncrt library, it provides pico/nano/small/balanced/fast variant to provide standard c library, math library, and libgcc library features, and need to use together with ``-lheapops_minimal/-lheapops_basic/-lheapops_realtime`` heap operation API, and ``-lfileops_uart/-lfileops_semi/-lfileops_rtt`` file io operation API, when using this libncrt library, please don't link ``-lgcc -lc_nano/-lc -lm -lsemihost -lnosys``, and it also can't link with ``-lstdc++``
* Upgrading libncrt from Nuclei GNU Toolchain 2022.12 to Nuclei Toolchain 2023.10, please change it like this, take **libncrt_small** as example:

  - **asm/c/c++ options**: ``--specs=libncrt_small.specs`` -> ``--specs=libncrt_small.specs`` works for gcc, or ``-isystem=/include/libncrt`` works for both gcc and clang
  - **ld options**: ``--specs=libncrt_small.specs`` -> ``--specs=libncrt_small.specs -lheapops_basic -lfileops_uart`` works for gcc, ``-nodefaultlibs -lncrt_small -lheapops_basic -lfileops_uart`` works for both gcc and clang
  - We recommend you to use later version works for both gcc and clang, ``-nodefaultlibs`` is used to exclude startup crt, libgcc and c library in default gcc or clang, use the version specified by us to use libncrt.

.. list-table:: Available STDCLIB choices
   :widths: 10 70
   :header-rows: 1
   :align: center

   * - **STDCLIB**
     - Description
   * - newlib_full
     - | Normal version of newlib, optimized for speed at cost of size.
       | It provided full feature of newlib, with file io supported.
   * - newlib_fast
     - Newlib nano version, with printf float and scanf float support.
   * - newlib_small
     - Newlib nano version, with printf float support.
   * - newlib_nano
     - Newlib nano version, without printf/scanf float support.
   * - libncrt_fast
     - Nuclei C runtime library optimized for speed, full feature
   * - libncrt_balanced
     - Nuclei C runtime library balanced at speed and code size, full feature
   * - libncrt_small
     - Nuclei C runtime library optimized for code size, full feature
   * - libncrt_nano
     - Nuclei C runtime library optimized for code size, without float/double support
   * - libncrt_pico
     - Nuclei C runtime library optimized for code size, without long/long long/float/double support
   * - nostd
     - no std c library will be used, and don't search the standard system directories for header files
   * - nospec
     - no std c library will be used, not pass any --specs options

.. note::

    * For clang based compiler, if ``-u _print_float`` is not passed in linker options, it may fail
      during link process, so here we pass ``-u _print_float`` for newlib_nano, then it means for
      nuclei_llvm and terapines toolchain, ``STDCLIB=newlib_nano`` equals to ``STDCLIB=newlib_small``
    * Nuclei libncrt library couldn't be used with terapines toolchain, so you can't use any libncrt library
      when you are using terapines toolchain.
    * About Newlib and Newlib nano difference, please check
      https://github.com/riscv-collab/riscv-newlib/blob/riscv-newlib-3.2.0/newlib/README
    * About Nuclei C runtime library, it provided basic libgcc, c library and math library feature, but
      it didn't provided all the features that newlib can do, it is highly optimized for deeply embedded scenery,
      user no need to link with ``-lm`` when using libncrt library when math library is needed.
    * Nuclei C runtime library is only available in Nuclei GNU Toolchain released after Nov 2021,
      about how to use this library, please follow doc located in ``gcc\share\pdf``, changes need
      to be done in startup code, linker script, stub code, and compiler options, you can check commit
      history of nuclei sdk for support of libncrt.
    * Nuclei C runtime library(libncrt) only support RV32 CPU target, so you cannot use it with RV64 CPU.
    * Since there are different c runtime library can be chosen now, so developer
      need to provide different stub functions for different library, please check
      ``SoC/evalsoc/Common/Source/Stubs/`` and ``SoC/evalsoc/build.mk`` for example.

.. _develop_buildsystem_var_ncrtheap:

NCRTHEAP
~~~~~~~~

.. note::

    * This variable is added in 0.5.0 release to support libncrt v3.0.0.

This variable is only valid when using libncrt c library >= v3.0.0, and you can choose different
heapops when using libncrt c library to do heap related operations such as malloc or free.

* **basic**: default, this is previous release of libncrt c library used one. A low-overhead best-fit heap where allocation and deallocation have very little internal fragmentation
* **realtime**: A real-time heap where allocation and deallocation have O(1) performance
* **minimal**: An allocate-only heap where deallocation and reallocation are not implemented

For previous libncrt library, this heapops is default binded with libncrt library, so you can't
choose different heap type, but now you can choose according to your requirements.

.. _develop_buildsystem_var_ncrtio:

NCRTIO
~~~~~~

.. note::

    * This variable is added in 0.5.0 release to support libncrt v3.0.0.

This variable is only valid when using libncrt c library >= v3.0.0, and you can choose different
fileops when using libncrt c library to do basic input/output operations.

* **uart**: default, lower level input/output via uart, developer need to implement metal_tty_putc/getc
* **semi**: input/output via semihosting, if you pass **SEMIHOST=1** in make, it will default choose this one when using libncrt library.
* **rtt**: input/output via jlink rtt, require to use JLink tool.

.. _develop_buildsystem_var_smp:

SMP
~~~

**SMP** variable is used to control smp cpu core count, valid number must > 1.

When **SMP** variable is defined, extra gcc options for ld is passed
``-Wl,--defsym=__SMP_CPU_CNT=$(SMP)``, and extra c macro ``-DSMP_CPU_CNT=$(SMP)``
is defined this is passed in each SoC's build.mk, such as ``SoC/evalsoc/build.mk``.

When SMP variable is defined, extra openocd command ``set SMP $(SMP)`` will also
be passed when run openocd upload or create a openocd server.

For SMP application, please check ``application/baremetal/smphello``, if you want to implement
a smp application, you need to reimplement ``smp_main``, which all harts will run to this function
instead of ``main``, if you don't implement it, a weak ``smp_main`` in ``startup_<Device>.S`` will
be used, and only boot hartid specified by **BOOT_HARTID** will enter to main, other harts will do wfi.

.. _develop_buildsystem_var_boot_hartid:

BOOT_HARTID
~~~~~~~~~~~

.. note::

   * This new variable **BOOT_HARTID** is added in ``0.4.0`` release

This variable is used to control the boot hartid in a multiple core system.
If **SMP** variable is specified, it means this application is expected to be a smp application,
otherwise it means this application is expected to be a amp application.

For amp application, only the boot hart specified by **BOOT_HARTID** will run, other harts
will directly do wfi when startup, but for smp application, other hartid will do normal boot
code instead of code/data/bss init, and do sync harts to make sure all harts boots.

For both amp and smp application, the program should execute on a share memory which all
harts can access, not hart private memory such as ilm/dlm.

Currently **SMP** and **BOOT_HARTID** support all require SOC support code to implement it, currently
evalsoc support it, currently qemu simulation didn't work for SMP/AMP use case.

Here is some basic usage for SMP and BOOT_HARTID on UX900 x4, run on external ddr.

.. code-block:: shell

    # cd to helloworld
    cd <Nuclei SDK>/application/baremetal/helloworld
    # clean program
    make SOC=evalsoc CORE=ux900 clean
    # AMP: choose hart 1 as boot hartid, other harts spin
    make SOC=evalsoc CORE=ux900 BOOT_HARTID=1 DOWNLOAD=ddr clean upload
    cd <Nuclei SDK>/application/baremetal/smphello
    # SMP: choose hart 2 as boot hartid
    make SOC=evalsoc CORE=ux900 BOOT_HARTID=2 SMP=4 DOWNLOAD=ddr clean upload

.. _develop_buildsystem_var_hartid_ofs:

HARTID_OFS
~~~~~~~~~~

.. note::

   * This new variable is added in ``0.5.0`` release

This variable is used to set hartid offset relative to real hart index in a complex AMP SoC system.

eg.

In a SoC system, it has 2 CPU, CPU 0 has 2 smp core, CPU 1 has 1 core, and CPU 0 hartid is 0, 1,
and CPU 1 hartid is 2, so for CPU 0, HARTID_OFS is 0, for CPU 1, HARTID_OFS is 2.

.. _develop_buildsystem_var_stacksz:

STACKSZ
~~~~~~~

**STACKSZ** variable is used to control the per core stack size reserved in linker script,
this need to cooperate with link script file and linker options.

In link script file, ``__STACK_SIZE`` symbol need to use ``PROVIDE`` feature of ld
to define a weak version, such as ``PROVIDE(__STACK_SIZE = 2K);``, and gcc will pass
ld options ``-Wl,--defsym=__STACK_SIZE=$(STACKSZ)`` to overwrite the default value if
**STACKSZ** is defined.

**STACKSZ** variable must be a valid value accepted by ld, such as 0x2000, 2K, 4K, 8192.

For SMP version, stack size space need to reserve **STACKSZ** x SMP Core Count size.

You can refer to ``SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/gcc_evalsoc_ilm.ld`` for smp version.

.. _develop_buildsystem_var_heapsz:

HEAPSZ
~~~~~~

**HEAPSZ** variable is used to control the heap size reserved in linker script,
this need to cooperate with link script file and linker options.

In link script file, ``__HEAP_SIZE`` symbol need to use ``PROVIDE`` feature of ld
to define a weak version, such as ``PROVIDE(__HEAP_SIZE = 2K);``, and gcc will pass
ld options ``-Wl,--defsym=__HEAP_SIZE=$(HEAPSZ)`` to overwrite the default value if
**HEAPSZ** is defined.

**HEAPSZ** variable must be a valid value accepted by ld, such as 0x2000, 2K, 4K, 8192.

.. _develop_buildsystem_var_riscv_arch:

RISCV_ARCH
~~~~~~~~~~

**RISCV_ARCH** variable is used to control compiler option ``-mcmodel=$(RISCV_ARCH)``.

It might override RISCV_ARCH defined in SoC build.mk, according to your build.mk implementation.

**RISCV_ARCH** might directly affect the gcc compiler option depended on the implementation of SoC build.mk.

Take ``SOC=evalsoc`` for example.

* **CORE=n305 RISCV_ARCH=rv32imafdc_zk_zks RISCV_ABI=ilp32d ARCH_EXT=_zba_zbb_zbc_zbs**, then final compiler options will be
  ``-march=rv32imafdc_zk_zks -mabi=ilp32d -mtune=nuclei-300-series``. The **ARCH_EXT** is ignored.

.. _develop_buildsystem_var_riscv_abi:

RISCV_ABI
~~~~~~~~~

**RISCV_ABI** variable is used to control compiler option ``-mcmodel=$(RISCV_ABI)``.

It might override RISCV_ABI defined in SoC build.mk, according to your build.mk implementation.

.. _develop_buildsystem_var_riscv_cmodel:

RISCV_CMODEL
~~~~~~~~~~~~

**RISCV_CMODEL** is used to control compiler option ``-mcmodel=$(RISCV_CMODEL)``.

For RV32, default value is ``medlow``, otherwise ``medany`` for RV64.

You can set ``RISCV_CMODEL`` to override predefined value.

.. _develop_buildsystem_var_riscv_tune:

RISCV_TUNE
~~~~~~~~~~

**RISCV_TUNE** is used to control compiler option ``-mtune=$(RISCV_TUNE)``.

It is defined in SoC build.mk, you can override it if your implementation
allow it.

.. _develop_buildsystem_var_app_common_flags:

APP_COMMON_FLAGS
~~~~~~~~~~~~~~~~

.. note::

    * Added in 0.4.0 release.

This variable is used to define app common compiler flags to all c/asm/cpp compiler.
You can pass it via make command to define extra flags to compile application.


.. _develop_buildsystem_var_app_asmflags:

APP_ASMFLAGS
~~~~~~~~~~~~

This variable is similiar to **APP_COMMON_FLAGS** but used to pass extra app asm flags.


.. _develop_buildsystem_var_app_cflags:

APP_CFLAGS
~~~~~~~~~~

This variable is similiar to **APP_COMMON_FLAGS** but used to pass extra app c flags.

.. _develop_buildsystem_var_app_cxxflags:

APP_CXXFLAGS
~~~~~~~~~~~~

This variable is similiar to **APP_COMMON_FLAGS** but used to pass extra app cxx flags.

.. _develop_buildsystem_var_app_ldflags:

APP_LDFLAGS
~~~~~~~~~~~

This variable is similiar to **APP_COMMON_FLAGS** but used to pass extra app linker flags.

.. _develop_buildsystem_var_pfloat:

PFLOAT
~~~~~~

.. note::

    * **Removed** in 0.5.0 release, no longer support it.
    * **Deprecated** variable, please use :ref:`develop_buildsystem_var_stdclib` now
    * ``NEWLIB=nano PFLOAT=1`` can be replaced by ``STDCLIB=newlib_small`` now

.. _develop_buildsystem_var_newlib:

NEWLIB
~~~~~~

.. note::

    * **Removed** in 0.5.0 release, no longer support it.
    * **Deprecated** variable, please use :ref:`develop_buildsystem_var_stdclib` now

.. _develop_buildsystem_var_nogc:

NOGC
~~~~

**NOGC** variable is used to control whether to enable gc sections to reduce program
code size or not, by default GC is enabled to reduce code size.

When GC is enabled, these options will be added:

* Adding to compiler options: ``-ffunction-sections -fdata-sections``
* Adding to linker options: ``-Wl,--gc-sections -Wl,--check-sections``

If you want to enable this GC feature, you can set **NOGC=0** (default), GC feature will
remove sections for you, but sometimes it might remove sections that are useful,
e.g. For Nuclei SDK test cases, we use ctest framework, and we need to set **NOGC=1**
to disable GC feature.

When ``NOGC=0``(default), extra compile options ``-ffunction-sections -fdata-sections``,
and extra link options ``-Wl,--gc-sections -Wl,--check-sections`` will be passed.

.. _develop_buildsystem_var_rtthread_msh:

RTTHREAD_MSH
~~~~~~~~~~~~

**RTTHREAD_MSH** variable is valid only when **RTOS** is set to **RTThread**.

When **RTTHREAD_MSH** is set to **1**:

* The RTThread MSH component source code will be included
* The MSH thread will be enabled in the background
* Currently the msh getchar implementation is using a weak function implemented
  in ``rt_hw_console_getchar`` in ``OS/RTTThread/libcpu/risc-v/nuclei/cpuport.c``

.. _develop_buildsystem_app_build_vars:

Build Related Makefile variables used only in Application Makefile
------------------------------------------------------------------

If you want to specify additional compiler flags, please follow this guidance
to modify your application Makefile.

Nuclei SDK build system defined the following variables to control the
build options or flags.

* :ref:`develop_buildsystem_var_incdirs`
* :ref:`develop_buildsystem_var_c_incdirs`
* :ref:`develop_buildsystem_var_cxx_incdirs`
* :ref:`develop_buildsystem_var_asm_incdirs`
* :ref:`develop_buildsystem_var_srcdirs`
* :ref:`develop_buildsystem_var_c_srcdirs`
* :ref:`develop_buildsystem_var_cxx_srcdirs`
* :ref:`develop_buildsystem_var_asm_srcdirs`
* :ref:`develop_buildsystem_var_c_srcs`
* :ref:`develop_buildsystem_var_cxx_srcs`
* :ref:`develop_buildsystem_var_asm_srcs`
* :ref:`develop_buildsystem_var_exclude_srcs`
* :ref:`develop_buildsystem_var_common_flags`
* :ref:`develop_buildsystem_var_cflags`
* :ref:`develop_buildsystem_var_cxxflags`
* :ref:`develop_buildsystem_var_asmflags`
* :ref:`develop_buildsystem_var_ldflags`
* :ref:`develop_buildsystem_var_ldlibs`
* :ref:`develop_buildsystem_var_libdirs`
* :ref:`develop_buildsystem_var_linker_script`

.. _develop_buildsystem_var_incdirs:

INCDIRS
~~~~~~~

This **INCDIRS** is used to pass C/CPP/ASM include directories.

e.g. To include current directory ``.`` and ``inc`` for C/CPP/ASM

.. code-block:: makefile

    INCDIRS = . inc


.. _develop_buildsystem_var_c_incdirs:

C_INCDIRS
~~~~~~~~~

This **C_INCDIRS** is used to pass C only include directories.

e.g. To include current directory ``.`` and ``cinc`` for C only

.. code-block:: makefile

    C_INCDIRS = . cinc


.. _develop_buildsystem_var_cxx_incdirs:

CXX_INCDIRS
~~~~~~~~~~~

This **CXX_INCDIRS** is used to pass CPP only include directories.

e.g. To include current directory ``.`` and ``cppinc`` for CPP only

.. code-block:: makefile

    CXX_INCDIRS = . cppinc


.. _develop_buildsystem_var_asm_incdirs:

ASM_INCDIRS
~~~~~~~~~~~

This **ASM_INCDIRS** is used to pass ASM only include directories.

e.g. To include current directory ``.`` and ``asminc`` for ASM only

.. code-block:: makefile

    ASM_INCDIRS = . asminc


.. _develop_buildsystem_var_srcdirs:

SRCDIRS
~~~~~~~

This **SRCDIRS** is used to set the source directories used to search
the C/CPP/ASM source code files, it will not do recursively.

e.g. To search C/CPP/ASM source files in directory ``.`` and ``src``

.. code-block:: makefile

    SRCDIRS = . src

.. _develop_buildsystem_var_c_srcdirs:

C_SRCDIRS
~~~~~~~~~

This **C_SRCDIRS** is used to set the source directories used to search
the C only source code files(*.c, *.C), it will not do recursively.

e.g. To search C only source files in directory ``.`` and ``csrc``

.. code-block:: makefile

    C_SRCDIRS = . csrc


.. _develop_buildsystem_var_cxx_srcdirs:

CXX_SRCDIRS
~~~~~~~~~~~

This **CXX_SRCDIRS** is used to set the source directories used to search
the CPP only source code files(*.cpp, *.CPP), it will not do recursively.

e.g. To search CPP only source files in directory ``.`` and ``cppsrc``

.. code-block:: makefile

    CXX_SRCDIRS = . cppsrc


.. _develop_buildsystem_var_asm_srcdirs:

ASM_SRCDIRS
~~~~~~~~~~~

This **ASM_SRCDIRS** is used to set the source directories used to search
the ASM only source code files(*.s, *.S), it will not do recursively.

e.g. To search ASM only source files in directory ``.`` and ``asmsrc``

.. code-block:: makefile

    ASM_SRCDIRS = . asmsrc


.. _develop_buildsystem_var_c_srcs:

C_SRCS
~~~~~~

If you just want to include a few of C source files in directories, you can use this
**C_SRCS** variable, makefile wildcard pattern supported.

e.g. To include ``main.c`` and ``src/hello.c``

.. code-block:: makefile

    C_SRCS = main.c src/hello.c

.. _develop_buildsystem_var_cxx_srcs:

CXX_SRCS
~~~~~~~~

If you just want to include a few of CPP source files in directories, you can use this
**CXX_SRCS** variable, makefile wildcard pattern supported.

e.g. To include ``main.cpp`` and ``src/hello.cpp``

.. code-block:: makefile

    CXX_SRCS = main.cpp src/hello.cpp


.. _develop_buildsystem_var_asm_srcs:

ASM_SRCS
~~~~~~~~

If you just want to include a few of ASM source files in directories, you can use this
**ASM_SRCS** variable, makefile wildcard pattern supported.

e.g. To include ``asm.s`` and ``src/test.s``

.. code-block:: makefile

    ASM_SRCS = asm.s src/test.s

.. _develop_buildsystem_var_exclude_srcs:

EXCLUDE_SRCS
~~~~~~~~~~~~

If you just want to exclude a few of c/asm/cpp source files in directories,
you can use this **EXCLUDE_SRCS** variable, makefile wildcard pattern supported.

e.g. To exclude ``test2.c`` and ``src/test3.c``

.. code-block:: makefile

    EXCLUDE_SRCS = test2.c src/test3.c

.. _develop_buildsystem_var_common_flags:

COMMON_FLAGS
~~~~~~~~~~~~

This **COMMON_FLAGS** variable is used to define common compiler flags to all c/asm/cpp compiler.

For example, you can add a newline ``COMMON_FLAGS += -O3 -funroll-loops -fpeel-loops``
in your application Makefile and these options will be passed to C/ASM/CPP compiler.

This variable should be defined in Makefile.


.. _develop_buildsystem_var_cflags:

CFLAGS
~~~~~~

Different to **COMMON_FLAGS**, this **CFLAGS** variable is used to define common compiler flags to C compiler only.

For example, you can add a newline ``CFLAGS += -O3 -funroll-loops -fpeel-loops``
in your application Makefile and these options will be passed to C compiler.

.. _develop_buildsystem_var_cxxflags:

CXXFLAGS
~~~~~~~~

Different to **COMMON_FLAGS**, this **CXXFLAGS** variable is used to define common compiler flags to cpp compiler only.

For example, you can add a newline ``CXXFLAGS += -O3 -funroll-loops -fpeel-loops``
in your application Makefile and these options will be passed to cpp compiler.

.. _develop_buildsystem_var_asmflags:

ASMFLAGS
~~~~~~~~

Different to **COMMON_FLAGS**, this **ASMFLAGS** variable is used to define common compiler flags to asm compiler only.

For example, you can add a newline ``ASMFLAGS += -O3 -funroll-loops -fpeel-loops``
in your application Makefile and these options will be passed to asm compiler.

.. _develop_buildsystem_var_ldflags:

LDFLAGS
~~~~~~~

This **LDFLAGS** is used to pass extra linker flags, for example,
if you want to use standard system libraries when linking, you can add a newline
``LDFLAGS += -nodefaultlibs`` in you application Makefile.

If you want to link with other libraries, please use ``LDLIBS`` and ``LIBDIRS`` variables.

.. _develop_buildsystem_var_ldlibs:

LDLIBS
~~~~~~

This **LDLIBS** variable is library flags or names given to compilers
when they are supposed to invoke the linker.

Non-library linker flags, such as -L, should go in the **LIBDIRS** or **LDFLAGS** variable.

.. _develop_buildsystem_var_libdirs:

LIBDIRS
~~~~~~~

This **LIBDIRS** variable is used to store the library directories, which could
be used together with **LDLIBS**.

For example, if you have a library located in **$(NUCLEI_SDK_ROOT)/Library/DSP/libnmsis_dsp_rv32imac.a**,
and you want to link it, then you can define these lines:

.. code-block:: makefile

   LDLIBS = -lnmsis_dsp_rv32imac
   LIBDIRS = $(NUCLEI_SDK_ROOT)/Library/DSP

.. _develop_buildsystem_var_linker_script:

LINKER_SCRIPT
~~~~~~~~~~~~~

This **LINKER_SCRIPT** variable could be used to set the link script of the application.

By default, there is no need to set this variable, since the build system will define
a default linker script for application according to the build configuration. If you want
to define your own linker script, you can set this variable.

For example, ``LINKER_SCRIPT := gcc.ld``.


.. _GNU Make Standard Library (GMSL): http://sourceforge.net/projects/gmsl/
.. _B 1.0 extension: https://github.com/riscv/riscv-bitmanip/releases/tag/1.0.0
.. _P 0.5.4 extension: https://github.com/riscv/riscv-p-spec/blob/33be869910077afd52653031f18a235b1f9d4442/P-ext-proposal.adoc
.. _K 1.0 extension: https://github.com/riscv/riscv-crypto/releases/tag/v1.0.0
.. _V 1.0 extension: https://github.com/riscv/riscv-v-spec/releases/tag/v1.0
.. _Zc 1.0 extension: https://github.com/riscv/riscv-code-size-reduction/releases/tag/v1.0.4-3
.. _zfh extension: https://wiki.riscv.org/display/HOME/Recently+Ratified+Extensions
.. _zvfh extension: https://github.com/riscv/riscv-v-spec/releases/tag/zvfh
.. _Nuclei Toolchain 2023.10: https://github.com/riscv-mcu/riscv-gnu-toolchain/releases/tag/nuclei-2023.10
