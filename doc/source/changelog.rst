.. _changelog:

Changelog
=========

V0.8.0
------

.. note::

    - Two new benchmark cases ``dhrystone_v2.2`` and ``whetstone_v1.2`` are added in this release.
    - In Nuclei Studio IDE, if you are importing this Nuclei SDK 0.8.0 as a NPK package, you will be able to see following versions in new project wizard:

      - **Dhrystone Benchmark, Version 2.1**: located in ``application/baremetal/benchmark/dhrystone``, previous existed version
      - **Whetstone Benchmark, Roy Longbottom Version**: located in ``application/baremetal/benchmark/whetstone``, previous existed version
      - **Dhrystone Benchmark, Version 2.2**: located in ``application/baremetal/benchmark/dhrystone_v2.2``, **new** introduced version
      - **Whetstone Benchmark, Netlib Version 1.2**: located in ``application/baremetal/benchmark/whetstone_v1.2``, **new** introduced version

This is release version ``0.8.0`` of Nuclei SDK.

* NMSIS

  - Fix wrong macro ``PLIC_GetThreshold`` & ``PLIC_GetThreshold_S`` implementation for ``core_feature_plic.h``
  - Add ``MTIME_SRW_CTRL`` bitfields in **SysTimer_Type** structure for ``core_feature_timer.h``
  - Optimize ECLIC API for better code performance in ``core_feature_eclic.h``
  - Add SSTC support in ``core_feature_timer.h``, a new macro called ``__SSTC_PRESENT`` is added
  - Update and add more CSR Union types
  - Add more CSR macros such shartid csr, worldguard csrs, and related csr bitfield macro
  - Add the ``BENCH_XLEN_MODE`` macro to enable more accurate cycle and HPM counter measurements for **RV32**,
    when ``BENCH_XLEN_MODE`` is enabled, the cycle/instret/time/hpm_counter will be 32 bits for rv32 and 64 bits for rv64.
  - Fix return type error of ``__get_hpm_counter``
  - Add new APIs to read ``cycle/instret/time/hpm_counter`` with ``XLEN`` bits:

    - ``unsigned long __read_cycle_csr()``
    - ``unsigned long __read_instret_csr()``
    - ``unsigned long __read_time_csr()``
    - ``unsigned long __read_hpm_counter(unsigned long idx)``
  - Fix ``__clear_core_irq_pending`` and ``__clear_core_irq_pending_s`` implementation in ``core_feature_base.h``
  - Fix ``__enable_sw_irq_s`` implementation in ``core_feature_base.h``
  - Add PMA(Physical Memory Attribute) APIs for managing attribute type(Device/Non-Cacheable/Cacheable) of memory regions when **__PMA_PRESENT=1**
  - Fix and update HPM v1 event macro due to Nuclei ISA documentation update in ``nmsis_bench.h``
  - Add new PMU v1 and v2 event macros in ``nmsis_bench.h``
  - Add ``flushpipe`` and ``fence`` in each ccm operation API in ``core_feature_cache.h``

* Application

  - Add more application code compile check message for better example requirement explanation
  - Add :ref:`design_app_demo_sstc` to show how to SSTC(S-Mode timer interrupt extension)
  - Add :ref:`design_app_rtthread_demo_smode` to show how to run rt_thread in S-Mode, it will require TEE and PMP extension
  - Remove ``demo_spmp`` application due to hw ``sPMP`` upgraded to ``sMPU`` and no longer supported,
  - please use :ref:`design_app_demo_smpu` now.
  - Add ``-fno-tree-tail-merge`` compiler option for threadx RTOS example compiling, which is required for correct
  - compiling
  - Fix :ref:`design_app_demo_vnice` insufficient mask length when vlen > 128
  - Add more documentation for :ref:`design_app_demo_dsp` example
  - Optimize :ref:`design_app_smphello` spinlock usage and update doc for it
  - Optimize :ref:`design_app_demo_profiling` example execution speed on hw from about 5min to 30s by decease the loop count
  - Update :ref:`design_app_freertos_demo` example to use ``configTICK_TYPE_WIDTH_IN_BITS`` instead of ``configUSE_16_BIT_TICKS``
  - Add :ref:`design_app_demo_pma` case to show how to use PMA related API in ``core_feature_pma.h``
  - Add :ref:`design_app_demo_smode_plic` to show how to use PLIC in S-Mode, it will require PLIC and PMP extension
  - Increase freertos timer stack size from ``256`` to ``512`` due to timer task still generate vector instruction even with :ref:`AUTOVEC=0 <develop_buildsystem_var_autovec>`
  - Add two new benchmark cases :ref:`design_app_dhrystone_v2.2` and :ref:`design_app_whetstone_v1.2` which are the ones used in linux benchmark

* SoC

  - Add more documentation about IAR compiler support and porting notes, especially the vector table alignment with the ``MTVT`` CSR.
  - Add ``nx1000/nx1000f/nx1000fd/ux1000/ux1000f/ux1000fd`` in supported CPU :ref:`develop_buildsystem_var_core` list
  - Only enable i/d cache when ecc not present in evalsoc startup asm code to avoid x-state propagation during rtl simulation
  - Fix ``#endif`` not placed correctly when XLCFG_TEE=1 and CODESIZE=1 in system_evalsoc.c
  - Only initialize ECLIC SMode related registers when TEE really present for evalsoc
  - Place default vector entry for vector_table_s when SSTC present for evalsoc
  - Add ``#define _DEFAULT_SOURCE`` in all SoC's newlibc stub implementation to use BSD Standard API when compiler c standard is not gnu c standard ``-std=gnu23``,
    such as ``-std=c23``, to fix compiler error ``error: implicit declaration of function 'TIMEVAL_TO_TIMESPEC' [-Wimplicit-function-declaration]``
  - Add ``__SMODE_PRESENT`` macro in ``evalsoc.h`` to represent s-mode present or not
  - Add support for smode clint and plic support for evalsoc
  - Add a README.md to introduce evalsoc reference implementation of NMSIS Device Templates in ``SoC/evalsoc/README.md``

* RTOS

  - Add S-Mode RT-Thread support which rely on TEE feature, SSTC feature is preferred
  - Update FreeRTOS port to use ``configTICK_TYPE_WIDTH_IN_BITS`` instead of ``configUSE_16_BIT_TICKS``
  - Cherry-pick a FreeRTOS incorrect error checking of prvCreateIdleTasks fix, see https://github.com/FreeRTOS/FreeRTOS-Kernel/commit/a49c35b5dc0f1f521eef3ef993d401af7f26f439
  - Add ThreadX module support for both RISC-V 32 and 64 bit
  - Add FreeRTOS lazy fp/vector registers save and restore support

* Build System

  - Add **COMPILE_PREFIX** support for :ref:`develop_buildsystem_var_toolchain` ``nuclei_llvm``, now both ``nuclei_llvm`` and ``nuclei_gnu`` support this variable, you can change it like this ``COMPILE_PREFIX=/path/to/newgcc/bin/riscv64-unknown-elf-`` when do make command
  - Add :ref:`develop_buildsystem_var_autovec` make variable, when **AUTOVEC=0**, it will disable auto vectorization as much as possible, this is useful for some application which require no auto vectorization
  - Add ``GDB_UPLOAD_EXTRA_CMDS`` make variable to execute extra commands after upload elf file to target

* Tools

  - Add exclusive lock when program fpga for ``nsdk_cli`` tools
  - Update ``hpm_parse.py`` to match hpm v1 and v2 update

V0.7.1
------

This is release version ``0.7.1`` of Nuclei SDK.

* NMSIS

  - Fix Cache CCM related API compile fail using c++ compiler
  - **mfp16mode** csr is renamed to **mmisc_ctl1** due to hw changes
  - Update prebuilt NMSIS DSP/NN library to release 1.3.1

* SoC

  - Only call ``EnableSUCCM`` in ``_premain_init`` process when CCM present and S/U mode present defined in auto generated cpufeature.h

* Misc

  - Fix various typos found in source code and doc
  - Recommend evalsoc user to run :ref:`design_app_cpuinfo` to check cpu features it present
  - If you want to do openocd rtos aware debug, you need to follow note in commit b7ed34e96
  - Evalsoc uart eclic irq maybe not working due to different cpu configuration

V0.7.0
------

This is release version ``0.7.0`` of Nuclei SDK.


* Application

  - Add ``demo_plic`` case to show how to use PLIC related API in PLIC interrupt mode.
  - Add ``demo_clint_timer`` case to show how to use systimer in CLINT interrupt mode not ECLIC interrupt mode.
  - Update ``demo_pmp`` case to make it suitable for when PMP not present.
  - Change download mode from ``ddr`` to ``sram`` for smp and cache cases to be suitable for some custom soc sdk.

* NMSIS

  - Add more ECC related macros for ``milm_ctl/mdlm_ctl/mcache_ctl`` csr
  - Add more PLIC interrupt API in ``core_feature_plic.h``
  - Add more interrupt related API when in plic interrupt mode, see changes in ``core_feature_base.h``
  - Bump NMSIS version to 1.3.0 with updated NMSIS Core/DSP/NN header files and prebuilt library

* SoC

  - Add **Terapines ZCC NPK** support, require Nuclei Studio >= 2024.06
  - Merge newlib stub code from many files into one file called ``stubs.c`` for all SoC supported in Nuclei SDK
  - Enable I/D cache for evalsoc before data/bss initialization steps using ``cpufeature.h`` for faster data initialization
  - gd32vf103 default CORE name changed from ``n205`` to ``n203`` which are the same in software
  - gd32vw55x default CORE name changed from ``n307fd`` to ``n300fd`` which are the same in software
  - evalsoc default CORE name changed from ``n307fd`` to ``n300fd`` which are the same in software
  - Add plic interrupt and exception related handling code for evalsoc
  - Fix BPU is not enabled during startup for startup code for IAR compiler, which will increase performance of 600/900/1000 series a lot

* Build System

  - Introduce ``XLCFG_xxx`` make variable for evalsoc which is only internally used by Nuclei to overwrite default cpufeature.h macro definition, which will be useful for some applications such as demo_cidu, demo_cache, demo_spmp, demo_smpu and demo_smode_eclic
  - Introduce ``ECC_EN`` make variable for evalsoc which is only internally used by Nuclei to control whether ECC check is enabled or disabled.
  - Add core ``n200e/n202/n202e`` and remove ``n205/n205e/n305/n307/n307fd`` which can be replaced by ``n203/n203e/n300/n300f/n300fd``
  - Prebuilt IAR projects and workbench are updated due to evalsoc support changes for plic and clint interrupt modes.
  - Add ``SYSCLK`` make variable for manually set default SYSTEM_CLOCK macro in evalsoc, it is useful for ``CODESIZE=1`` case
  - Add ``QEMU_MC_EXTOPT`` make variable to pass extra Nuclei Qemu ``-M`` machine options for evalsoc.
  - Add ``QEMU_CPU_EXTOPT`` make variable to pass extra Nuclei Qemu ``-cpu`` cpu options for evalsoc.

V0.6.0
------

This is release version ``0.6.0`` of Nuclei SDK.

.. note::

   - Please use  **Nuclei Studio 2024.06** with this Nuclei SDK 0.6.0.
   - There are many changes in this release, so we decide to name it as 0.6.0, not 0.5.1
   - This version introduced **ThreadX and FreeRTOS-SMP support** for Nuclei RISC-V Processors.
   - This version introduced a ``profiling`` middleware and an example to show code coverage and profiling technology
     using gcov and gprof in **Nuclei Studio 2024.06**.
   - We introduced support for **Nuclei 100 series RISC-V CPU**, but in seperated Nuclei SDK branches called **master_n100** or **develop_n100**, see https://doc.nucleisys.com/nuclei_n100_sdk
   - This version introduced support for gd32vw55x chip and Nuclei DLink Board.
   - Better **Terapines ZCC** toolchain integrated in Nuclei SDK and Nuclei Studio, try ZStudio Lite version here https://www.terapines.com/products/
   - Better **IAR Workbench** support in Nuclei SDK, with Baremetal SMP and FreeRTOS SMP supported.

* Application

  - Add ThreadX RTOS example to show how to use ThreadX in SDK.
  - Add Nuclei 1000 series benchmark flags for benchmark examples.
  - Add ``demo_vnice`` example to show how to use Nuclei Vector NICE feature.
  - Add ``demo_profiling`` example to how to use gprof and gcov in Nuclei Studio.
  - Add ``smphello``, ``demo_cidu`` baremetal SMP examples in IAR workbench.
  - Add FreeRTOS ``smpdemo`` example to show how to use SMP version of FreeRTOS.
  - Optimize and fix ``cpuinfo`` example for better cpu feature dection.
  - Optimize benchmark gcc13 flags to provide better performance.
  - Fix wrong ipc calculating for benchmark examples.
  - Reset mcycle and minstret when read cycle or instret in benchmark examples.
  - Fix dhrystone strcmp_xlcz.S removed by make clean in windows.
  - Update benchmark flags for benchmark examples when compiled with Terapines ZCC Toolchain.
  - Fix ``lowpower`` example no need to use ``newlib_full`` library.

* NMSIS

  - Update many CSR structure defined in ``core_feature_base.h`` such as ``CSR_MCFGINFO_Type``, ``CSR_MDLMCTL_Type`` and ``CSR_MCACHECTL_Type`` etc.
  - Add ``__set_rv_cycle`` and ``__set_rv_instret`` API to set cycle and instret csr registers.
  - Add ``CSR_MTLBCFGINFO_Type`` CSR structure in ``core_feature_base.h``.
  - Fix protection type error in PMP/sPMP API.
  - Fix wrong CLIC_CLICINFO_VER_Msk and CLIC_CLICINFO_NUM_Msk macro value in ``core_feature_eclic.h``
  - Add ``__ROR64`` in ``core_compatiable.h``.
  - Add and update DSP intrinsic APIs in ``core_feature_dsp.h``.
  - Add and update Nuclei customized CSRs in ``riscv_encoding.h``.
  - Sync NMSIS DSP/NN library 1.2.1

* SoC

  - Redesign ``evalsoc`` reference SoC support software for better evalsoc and nuclei cpu support, see :ref:`design_soc_evalsoc_usage`
  - Remove ``-msave-restore`` in npk.yml to fix dhrystone benchmark value is low in Nuclei Studio issue.
  - No need to get system clock using ``get_cpu_freq`` for gd32vf103.
  - In npk.yml, when pass ``-isystem=`` should be changed to ``-isystem =`` as a workaround for Nuclei Studio to pass correct system include header.
  - Update standard c library and arch ext prompt for soc npk.yml for better hints.
  - Add ``gd32vf103c_dlink`` board support for Nuclei DLink development.
  - Fix non-ABS relocation R_RISCV_JAL against symbol '_start' fail for nuclei_llvm toolchain
  - Add Nuclei ``ux1000fd`` support in both NPK and Makefile based Build System.
  - Add support for **gd32vw55x** SoC which is Gigadevice new Nuclei RISC-V N300 Processor based WiFi MCU.
  - Add **SPLITMODE** support for **evalsoc** when evaluate NA class Core.
  - Allow custom linker script if npk variable ``linker_script`` is not empty.
  - Explicit declare asm function in gcc asm code if that part of code is a function, which is required by ``gprof`` plugin in Nuclei Studio.
  - Clear zc bit for non zc elf in mmsic_ctl csr for cases when cpu is not reset but zc bit is set before.
  - Only print CSR value when CSR is present during ``__premain_init`` for **evalsoc**.
  - Fix undefined symbol when link cpp for clang ``__eh_frame_start/__eh_frame_hdr_start/__eh_frame_end/__eh_frame_hdr_end``
  - Add **LDSPEC_EN**, **L2_EN** and **BPU_EN** for evalsoc in Makefile based build system to control
    load speculative, L2 cache and BPU enable or disable, which is only internally used.
  - Move eclic and interrupt and exception initialization from startup asm code into premain c code for evalsoc.
  - Optimize cpu startup when ECLIC not present it will not be initialized, which is helpful for CPU without ECLIC unit.
  - evalsoc ``SystemIRegionInfo`` variable is removed now, if you want to access to the base address of cpu internal device, you can use ``*_BASEADDR``,
    such as ``__CIDU_BASEADDR``.
  - Introduce an IAR startup asm code called ``IAR/startup.S`` for evalsoc to support SMP boot, and for SMP stack setup,
    different IAR linker script is required, see the iar linker script provided in ``smphello`` or ``freertos/smpdemo``.

* Build System

  - Now disassemble elf will show no alias instructions
  - Add ``u600*/u900*/ux1000fd`` into support CORE list
  - Update and optimize toolchain support for Terapines ZCC Toolchain, which can provide better performance
  - In ``Build/toolchain/nuclei_gnu.mk``, ``-mmemcpy-strategy=scalar`` option is replaced by ``-mstringop-strategy=scalar`` in official gcc 14, see
    https://gcc.gnu.org/git/?p=gcc.git;a=commit;h=4ae5a7336ac8e1ba57ee1e885b5b76ed86cdbfd5

* RTOS

  - Bump FreeRTOS Kernel to V11.1.0
  - Bump RTThread Nano to V3.1.5
  - Introduce FreeRTOS SMP support for Nuclei RISC-V CPU
  - Introduce Eclipse ThreadX v6.4.1 Support for Nuclei RISC-V CPU

* Misc

  - Add ``Zc/Zicond`` and ``1000`` series support in SDK CLI script used internally
  - Optimize gitlab ci jobs to speedup job execution time and better merge request pipeline check


V0.5.0
------

This is release version ``0.5.0`` of Nuclei SDK, please use it with `Nuclei Studio 2023.10`_ release.

.. note::

   - This 0.5.0 version is a big change version for Nuclei SDK, it now support `Nuclei Toolchain 2023.10`_,
     which have gnu toolchain and llvm toolchain in it, gcc version increased to gcc 13, and clang version
     used is clang 17. It will no longer support old gcc 10 version, since gcc and clang ``-march`` option
     changed a lot, such as b extension changed to ``_zba_zbb_zbc_zbs``.
   - This version also introduced other compiler support such as terapines zcc and IAR compiler.
     For terapines zcc compiler, please visit https://www.terapines.com/ to contact them for toolchain evaluation, pass ``TOOLCHAIN=terapines`` during make to select terapines zcc compiler.
     For IAR compiler, please visit https://www.iar.com/riscv for IAR workbench evaluation, we provided iar projects to take a try with it.
   - This version introduced libncrt v3.0.0 support, which split libncrt library into three parts, you need to take care when using newer toolchain.
   - This version removed demosoc support, please use evalsoc instead.
   - This version introduced qemu 8.0 support, old qemu will not be supported.
   - This version introduced Nuclei Studio 2023.10 support which introduced llvm toolchain support via npk, so it can only works with 2023.10 or later version.
   - This version required a lot of new npk features introduced in `Nuclei Studio 2023.10`_, so it can only be imported as npk package in `Nuclei Studio 2023.10`_ or later version.

* Application

  - Add cpuinfo case to dump nuclei cpu feature
  - Add stack check demo to demostrate nuclei stack check feature
  - Add support for gcc13/clang17/terapines/iar compiler
  - Fix missing break in ``__set_hpm_event`` function, take care if you are using this API.
  - For different compiler option support, we introduced ``toolchain_$(TOOLCHAIN).mk`` file to place toolchain specified options, see benchmark examples' Makefile
  - Optimize demo_cidu smp case
  - Optimize application code and makefile when port for clang, terapines zcc and iar compiler
  - Change :ref:`develop_buildsystem_var_archext` makefile comment for demo_dsp when using gcc 13
  - Auto choose proper CPU_SERIES and proper optimization flags for benchmark cases
  - Optimize whetstone cost to decrease execution time for better ci testing in qemu and fpga
  - Add Zc and Xxlcz extension optimization for coremark and dhrystone cases
  - Do specical adaption for demo_pmp/demo_spmp for iar compiler which require customized iar linker icf for this cases
  - Optimize benchmark flags when using gcc 13

* NMSIS

  - Add bench reset/sample/stop/stat and get usecyc/sumcyc/lpcnt APIs in NMSIS Core
  - Add more CSRs such as Zc/Stack Check in riscv_encoding.h
  - Rename NMSIS DSP/NN library name to match gcc 13 changes, eg. ``b -> zba_zbb_zbc_zbs``, so the library name changed a lot
  - Add IAR compiler support in NMSIS Core
  - No more bitmanip extension intrinsic header ``<rvintrin.h>`` for gcc13
  - Fix ``__RV_CLAMP`` macro and add ``__MACHINE/SUPERVISOR/USER_INTERRUPT`` macros
  - Add ``__get_hart_index`` and ``SysTimer_GetHartID`` and modify ``__get_hart_id`` API
  - In <Device.h>, we introduced ``__HARTID_OFFSET`` and ``__SYSTIMER_HARTID`` macro to represent timer hart index relation with cpu hartid for AMP SoC
  - Update NMSIS Core/DSP/NN header files to `NMSIS 1.2.0`_
  - Update NMSIS DSP/NN prebuilt library to v1.2.0, and added F16 prebuilt library

* SOC

  - **CAUTION**: Demosoc support is removed since evalsoc is the successor, please use **evalsoc** now.
  - Set **RUNMODE_CCM_EN** macro when **CCM_EN** make variable passed and allow ``__CCM_PRESENT`` overwrite by **RUNMODE_CCM_EN** macro
  - Enable ``__CIDU_PRESENT`` macro passed via compiler option
  - Update cpu startup asm code to fix clang compile issue such as STB_WEAK warning and non-ABS relocation error
  - Update cpu startup asm code to support zcmt jump table
  - Update gnu linker files to support zcmt extension
  - Update gnu linker files to fix 2 byte gap issue, and align section to 8bytes and reorg sections
  - Update openocd configuration files to support openocd new version
  - Make ``metal_tty_putc/getc`` with ``__USED`` attribute to avoid ``-flto`` build and link fail
  - Add startup and exception code and iar linker icf files for IAR compiler support
  - Add new macros ``__HARTID_OFFSET`` and ``__SYSTIMER_HARTID`` in evalsoc.h
  - Add **HARTID_OFFSET** make variable to control hartid offset for evalsoc
  - Boot hartid check no longer only compare lower 8bits for evalsoc
  - Currently IAR compiler support is only for single core support, smp support is not yet ready and need to use in IAR workbench
  - Update Nuclei Studio NPK files to support both gcc and llvm toolchain support, this require `Nuclei Studio 2023.10`_, which is incompatiable with previous IDE version.

* Build System

  - Fix semihost not working when link with semihost library
  - Add support for gcc 13, clang 17, terapines zcc toolchain using :ref:`develop_buildsystem_var_toolchain` make variable, eg. ``TOOLCHAIN=nuclei_gnu`` for gnu gcc toolchain, ``TOOLCHAIN=nuclei_llvm`` for llvm toolchain, ``TOOLCHAIN=terapines`` for terapines zcc toolchain
  - Add support for libncrt v3.0.0, which spilt libncrt into 3 parts, the c library part, fileops part, and heapops part, so :ref:`develop_buildsystem_var_ncrtheap` and :ref:`develop_buildsystem_var_ncrtio` makefile variable are added to support new version of libncrt, about upgrading libncrt, please check :ref:`develop_buildsystem_var_stdclib`
  - To support both gcc, clang, zcc, now we no longer use ``--specs=nano.specs`` like ``--specs=`` gcc only options, since clang don't support it, we directly link the required libraries according to the library type you want to use in Makefile, group all the required libraries using ``--start-group archives --end-group`` of linker option, see https://sourceware.org/binutils/docs/ld/Options.html, but when using Nuclei Studio, the Eclipse CDT based IDE didn't provided a good way to do library group, here is an issue tracking it, see https://github.com/eclipse-embed-cdt/eclipse-plugins/issues/592

    - And also now we defaultly enabled ``-nodefaultlibs`` option to not use any standard system libraries when linking, so we need to specify the system libraries we want to use during linking, which is the best way to support both gcc and clang toolchain.

  - When using libncrt library, this is no need to link with other libgcc library, c library or math library, such as gcc libgcc library(``-lgcc``), newlib c library(``-lc/-lc_nano``) and math library(``-lm``), the c and math features are also provided in libncrt library
  - When using Nuclei Studio with imported Nuclei SDK NPK package, you might meet with undefined reference issue during link
  - The use of :ref:`develop_buildsystem_var_archext` is changed for new toolchain, eg. you can't pass ``ARCH_EXT=bp`` to represent b/p extension, instead you need to pass ``ARCH_EXT=_zba_zbb_zbc_zbs_xxldspn1x``
  - Show CC/CXX/GDB when make showflags
  - Add u900 series cores support
  - No longer support gd32vf103 soc run on qemu
  - Add extra ``-fomit-frame-pointer -fno-shrink-wrap-separate`` options for Zc extension to enable zcmp instruction generation
  - Extra **CPU_SERIES** macro is passed such (200/300/600/900) during compiling for benchmark examples
  - When you want to select different nmsis library arch, please use :ref:`develop_buildsystem_var_nmsis_lib_arch` make variable, see demo_dsp as example

* Tools

  - A lot of changes mainly in nsdk cli configs have been made to remove support of demosoc, and change it to evalsoc
  - A lot of changes mainly in nsdk cli configs have been made to support newer :ref:`develop_buildsystem_var_archext` variable format
  - Add llvm ci related nsdk cli config files
  - Add Zc/Xxlcz fpga benchmark config files
  - Support qemu 8.0 in nsdk cli tools
  - Update configurations due to application adding and updating

* RTOS

  - Add freertos/ucosii/rtthread porting code for IAR compiler
  - Enable vector when startup new task for rtos for possible execute rvv related instruction exception

* Misc

  - Change gitlab ci to use `Nuclei Toolchain 2023.10`_
  - Add IAR workbench workspace and projects for evalsoc, so user can quickly evaluate IAR support in IAR workbench

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
.. _Nuclei Studio 2023.10: https://github.com/Nuclei-Software/nuclei-studio/releases/tag/2023.10
.. _Nuclei Toolchain 2023.10: https://github.com/riscv-mcu/riscv-gnu-toolchain/releases/tag/nuclei-2023.10
.. _NMSIS 1.2.0: https://github.com/Nuclei-Software/NMSIS/releases/tag/1.2.0
