.. _changelog:

Changelog
=========

V0.4.1
------

This is release version ``0.4.1`` of Nuclei SDK.

* Application

  - Add demo_cidu to demo cidu feature of Nuclei RISC-V Processor
  - Add demo_cache to demo ccm feature of Nuclei RISC-V Processor
  - Optimize demo_nice for rv64
  - Fix compile error when -Werror=shadow
  - Update helloworld and smphello due to mhartid changes

* NMSIS

  - Bump NMSIS to 1.1.1 release version, NMSIS DSP/NN prebuilt libraries are built with 1.1.1 release.
  - Add CIDU support via core_feature_cidu.h, and ``__CIDU_PRESENT`` macro is required in ``<Device>.h`` to represent CIDU present or not
  - Add macros of HPM m/s/u event enable, events type, events idx
  - Fix define error of HPM_INIT macro
  - Due to mhartid csr update for nuclei subsystem reference design, two new API added called ``__get_hart_id`` and ``__get_cluster_id``

    - mhartid csr is now used to present cluster id and hart id for nuclei subsystem reference design
    - bit 0-7 is used for hart id in current cluster
    - bit 8-15 is used for cluster id of current cluster
    - for normal nuclei riscv cpu design, the mhartid csr is used as usual, but in NMSIS Core, we only take
      lower 8bits in use cases like systimer, startup code to support nuclei subsystem

* Build System

  - Add semihost support in build system via SEMIHOST make variable, if SEMIHOST=1, will link semihost library, currently only works with newlibc library, not working with libncrt
  - Add support for compile cpp files with suffix like .cc or .CC
  - Remove ``--specs=nosys.specs`` compile options used during compiling, since we have implement almost all necessary newlibc stub functions, no need to link the nosys version, which will throw warning of link with empty newlibc stub functions.

* SoC

  - Fix missing definition of BOOT_HARTID in ``startup_demosoc.S``
  - Update demosoc and evalsoc interrupt id and handler definition for CIDU changes
  - Add ``__CIDU_PRESENT`` macro to control CIDU present or not in ``demosoc.h`` and ``evalsoc.h`` which is the ``<Device>.h``
  - Add uart status get and clear api for evalsoc and demosoc, which is used by cidu demo
  - Add semihost support for all SoCs, currently only works with newlib, ``SEMIHOST=1`` control semihost support
  - Update openocd configuration file to support semihosting feature
  - Add extra run/restart command for openocd debug configuration in smp debug in npk for Nuclei Studio
  - Update smp/boot flow to match mhartid csr update
  - **BOOT_HARTID** is the choosen boot hart id in current cluster, not the full mhartid register value, for example, it the mhartid csr register is 0x0101, and the **BOOT_HARTID** should be set to 1, if you want hart 1 to be boot hart
  - Update and add more newlib stub functions in demosoc/evalsoc/gd32vf103 SoC's newlibc stub implementation, since we are no longer compile with ``--specs=nosys.specs``

* CI

  - Add demo_cidu and demo_cache in ci configuration files, but expect it to run fail when run in qemu
  - Don't check certificate when download tool

* Tools

  - Modify openocd configuration file in nsdk_utils.oy support win32 now
  - Add new feature to generate cpu json when knowing cpu arch in nsdk_runcpu.py script
  - Add runresult_diff.py script to compare the difference of two runresult.xlsx.csvtable.json files, useful when
    do benchmark difference check
  - Add ``--uniqueid <id>`` option for nsdk cli tools

V0.4.0
------

This is release version ``0.4.0`` of Nuclei SDK.

* Application

  - Add :ref:`design_app_demo_pmp` application to demostrate pmp feature.
  - Add :ref:`design_app_demo_spmp` application to demostrate smode pmp feature, spmp is present when TEE feature is enabled.
  - Add :ref:`design_app_demo_smode_eclic` application to demonstrate ECLIC interrupt with TEE feature of Nuclei Processor.
  - Changed ``test/core`` test case due to ``EXC_Frame_Type`` struct member name changes.
  - Fix XS bit set bug in demo_nice application.
  - Add return value in smphello application.

* NMSIS

  - Add ``__CTZ`` count trailing zero API in core_compatiable.h
  - Add ``__switch_mode`` switch risc-v privilege mode API in core_feature_base.h
  - Add ``__enable_irq_s``, ``__disable_irq_s`` smode irq control(on/off) API in core_feature_base.h
  - Add ``__set_medeleg`` exception delegation API in core_feature_base.h
  - Update and add smode eclic related API in core_feature_eclic.h only present when **TEE_PRESENT=1**
  - Optimize APIs of PMP and add ``__set_PMPENTRYx`` and ``__get_PMPENTRYx`` API for easily PMP configuration in core_feature_pmp.h
  - Add spmp related APIs for smode pmp hardware feature when **__SPMP_PRESENT=1**
  - Add per-hart related APIs for systimer such as ``SysTimer_SetHartCompareValue``, ``SysTimer_SetHartSWIRQ`` and etc in core_feature_timer.h, this is mainly needed when configure timer in smode per hart
  - Add TEE related csr macros in riscv_encoding.h
  - Add iregion offset macros and N3/VP mask in riscv_encoding.h and use it in demosoc/evalsoc implementation.
  - Add ``ICachePresent/DCachePresent`` API
  - Don't sub extra cost for BENCH_xxx API
  - Update NMSIS Core/DSP/NN and prebuilt library to version 1.1.0

* Build System

  - Add ``intexc_<Device>_s.S`` asm file into compiling for evalsoc and demosoc
  - Show ``ARCH_EXT`` information when run make info
  - Don't specify elf filename when run gdb, only specify it when do load to avoid some gdb internal error
  - Add ``BOOT_HARTID`` and ``JTAGSN`` support, which need to be done in SoC support code and build system

* SoC

  - Add smode interrupt and exception handling framework for evalsoc and demosoc, for details see code changes.

    - A new section called ``.vector_s`` is added(required in linker script) to store smode vector table which is initialized in ``system_<Device>.c``
    - A new ``intexc_<Device>_s.S`` asm source file is added to handle s-mode interrupt and exception
    - A default smode exception register and handling framework is added in ``system_<Device>.c``
    - **API Changes**: ``Exception_DumpFrame`` parameters changed to add mode passing in ``system_<Device>.c/h``
    - **API Changes**: ``EXC_Frame_Type`` struct member mcause/mepc changed to cause/epc in ``system_<Device>.c/h``

  - Print ``\0`` instead of ``\r`` when do simulation exit for better integration in Nuclei Studio QEMU simulation.
  - Add ``clock`` stub function for libncrt library in demosoc/evalsoc/gd32vf103 SoC support software.
  - Add ``sram`` download mode for evalsoc/demosoc, for details directly check the linker script
  - Change default ``__ICACHE_PRESENT/__DCACHE_PRESENT`` to 1 for evalsoc/demosoc, when evalsoc/demosoc startup, it will
    enable i/d cache if it really present.
  - Update openocd configuration files to remove deprecated command which might not be support in future
  - Merge smp and single core openocd config into one configuration for evalsoc and demosoc
  - Add **BOOT_HARTID** support for evalsoc and demosoc, which is used to specify the boot hartid, used together with **SMP**
    can support SMP or AMP run mode
  - Add **JTAGSN** support to specify a unified hummingbird jtag debugger via ``adapter serial``
  - For AMP support, we can work together with Nuclei Linux SDK, see https://github.com/Nuclei-Software/nsdk_ampdemo
  - Add NPK support for SMP/AMP working mode, and sram download mode

* CI

  - Start to use Nuclei QEMU/Toolchain/OpenOCD 2022.12 in daily ci for gitlab runner

* Tools

  - Add ``httpserver.py`` tool to create a http server on selected folder, good to preview built documentation.
  - Fix many issues related to nsdk_cli scripts when integrated using fpga hardware ci flow.
  - Support extra parsing benchmark python script for nsdk_cli tools, see 5f546fa0
  - Add ``nsdk_runcpu.py`` tool to run fpga baremetal benchmark

* Documentation

  - Add ``make preview`` to preview build documentation.

V0.3.9
------

This is release version ``0.3.9`` of Nuclei SDK.

* Application

  - Add ``lowpower`` application to demonstrate low-power feature of Nuclei Processor.
  - Update ``demo_nice`` application due to RTL change in cpu.
  - Change dhrystone compiling options to match better with Nuclei CPU IP.

* NMSIS

  - Update riscv_encoding.h, a lot of changes in the CSRs and macros, VPU are added.
  - Add nmsis_bench.h, this header file will not be included in nmsis_core.h, if you want to
    use it, please directly include in your source code. It is used to help provide NMSIS
    benchmark and high performance monitor macro helpers.
  - Add hpm related API in core_feature_base.h
  - Add enable/disable vector API only when VPU available

* Build System

  - Fix upload program the pc is not set correctly to _start when cpu is reset in flash programming mode.
  - Add run_qemu_debug/run_xlspike_rbb/run_xlspike_openocd make targets

* SoC

  - Add npk support for smp, required to update ide plugin in Nuclei Studio 2022.04. And also a new version
    of qemu is required, if you want to run in qemu.
  - Add ``evalsoc`` in Nuclei SDK, ``evalsoc`` is a new evaluation SoC for Nuclei RISC-V Core, for next generation
    of cpu evaluation with iregion feature support. ``demosoc`` will be deprecated in future, when all our CPU IP
    provide iregion support.
  - **Important**: A lot of changes are made to linker script of SDK.

    - rodata are placed in data section for ilm/flash/ddrdownload mode, but placed in text section for flashxip download mode.
    - For ilm download mode, if you want to make the generated binary smaller, you can change REGION_ALIAS of DATA_LMA from ``ram`` to ``ilm``.
    - Add ``_text_lma/_text/_etext`` to replace ``_ilm_lma/_ilm/_eilm``, and startup code now using new ld symbols.
    - Use REGION_ALIAS to make linker script portable
    - Linker scripts of gd32vf103/evalsoc/demosoc are all changed.
  - FPU state are set to initial state when startup, not previous dirty state.
  - Vector are enabled and set to initial state when startup, when vector are enabled during compiling.
  - For latest version of Nuclei CPU IP, BPU cold init need many cycles, so we placed bpu enable before enter to main.


V0.3.8
------

This is release version ``0.3.8`` of Nuclei SDK.

* Application

  - Add ``smphello`` application to test baremetal smp support, this will do demonstration
    to boot default 2 core and each hart print hello world.


* NMSIS

  - Some macros used in NMSIS need to expose when DSP present
  - nmsis_core.h might be included twice, it might be included by <Device.h> and <riscv_math.h>


* Build

  - Add ``SYSCLK`` and ``CLKSRC`` make variable for gd32vf103 SoC to set system clock in hz and clock source, such as ``SYSCLK=72000000 CLKSRC=hxtal``
  - Exclude source files using ``EXCLUDE_SRCS`` make variable in Makefile
  - ``C_SRCS/ASM_SRCS/CXX_SRCS`` now support wildcard pattern
  - ``USB_DRV_SUPPORT`` in gd32vf103 is removed, new ``USB_DRIVER`` is introduced, ``USB_DRIVER=device/host/both`` to choose device, host or both driver code.
  - ``SMP``, ``HEAPSZ`` and ``STACKSZ`` make variable are introduced to control stack/heap
    size and smp cpu count used in SDK

* SoC

  - Add libncrt 2.0.0 support for demosoc and gd32vf103, libncrt stub functions need to be adapted, see 2e09b6b0 and 2e09b6b0
  - Fix ram size from 20K to 32K for gd32vf103v_eval and gd32vf103v_rvstar
  - Change demosoc eclic/timer baseaddr to support future cpu iregion feature, see eab28320d and 18109d04
  - Adapt system_gd32vf103.c to support control system clock in hz and clock source via macro **SYSTEM_CLOCK** and **CLOCK_USING_IRC8M** or **CLOCK_USING_HXTAL**
  - Merge various changes for gd32vf103 support from ``gsauthof@github``, see PR #37, #38, #40
  - Remove usb config header files and usb config source code for gd32vf103
  - Change gd32vf103 linker scripts to support ``HEAPSZ`` and ``STACKSZ``
  - Change demosoc linker scripts to support ``HEAPSZ``, ``STACKSZ`` and ``SMP``
  - Add baremetal SMP support for demosoc, user can pass ``SMP=2`` to build for 2 smp cpu.

* Tools

  - Record more flags in ``nsdk_report.py`` such as ``NUCLEI_SDK_ROOT``, ``OPENOCD_CFG`` and ``LINKER_SCRIPT``.
  - Fix nsdk_report.py generated runresult.xls file content is not correct when some application failed
  - Add benchmark c standard script in tools/misc/barebench
  - Change to support ``SMP`` variable

* OS

  - RT_HEAP_SIZE defined in cpuport.c is small, need to be 2048 for msh example when RT_USING_HEAP is enabled
  - Application can define RT_HEAP_SIZE in rtconfig.h to change the size

For detailed changes, please check commit histories since 0.3.7 release.


V0.3.7
------

This is release version ``0.3.7`` of Nuclei SDK.

* Application

  - **CAUTION**: Fix benchmark value not correct printed when print without float c library,
    which means the CSV printed value in previous release is not correct, please take care
  - Add **DHRY_MODE** variable to support different dhrystone run options in dhrystone benchmark, ``ground``, ``inline`` and ``best`` are supported

* NMSIS

  - Bump to v1.0.4
  - Add B-extension support for NMSIS
  - Fix various issues reported in github

* Build
  - add ``showflags`` target to show compiling information and flags
  - add ``showtoolver`` target to show tool version used

* SoC

  - Change all un-registered interrupt default handler to ``default_intexc_handler``, which means user need to register
    the interrupt handler using ``ECLIC_SetVector`` before enable it.
  - Add **RUNMODE** support only in ``demosoc``, internal usage
  - Add jlink debug configuration for gd32vf103 soc

* Tools

  - Update ``nsdk_report.py`` script to support generate benchmark run result in excel.
  - Add ``ncycm`` cycle model runner support in ``nsdk_bench.py``
  - Add ``nsdk_runner.py`` script for running directly on different fpga board with feature of programing fpga bitstream using vivado


For detailed changes, please check commit histories since 0.3.6 release.

V0.3.6
------

This is release version ``0.3.6`` of Nuclei SDK.

* Application

  - update coremark benchmark options for n900/nx900, which can provide better score number
  - benchmark value will be print in float even printf with float is not supported in c library
  - baremetal applications will exit with an return value in main

* NMSIS

  - add ``__CCM_PRESENT`` macro in NMSIS-Core, if CCM hardware unit is present in your CPU,
    ``__CCM_PRESENT`` macro need to be set to 1 in ``<Device>.h``
  - Fixed mtvec related api comment in ``core_feature_eclic.h``
  - Add safely write mtime/mtimecmp register for 32bit risc-v processor
  - rearrage #include header files for all NMSIS Core header files
  - removed some not good #pragma gcc diagnostic lines in ``nmsis_gcc.h``

* Build

  - Add experimental ``run_xlspike`` and ``run_qemu`` make target support
  - ``SIMU=xlspike`` or ``SIMU=qemu`` passed in make will auto exit xlspike/qemu if main function returned

* SoC

  - Add xlspike/qemu auto-exit support for gd32vf103 and demosoc, required next version after Nuclei QEMU 2022.01

For detailed changes, please check commit histories since 0.3.5 release.

V0.3.5
------

This is release version ``0.3.5`` of Nuclei SDK.

.. caution::

    - This version introduce a lot of new features, and required Nuclei GNU Toolchain 2022.01
    - If you want to import as NPK zip package into Nuclei Studio, 2022.01 version is required.
    - If you want to have smaller code size for Nuclei RISC-V 32bit processors, please define ``STDCLIB=libncrt_small``
      in your application Makefile, or change **STDCLIB** defined in ``Build/Makefile.base`` to make it available
      globally.


* Application

  - **DSP_ENABLE** and **VECTOR_ENABLE** are deprecated now in demo_dsp application, please use **ARCH_EXT** to replace it.
    ``ARCH_EXT=p`` equal to ``DSP_ENABLE=ON``, ``ARCH_EXT=v`` equal to ``VECTOR_ENABLE=ON``.
  - ``demo_dsp`` application no need to set include and libraries for NMSIS DSP library, just use ``NMSIS_LIB = nmsis_dsp`` to
    select NMSIS DSP library and set include directory.
  - Update coremark compile options for different Nuclei cpu series, currently
    900 series options and 200/300/600 series options are provided, and can be selected by ``CPU_SERIES``.

      - ``CPU_SERIES=900``: the compiler options for Nuclei 900 series will be selected.
      - otherwise, the compiler options for Nuclei 200/300/600 series will be selected, which is by default for 300
  - Fix ``whetstone`` application compiling issue when compiled with v extension present

* SoC

  - Provide correct gd32vf103.svd, the previous one content is messed up.
  - ``putchar/getchar`` newlib stub are required to be implemented for RT-Thread porting
  - Added support for newly introduced nuclei c runtime library(libncrt).
  - Rearrange stub function folder for gd32vf103 and demosoc to support
    different c runtime library.
  - A lot changes happened in link scripts under SoC folder
    - heap section is added for libncrt, size controlled by ``__HEAP_SIZE``
    - heap start and end ld symbols are ``__heap_start`` and ``__heap_end``
    - stub function ``sbrk`` now using new heap start and end ld symbols
    - tdata/tbss section is added for for libncrt, thread local storage supported
  - For **flash** download mode, vector table are now placed in ``.vtable`` section now instead of ``.vtable_ilm``,
    ``VECTOR_TABLE_REMAPPED`` macro is still required in **DOWNLOAD=flash** mode
  - flash program algo used in openocd for demosoc changed to nuspi, see changes in openocd_demosoc.cfg

* NMSIS

  - Update NMSIS Core/DSP/NN to version 1.0.3, see `NMSIS 1.0.3 Changelog`_
  - Update prebuilt NMSIS DSP/NN library to version 1.0.3 built by risc-v gcc 10.2
  - For NMSIS Core 1.0.3, no need to define ``__RISCV_FEATURE_DSP`` and ``__RISCV_FEATURE_VECTOR``
    for ``riscv_math.h`` now, it is now auto-defined in ``riscv_math_types.h``

* OS

  - Change RT-Thread porting to support libncrt and newlibc, mainly using putchar and getchar

* Build System

  - Introduce :ref:`develop_buildsystem_var_stdclib` makefile variable to support different c library.
  - **NEWLIB** and **PFLOAT** variable is deprecated in this release.
  - Introduce :ref:`develop_buildsystem_var_archext` makefile variable to support b/p/v extension.
  - Only link ``-lstdc++`` library when using **STDCLIB=newlib_xxx**
  - **RISCV_CMODEL** variable is added to choose code model, medlow or medany can be chosen,
    default is ``medlow`` for RV32 otherwise ``medany`` for RV64.
  - **RISCV_TUNE** variable is added to select riscv tune model, for Nuclei CPU, we added ``nuclei-200-series``,
    ``nuclei-300-series``, ``nuclei-600-series`` and ``nuclei-900-series`` in Nuclei RISC-V GNU toolchain >= 2021.12

* Contribution

  - Update contribution guide due to runtime library choices provided now.

* NPK

  - **newlibsel** configuration variable changed to **stdclib**, and is not compatiable.

    - **newlibsel=normal** change to **stdclib=newlib_full**
    - **newlibsel=nano_with_printfloat** changed to **stdclib=newlib_small**
    - **newlibsel=nano** changed to **stdclib=newlib_nano**
    - **stdclib** has more options, please see ``SoC/demosoc/Common/npk.yml``
    - **nuclei_archext** is added as new configuration variable, see ``SoC/demosoc/Common/npk.yml``

* tools

  - generate benchmark values in csv files when running nsdk_bench.py or nsdk_execute.py
  - fix xl_spike processes not really killed in linux environment when running nsdk_bench.py

For detailed changes, please check commit histories since 0.3.4 release.


V0.3.4
------

This is release version ``0.3.4`` of Nuclei SDK.

* CI

    - Fix gitlab ci fail during install required software

* Build System

    - build asm with -x assembler-with-cpp

* Tools

    - Fix ``tools/scripts/nsdk_cli/configs/nuclei_fpga_eval_ci_qemu.json`` description issue for dsp enabled build configs
    - Generate html report when run ``tools/scripts/nsdk_cli/nsdk_bench.py``
    - nsdk_builder.py: modify qemu select cpu args,change ``p`` to ``,ext=p``

* SoC

    - For demosoc, if you choose ilm and ddr download mode, then the data section's LMA is equal to VMA now, and there
      will be no data copy for data section, bss section still need to set to zero.
    - For demosoc, if you choose ilm and ddr download mode, The rodata section are now also placed in data section.

* NPK

    - add ``-x assembler-with-cpp`` in npk.yml for ssp


For detailed changes, please check commit histories since 0.3.3 release.


V0.3.3
------

This is release version ``0.3.3`` of Nuclei SDK.

* NPK

    - Fix NPK issues related to QEMU for demosoc and gd32vf103, and RTOS macro definitions in NPK
    - This SDK release required Nuclei Studio 2021.09-ENG1, 2021.08.18 build version

For detailed changes, please check commit histories since 0.3.2 release.

V0.3.2
------

This is release version ``0.3.2`` of Nuclei SDK.

* Build

    - **Important changes** about build system:

      - The SoC and RTOS related makefiles are moving to its own folder, and controlled By
        **build.mk** inside in in the SoC/<SOC> or OS/<RTOS> folders.
      - Middlware component build system is also available now, you can add you own middleware or library
        into ``Components`` folder, such as ``Components/tjpgd`` or ``Components/fatfs``, and you can include
        this component using make variable ``MIDDLEWARE`` in application Makefile, such as ``MIDDLEWARE := fatfs``,
        or ``MIDDLEWARE := tjpgd fatfs``.
      - Each middleware component folder should create a ``build.mk``, which is used to control
        the component build settings and source code management.
      - An extra ``DOWNLOAD_MODE_STRING`` macro is passed to represent the DOWNLOAD mode string.
      - In ``startup_<Device>.S`` now, we don't use ``DOWNLOAD_MODE`` to handle the vector table location, instead
        we defined a new macro called ``VECTOR_TABLE_REMAPPED`` to stand for whether the vector table's vma != lma.
        If ``VECTOR_TABLE_REMAPPED`` is defined, the vector table is placed in ``.vtable_ilm``, which means the vector
        table is placed in flash and copy to ilm when startup.
    - Change openocd ``--pipe`` option to ``-c "gdb_port pipe; log_output openocd.log"``
    - Remove ``-ex "monitor flash protect 0 0 last off"`` when upload or debug program to avoid error
      when openocd configuration file didn't configure a flash
    - Add ``cleanall`` target in **<NUCLEI_SDK_ROOT>/Makefile**, you can clean all the applications
      defined by ``EXTRA_APP_ROOTDIRS`` variable
    - Fix ``size`` target of build system

* Tools

    - Add ``nsdk_cli`` tools in Nuclei SDK which support run applications

      - **tools/scripts/nsdk_cli/requirements.txt**: python module requirement file
      - **tools/scripts/nsdk_cli/configs**: sample configurations used by scripts below
      - **tools/scripts/nsdk_cli/nsdk_bench.py**: nsdk bench runner script
      - **tools/scripts/nsdk_cli/nsdk_execute.py**: nsdk execute runner script

* SoC

    - Add general bit operations and memory access APIs in ``<Device>.h``, eg. ``_REG32(p, i)``, ``FLIP_BIT(regval, bitofs)``
    - ``DOWNLOAD_MODE_xxx`` macros are now placed in ``<Device>.h``, which is removed from ``riscv_encoding.h``, user can define
      different ``DOWNLOAD_MODE_xxx`` according to its device/board settings.
    - ``DOWNLOAD_MODE_STRING`` are now used to show the download mode string, which should be passed eg. ``-DOWNLOAD_MODE_STRING=\"flash\"``,
      it is used in ``system_<Device>.c``
    - ``DOWNLOAD_MODE_xxx`` now is used in ``startup_<Device>.S`` to control the vector table location,
      instead a new macro called ``VECTOR_TABLE_REMAPPED`` is used, and it should be defined in ``SoC/<SOC>/build.mk``
      if the vector table's LMA and VMA are different.

* NMSIS

    - Bump NMSIS to version 1.0.2

* OS

    - Fix OS task switch bug in RT-Thread

V0.3.1
------

This is official version ``0.3.1`` of Nuclei SDK.

.. caution::

    - We are using ``demosoc`` to represent the Nuclei Evaluation SoC for customer to replace the old name ``hbird``.
    - The ``hbird`` SoC is renamed to ``demosoc``, so the ``SoC/hbird`` folder is renamed to ``SoC/demosoc``,
      and the ``SoC/hbird/Board/hbird_eval`` is renamed to ``SoC/demosoc/Board/nuclei_fpga_eval``.

* SoC

    - board: Add support for TTGO T-Display-GD32, contributed by `tuupola`_
    - Add definitions for the Interface Association Descriptor of USB for GD32VF103, contributed by `michahoiting`_.
    - **IMPORTANT**: ``hbird`` SoC is renamed to ``demosoc``, and ``hbird_eval`` is renamed to ``nuclei_fpga_eval``

      - Please use ``SOC=demosoc BOARD=nuclei_fpga_eval`` to replace ``SOC=hbird BOARD=hbird_eval``
      - The changes are done to not using the name already used in opensource Hummingbird E203 SoC.
      - Now ``demosoc`` is used to represent the Nuclei Demo SoC for evaluation on Nuclei FPGA evaluation Board(MCU200T/DDR200T)

* Documentation

    - Update ``msh`` application documentation
    - Add basic documentation for **TTGO T-Display-GD32**
    - Add Platformio user guide(written in Chinese) link in get started guide contributed by Maker Young

* Application

    - Increase idle and finsh thread stack for RT-Thread, due to stack size is not enough for RISC-V 64bit
    - Set rt-thread example tick hz to 100, and ucosii example tick hz to 50

* Build

    - Format Makefile space to tab
    - Add $(TARGET).dasm into clean targets which are missing before

* Code style

    - Format source files located in application, OS, SoC, test using astyle tool

V0.3.0
------

This is official version ``0.3.0`` of Nuclei SDK.

* SoC

    - Add more newlib stub functions for all SoC support packages
    - Dump extra csr ``mdcause`` in default exception handler for hbird
    - Add Sipeed Longan Nano as new supported board
    - Add **gd32vf103c_longan_nano** board support, contributed by `tuupola`_ and `RomanBuchert`_

* Documentation

    - Add ``demo_nice`` application documentation
    - Add ``msh`` application documentation
    - Update get started guide
    - Add **gd32vf103c_longan_nano** board Documentation
    - Update board documentation structure levels

* Application

    - Cleanup unused comments in dhrystone
    - Add new ``demo_nice`` application to show Nuclei NICE feature
    - Add new ``msh`` application to show RT-Thread MSH shell component usage

* NMSIS

    - Fix typo in CLICINFO_Type._reserved0 bits
    - Fix ``__STRBT``, ``__STRHT``, ``__STRT`` and ``__USAT`` macros

* OS

    - Add ``msh`` component source code into RT-Thread RTOS source code
    - Add ``rt_hw_console_getchar`` implementation

* Build

    - Add ``setup.ps1`` for setting up environment in windows powershell

V0.2.9
------

This is official version ``0.2.9`` of Nuclei SDK.

* SoC

    - Remove ``ftdi_device_desc "Dual RS232-HS"`` line in openocd configuration.

      .. note::

         Newer version of RVSTAR and Hummingbird Debugger have changed the FTDI description
         from "Dual RS232-HS" to "USB <-> JTAG-DEBUGGER", to be back-compatiable with older
         version, we just removed this ``ftdi_device_desc "Dual RS232-HS"`` line.
         If you want to select specified JTAG, you can add this ``ftdi_device_desc`` according
         to your description.

    - Fix typos in **system_<Device>.c**
    - Fix gpio driver implementation bugs of hbird
    - Enable more CSR(micfg_info, mdcfg_info, mcfg_info) show in gdb debug

* Documentation

    - Add more faqs

* Build System

    - Remove unnecessary upload gdb command
    - Remove upload successfully message for ``make upload``


V0.2.8
------

This is the official release version ``0.2.8`` of Nuclei SDK.

* SoC

    - Fixed implementation for ``_read`` newlib stub function, now scanf
      can be used correctly for both gd32vf103 and hbird SoCs.

* Misc

    - Update platformio package json file according to latest platformio requirements


V0.2.7
------

This is the official release version ``0.2.7`` of Nuclei SDK.

* OS

    - Fix OS portable code, configKERNEL_INTERRUPT_PRIORITY should
      set to default 0, not 1. 0 is the lowest abs interrupt level.

* Application

    - Fix configKERNEL_INTERRUPT_PRIORITY in FreeRTOSConfig.h to 0

* NMSIS

    - Change timer abs irq level setting in function SysTick_Config from 1 to 0


V0.2.6
------

This is the official release version ``0.2.6`` of Nuclei SDK.

* Application

    - Fix typo in rtthread demo code
    - Update helloworld application to parse vector extension

* NMSIS

    - Update NMSIS DSP and NN library built using NMSIS commit 3d9d40ff

* Documentation

    - Update quick startup nuclei tool setup section
    - Update build system documentation
    - Fix typo in application documentation

V0.2.5
------

This is the official release version ``0.2.5`` of Nuclei SDK.

This following changes are maded since ``0.2.5-RC1``.

* SoC

  - For **SOC=hbird**, in function ``_premain_init`` of ``system_hbird.c``, cache will be enable in following cases:

    - If ``__ICACHE_PRESENT`` is set to 1 in ``hbird.h``, I-CACHE will be enabled
    - If ``__DCACHE_PRESENT`` is set to 1 in ``hbird.h``, D-CACHE will be enabled

* Documentation

  - Fix several invalid cross reference links

* NMSIS

  - Update and use NMSIS 1.0.1


V0.2.5-RC1
----------

This is release ``0.2.5-RC1`` of Nuclei SDK.

* Documentation

  - Fix invalid links used in this documentation
  - Rename `RVStar` to `RV-STAR` to keep alignment in documentation

* NMSIS

  - Update and use NMSIS 1.0.1-RC1
  - Add NMSIS-DSP and NMSIS-NN library for RISC-V 32bit and 64bit
  - Both RISC-V 32bit and 64bit DSP instructions are supported

* SoC

  - All startup and system init code are adapted to match design changes of NMSIS-1.0.1-RC1

    - `_init` and `_fini` are deprecated for startup code, now please use `_premain_init` and `_postmain_fini` instead
    - Add `DDR` download mode for Hummingbird SoC, which downloaded program into DDR and execute in DDR


V0.2.4
------

This is release ``0.2.4`` of Nuclei SDK.

* Application

  - Upgrade the ``demo_dsp`` application to a more complicated one, and by default,
    ``DSP_ENABLE`` is changed from ``OFF`` to ``ON``, optimization level changed from
    ``O2`` to no optimization.

* SoC

  - Update openocd configuration file for Hummingbird FPGA evaluation board,
    If you want to use ``2-wire`` mode of JTAG, please change ``ftdi_oscan1_mode off``
    in ``openocd_hbird.cfg`` to ``ftdi_oscan1_mode on``.
  - Add ``delay_1ms`` function in all supported SoC platforms
  - Fix bugs found in uart and gpio drivers in hbird SoC
  - Move ``srodata`` after ``sdata`` for ILM linker script
  - Change bool to BOOL to avoid cpp compiling error in gd32vf103
  - Fix ``adc_mode_config`` function in gd32vf103 SoC

* Build System

  - Add **GDB_PORT** variable in build system, which is used to specify the gdb port
    of openocd and gdb when running ``run_openocd`` and ``run_gdb`` targets
  - Add Nuclei N/NX/UX 600 series core configurations into *Makefile.core*
  - Add -lstdc++ library for cpp application
  - Generate hex output for dasm target
  - Optimize Makefile to support MACOS


V0.2.3
------

This is release ``0.2.3`` of Nuclei SDK.

* OS

  - Add **RT-Thread 3.1.3** as a new RTOS service of Nuclei SDK, the kernel source
    code is from RT-Thread Nano project.
  - Update UCOSII source code from version ``V2.91`` to ``V2.93``
  - The source code of UCOSII is fetched from https://github.com/SiliconLabs/uC-OS2/
  - **Warning**: Now for UCOSII application development, the ``app_cfg.h``, ``os_cfg.h``
    and ``app_hooks.c`` are required, which can be also found in
    https://github.com/SiliconLabs/uC-OS2/tree/master/Cfg/Template

* Application

  - Add **RT-Thread** demo application.
  - Don't use the ``get_cpu_freq`` function in application code, which currently is only
    for internal usage, and not all SoC implementations are required to provide this function.
  - Use ``SystemCoreClock`` to get the CPU frequency instead of using ``get_cpu_freq()`` in
    ``whetstone`` application.
  - Update UCOSII applications due to UCOSII version upgrade, and application development
    for UCOSII also required little changes, please refer to :ref:`design_rtos_ucosii`
  - Fix ``time_in_secs`` function error in ``coremark``, and cleanup ``coremark`` application.

* Documentation

  - Add documentation about RT-Thread and its application development.
  - Update documentation about UCOSII and its application development.
  - Update ``coremark`` application documentation.

* Build System

  - Add build system support for RT-Thread support.
  - Build system is updated due to UCOSII version upgrade, the ``OS/UCOSII/cfg`` folder
    no longer existed, so no need to include it.

* SoC

  - Update SoC startup and linkscript files to support RT-Thread

* Misc

  - Add ``SConscript`` file in Nuclei SDK root, this file is used by RT-Thread package.

V0.2.2
------

This is release ``0.2.2`` of Nuclei SDK.

* OS

  - Update UCOSII portable code
  - Now both FreeRTOS and UCOSII are using similar portable code,
    which both use ``SysTimer Interrupt`` and ``SysTimer Software Interrupt``.

* Documentation

  - Update documentation about RTOS

V0.2.1
------

This is release ``0.2.1`` of Nuclei SDK.

* Build System

  - Add extra linker options ``-u _isatty -u _write -u _sbrk -u _read -u _close -u _fstat -u _lseek``
    in Makefile.conf to make sure if you pass extra ``-flto`` compile option, link phase will not fail

* Documentation

  - Add documentation about how to optimize for code size in application development, using ``demo_eclic``
    as example.

* OS

  - Update FreeRTOS to version V10.3.1
  - Update FreeRTOS portable code

* NMSIS

  - Update NMSIS to release ``v1.0.0-beta1``


V0.2.0-alpha
------------

This is release ``0.2.0-alpha`` of Nuclei SDK.

* Documentation

  - Initial verison of Nuclei SDK documentation
  - Update Nuclei-SDK README.md

* Application

  - Add ``demo_eclic`` application
  - Add ``demo_dsp`` application
  - ``timer_test`` application renamed to ``demo_timer``

* Build System

  - Add comments for build System
  - Small bug fixes

* **NMSIS**

  - Change ``NMSIS/Include`` to ``NMSIS/Core/Include``
  - Add ``NMSIS/DSP`` and ``NMSIS/NN`` header files
  - Add **NMSIS-DSP** and **NMSIS-NN** pre-built libraries


V0.1.1
------

This is release ``0.1.1`` of Nuclei SDK.

Here are the main features of this release:

* Support Windows and Linux development in command line using Make

* Support development using PlatformIO, see https://github.com/Nuclei-Software/platform-nuclei

* Support Humming Bird FPGA evaluation Board and GD32VF103 boards

  - The **Humming Bird FPGA evaluation Board** is used to run evaluation FPGA bitstream
    of Nuclei N200, N300, N600 and NX600 processor cores
  - The **GD32VF103 boards** are running using a real MCU from Gigadevice which is using
    Nuclei N200 RISC-V processor core

* Support different download modes flashxip, ilm, flash for our FPGA evaluation board


.. _Nuclei-SDK: https://github.com/Nuclei-Software/nuclei-sdk
.. _tuupola: https://github.com/tuupola
.. _RomanBuchert: https://github.com/RomanBuchert
.. _michahoiting: https://github.com/michahoiting
.. _NMSIS 1.0.3 Changelog: https://doc.nucleisys.com/nmsis/changelog.html#v1-0-3
