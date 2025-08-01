.. _design_app:

Application
===========

.. _design_app_overview:

Overview
--------

In Nuclei SDK, we just provided applications which can
run in different boards without any changes in code to
demonstrate the baremetal service, freertos service and
ucosii service features.

The provided applications can be divided into three categories:

* Bare-metal applications: Located in ``application/baremetal``

* FreeRTOS applications: Located in ``application/freertos``

* UCOSII applications: Located in ``application/ucosii``

* RTThread applications: Located in ``application/rtthread``

* ThreadX applications: Located in ``application/threadx``

**If you want to find more examples, please visit the following links**:

* Nuclei Board Labs: https://github.com/Nuclei-Software/nuclei-board-labs
* Nuclei Tensorflow Lite Micro AI Demo: https://github.com/Nuclei-Software/npk-tflm
* Nuclei Tinymaix TinyAI Demo: https://github.com/Nuclei-Software/npk-tinymaix
* NMSIS DSP Examples: https://doc.nucleisys.com/nmsis/dsp/get_started.html#how-to-run
* NMSIS NN Examples: https://doc.nucleisys.com/nmsis/nn/get_started.html#how-to-run
* NMSIS Crypto(MbedTLS) Examples: https://github.com/Nuclei-Software/mbedtls/blob/nuclei/v3.3.0/accelerator/README.md

And we can also provide more examples to test cpu features, please contact with our
AE for help.

If you want to develop your own application in Nuclei SDK, please click
:ref:`develop_appdev` to learn more about it.

The following applications are running using RV-STAR board or Nuclei Eval SoC.

.. note::

    * Since 0.7.0 introduced support for CLINT and PLIC interrupt mode, if
      you are working in such interrupt mode or don't have ECLIC module, then
      all RTOSes will not able to run in your environment, due to RTOS port
      require ECLIC interrupt.

    * Most of the application demostrated below using ``SOC=gd32vf103``,
      you can easily change it to other SoC such as evalsoc by change it to
      ``SOC=evalsoc``
    * Some applications may not be able to be run on your SoC using Nuclei CPU
      due to lack of cpu feature required to run on it.
    * Almost all the applications required Nuclei CPU configured with
      ECLIC and System Timer hardware feature.
    * Almost all the application required UART to print message, so you need to
      implement an UART drivers and clib stub functions, if you use
      :ref:`develop_buildsystem_var_semihost` to print message, it is not required.

Bare-metal applications
-----------------------

.. _design_app_helloworld:

helloworld
~~~~~~~~~~

This `helloworld application`_ is used to print hello world, and also
will check this RISC-V CSR **MISA** register value.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the helloworld directory
    cd application/baremetal/helloworld
    # Clean the application first
    make SOC=gd32vf103 clean
    # Build and upload the application
    make SOC=gd32vf103 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 21 2020, 12:24:22
    Download Mode: FLASHXIP
    CPU Frequency 109323529 Hz
    MISA: 0x40901105
    MISA: RV32IMACUX
    0: Hello World From Nuclei RISC-V Processor!
    1: Hello World From Nuclei RISC-V Processor!
    2: Hello World From Nuclei RISC-V Processor!
    3: Hello World From Nuclei RISC-V Processor!
    4: Hello World From Nuclei RISC-V Processor!
    5: Hello World From Nuclei RISC-V Processor!
    6: Hello World From Nuclei RISC-V Processor!
    7: Hello World From Nuclei RISC-V Processor!
    8: Hello World From Nuclei RISC-V Processor!
    9: Hello World From Nuclei RISC-V Processor!
    10: Hello World From Nuclei RISC-V Processor!
    11: Hello World From Nuclei RISC-V Processor!
    12: Hello World From Nuclei RISC-V Processor!
    13: Hello World From Nuclei RISC-V Processor!
    14: Hello World From Nuclei RISC-V Processor!
    15: Hello World From Nuclei RISC-V Processor!
    16: Hello World From Nuclei RISC-V Processor!
    17: Hello World From Nuclei RISC-V Processor!
    18: Hello World From Nuclei RISC-V Processor!
    19: Hello World From Nuclei RISC-V Processor!

.. _design_app_cpuinfo:

cpuinfo
~~~~~~~

This `cpuinfo application`_ is used to print the Nuclei RISC-V CPU information
to help you to know what CPU features are present in this processor.

You can also use openocd to probe the cpu feature, see https://doc.nucleisys.com/nuclei_tools/openocd/intro.html#nuclei-customized-features

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the helloworld directory
    cd application/baremetal/cpuinfo
    # Assume to run on UX900 SMPx8 CPU
    # Clean the application first
    make SOC=evalsoc DOWNLOAD=sram clean
    # Build and upload the application
    make SOC=evalsoc DOWNLOAD=sram upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May 28 2024, 13:36:12
    Download Mode: SRAM
    CPU Frequency 50322800 Hz
    CPU HartID: 0

    -----Nuclei RISC-V CPU Configuration Information-----
            MARCHID: 0x900
            MIMPID: 0x30900
                ISA: RV64 A B C D F I M S U Zc Xxlcz
                MCFG: ECLIC PLIC ICACHE DCACHE SMP ZC_XLCZ_EXT IREGION No-Safety-Mechanism DLEN=VLEN/2
            ICACHE: 64 KB(set=512,way=2,lsize=64,ecc=0)
            DCACHE: 64 KB(set=512,way=2,lsize=64,ecc=0)
                TLB: MainTLB(set=256,way=4,entry=1,ecc=0) ITLB(entry=16) DTLB(entry=16)
            IREGION: 0x18000000 128 MB
                    Unit        Size        Address
                    INFO        64KB        0x18000000
                    DEBUG       64KB        0x18010000
                    ECLIC       64KB        0x18020000
                    TIMER       64KB        0x18030000
                    SMP         64KB        0x18040000
                    CIDU        64KB        0x18050000
                    PLIC        64MB        0x1c000000
            SMP_CFG: CC_PRESENT=1 SMP_NUM=8 IOCP_NUM=0 PMON_NUM=4
            ECLIC: VERSION=0x0 NUM_INTERRUPT=71 CLICINTCTLBITS=3 MTH=0 NLBITS=3
            L2CACHE: 2 MB(set=2048,way=16,lsize=64,ecc=0)
        INFO-Detail:
                    mpasize : 32
    -----End of Nuclei CPU INFO-----

.. _design_app_demo_timer:

demo_timer
~~~~~~~~~~

This `demo_timer application`_ is used to demonstrate how to use
the CORE TIMER API including the Timer Interrupt and Timer Software Interrupt in ECLIC interrupt mode.

* Both interrupts are registered as non-vector interrupt.
* First the timer interrupt will run for 5 times
* Then the software timer interrupt will start to run for 5 times

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_timer directory
    cd application/baremetal/demo_timer
    # Clean the application first
    make SOC=gd32vf103 clean
    # Build and upload the application
    make SOC=gd32vf103 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 21 2020, 12:52:37
    Download Mode: FLASHXIP
    CPU Frequency 108794117 Hz
    init timer and start
    MTimer IRQ handler 1
    MTimer IRQ handler 2
    MTimer IRQ handler 3
    MTimer IRQ handler 4
    MTimer IRQ handler 5
    MTimer SW IRQ handler 1
    MTimer SW IRQ handler 2
    MTimer SW IRQ handler 3
    MTimer SW IRQ handler 4
    MTimer SW IRQ handler 5
    MTimer msip and mtip interrupt test finish and pass

.. _design_app_demo_clint_timer:

demo_clint_timer
~~~~~~~~~~~~~~~~

This `demo_clint_timer application`_ is used to demonstrate how to use
the CORE TIMER API including the Timer Interrupt and Timer Software Interrupt in CLINT interrupt mode.

* Interrupt is set to working in CLINT interrupt mode
* Both interrupts are registered as core interrupt.
* First the timer interrupt will run for 5 times
* Then the software timer interrupt will start to run for 5 times
* **NOTE**: not able to working in qemu, and only works for evalsoc


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_timer directory
    cd application/baremetal/demo_clint_timer
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jul 25 2024, 10:39:39
    Download Mode: ILM
    CPU Frequency 16000614 Hz
    CPU HartID: 0
    init timer and start
    SysTimer IRQ handler 1
    SysTimer IRQ handler 2
    SysTimer IRQ handler 3
    SysTimer IRQ handler 4
    SysTimer IRQ handler 5
    SysTimer SW IRQ handler 1
    SysTimer SW IRQ handler 2
    SysTimer SW IRQ handler 3
    SysTimer SW IRQ handler 4
    SysTimer SW IRQ handler 5
    SysTimer MTIP and MSIP CLINT interrupt test finish and pass

.. _design_app_demo_eclic:

demo_eclic
~~~~~~~~~~

This `demo_eclic application`_ is used to demonstrate how to use
the ECLIC API and Interrupt is working in ECLIC interrupt mode.

.. note::

    In this application's Makefile, we provided comments in Makefile about optimize
    for code size.

    If you want to optimize this application for code size, you can set the ``COMMON_FLAGS``
    variable to the following values, we recommend to use ``-Os -flto``.

    .. list-table:: Code size optimization for demo_eclic on RV-STAR target
       :widths: 60 20 20 20 20
       :header-rows: 1

       * - COMMON_FLAGS
         - text(bytes)
         - data(bytes)
         - bss(bytes)
         - total(bytes)
       * -
         - 13724
         - 112
         - 2266
         - 16102
       * - -flto
         - 13598
         - 112
         - 2266
         - 15976
       * - -Os
         - 9690
         - 112
         - 2264
         - 12066
       * - -Os -flto
         - 9132
         - 112
         - 2264
         - 11508
       * - -Os -msave-restore  -fno-unroll-loops
         - 9714
         - 112
         - 2264
         - 12090
       * - -Os -msave-restore  -fno-unroll-loops -flto
         - 9204
         - 112
         - 2264
         - 11580

* The timer interrupt and timer software interrupt are used
* The timer interrupt is registered as non-vector interrupt
* The timer software interrupt is registered as vector interrupt,
  and we enable its preemptive feature by using ``SAVE_IRQ_CSR_CONTEXT``
  and ``RESTORE_IRQ_CSR_CONTEXT`` in timer software interrupt handler
* The timer interrupt is triggered periodically
* The timer software interrupt is triggered in timer interrupt handler using
  ``SysTimer_SetSWIRQ`` function
* In the application code, there is a macro called ``SWIRQ_INTLEVEL_HIGHER`` to
  control the timer software interrupt working feature:

  - If **SWIRQ_INTLEVEL_HIGHER=1**, the timer software interrupt level is higher than
    timer interrupt level, so when timer software interrupt is triggered, then timer
    software interrupt will be processed immediately, and timer interrupt will be preempted
    by timer software interrupt.

  - If **SWIRQ_INTLEVEL_HIGHER=0**, the timer software interrupt level is lower than
    timer interrupt level, so when timer software interrupt is triggered, then timer
    software interrupt will be processed after timer interrupt, and timer interrupt will
    not be preempted by timer software interrupt.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_eclic directory
    cd application/baremetal/demo_eclic
    # Change macro SWIRQ_INTLEVEL_HIGHER value in demo_eclic.c
    # to see different working mode of this demo
    # Clean the application first
    make SOC=gd32vf103 clean
    # Build and upload the application
    make SOC=gd32vf103 upload

**Expected output(SWIRQ_INTLEVEL_HIGHER=1) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 21 2020, 16:35:58
    Download Mode: FLASHXIP
    CPU Frequency 108794117 Hz
    Initialize timer and start timer interrupt periodically
    -------------------
    [IN TIMER INTERRUPT]timer interrupt hit 0 times
    [IN TIMER INTERRUPT]trigger software interrupt
    [IN TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN SOFTWARE INTERRUPT]software interrupt hit 0 times
    [IN SOFTWARE INTERRUPT]software interrupt end
    [IN TIMER INTERRUPT]timer interrupt end
    -------------------
    [IN TIMER INTERRUPT]timer interrupt hit 1 times
    [IN TIMER INTERRUPT]trigger software interrupt
    [IN TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN SOFTWARE INTERRUPT]software interrupt hit 1 times
    [IN SOFTWARE INTERRUPT]software interrupt end
    [IN TIMER INTERRUPT]timer interrupt end
    -------------------
    [IN TIMER INTERRUPT]timer interrupt hit 2 times
    [IN TIMER INTERRUPT]trigger software interrupt
    [IN TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN SOFTWARE INTERRUPT]software interrupt hit 2 times
    [IN SOFTWARE INTERRUPT]software interrupt end
    [IN TIMER INTERRUPT]timer interrupt end
    -------------------
    [IN TIMER INTERRUPT]timer interrupt hit 3 times
    [IN TIMER INTERRUPT]trigger software interrupt
    [IN TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN SOFTWARE INTERRUPT]software interrupt hit 3 times
    [IN SOFTWARE INTERRUPT]software interrupt end
    [IN TIMER INTERRUPT]timer interrupt end


**Expected output(SWIRQ_INTLEVEL_HIGHER=0) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 21 2020, 16:35:58
    Download Mode: FLASHXIP
    CPU Frequency 108794117 Hz
    Initialize timer and start timer interrupt periodically
    -------------------
    [IN TIMER INTERRUPT]timer interrupt hit 0 times
    [IN TIMER INTERRUPT]trigger software interrupt
    [IN TIMER INTERRUPT]software interrupt will run when timer interrupt finished
    [IN TIMER INTERRUPT]timer interrupt end
    [IN SOFTWARE INTERRUPT]software interrupt hit 0 times
    [IN SOFTWARE INTERRUPT]software interrupt end
    -------------------
    [IN TIMER INTERRUPT]timer interrupt hit 1 times
    [IN TIMER INTERRUPT]trigger software interrupt
    [IN TIMER INTERRUPT]software interrupt will run when timer interrupt finished
    [IN TIMER INTERRUPT]timer interrupt end
    [IN SOFTWARE INTERRUPT]software interrupt hit 1 times
    [IN SOFTWARE INTERRUPT]software interrupt end
    -------------------
    [IN TIMER INTERRUPT]timer interrupt hit 2 times
    [IN TIMER INTERRUPT]trigger software interrupt
    [IN TIMER INTERRUPT]software interrupt will run when timer interrupt finished
    [IN TIMER INTERRUPT]timer interrupt end
    [IN SOFTWARE INTERRUPT]software interrupt hit 2 times
    [IN SOFTWARE INTERRUPT]software interrupt end
    -------------------
    [IN TIMER INTERRUPT]timer interrupt hit 3 times
    [IN TIMER INTERRUPT]trigger software interrupt
    [IN TIMER INTERRUPT]software interrupt will run when timer interrupt finished
    [IN TIMER INTERRUPT]timer interrupt end
    [IN SOFTWARE INTERRUPT]software interrupt hit 3 times
    [IN SOFTWARE INTERRUPT]software interrupt end

.. _design_app_demo_plic:

demo_plic
~~~~~~~~~

This `demo_plic application`_ is used to demonstrate how to use
the PLIC API and Interrupt is working in CLINT/PLIC interrupt mode.

.. note::

    This demo only works on evalsoc, and require PLIC module present.

* This demo will show how to use plic external interrupt
* This demo use uart rx interrupt
* **NOTE**: not able to working in qemu

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_plic directory
    cd application/baremetal/demo_plic
    # For this case, if your bit has PLIC, and you are not using sdk generated by nuclei_gen
    # XLCFG_PLIC=1 will define CFG_HAS_PLIC macro
    make SOC=evalsoc XLCFG_PLIC=1 clean
    # Build and upload the application
    make SOC=evalsoc XLCFG_PLIC=1 upload

.. code-block:: console

    Nuclei SDK Build Time: Jul 23 2024, 17:49:27
    Download Mode: ILM
    CPU Frequency 50000000 Hz
    CPU HartID: 0
    You can press any key now to trigger uart receive interrupt
    Enter uart0 interrupt, you just typed: 1
    Enter uart0 interrupt, you just typed: 2


.. _design_app_demo_dsp:

demo_dsp
~~~~~~~~

This `demo_dsp application`_ is used to demonstrate how to NMSIS-DSP API.

* Mainly show how we can use NMSIS DSP library and header files.
* It mainly demo the ``riscv_conv_xx`` functions and its reference functions
* By default, the application will use prebuilt NMSIS-DSP library match riscv isa arch
  defined by :ref:`develop_buildsystem_var_core` and :ref:`develop_buildsystem_var_archext`

* If your selected **CORE** and **ARCH_EXT** don't have a prebuilt NMSIS DSP library,
  you can use :ref:`develop_buildsystem_var_nmsis_lib_arch` make variable to select
  another most suitable prebuilt NMSIS DSP or NN library.

eg. If you build with ``make CORE=n900f ARCH_EXT=_zca_zcb_zcf_zcmp_zcmt_xxldsp clean all``,
you will get a link error like this ``riscv64-unknown-elf/bin/ld: cannot find -lnmsis_dsp_rv32imaf_zca_zcb_zcf_zcmp_zcmt_xxldsp: No such file or directory``, this is
caused by no prebuilt libraries are built with ``Zc*`` extension. But you can build it
by modify this example's Makefile by adding ``NMSIS_LIB_ARCH := rv32imafc_xxldsp`` newline.

Take care ``Zc*`` is not fully compatiable with ``C`` extension, especially when ``D`` extension
present, see latest RISC-V ISA manual ``riscv-unprivileged.pdf`` which can found in
https://github.com/riscv/riscv-isa-manual/releases .

And if you want to modify and build your own NMSIS DSP and NN library and see other
DSP and NN examples and test cases, you can checkout the following links:

- https://github.com/Nuclei-Software/NMSIS
- https://doc.nucleisys.com/nmsis/dsp/index.html
- https://doc.nucleisys.com/nmsis/nn/index.html

.. note::

    * For other Nuclei Processor Core based SoC, please check whether it has DSP
      feature enabled to decide which kind of **NMSIS-DSP** library to use.
    * Even our NMSIS-DSP library with DSP disabled are also optimized, so it can
      also provide good performance in some functions.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_dsp directory
    cd application/baremetal/demo_dsp
    # Clean the application first
    make SOC=gd32vf103 clean
    # Build and upload the application
    make SOC=gd32vf103 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun 18 2020, 17:43:31
    Download Mode: FLASHXIP
    CPU Frequency 108270000 Hz
    CSV, riscv_conv_q31, 1225418
    CSV, ref_conv_q31, 2666240
    SUCCESS, riscv_conv_q31
    CSV, riscv_conv_q15, 289940
    CSV, ref_conv_q15, 311158
    SUCCESS, riscv_conv_q15
    CSV, riscv_conv_q7, 463
    CSV, ref_conv_q7, 846
    SUCCESS, riscv_conv_q7
    CSV, riscv_conv_fast_q15, 106293
    CSV, ref_conv_fast_q15, 247938
    SUCCESS, riscv_conv_fast_q15
    CSV, riscv_conv_fast_q31, 490539
    CSV, ref_conv_fast_q31, 2215917
    SUCCESS, riscv_conv_fast_q31
    CSV, riscv_conv_opt_q15, 217250
    CSV, ref_conv_opt_q15, 311162
    SUCCESS, riscv_conv_opt_q15
    CSV, riscv_conv_opt_q7, 714
    CSV, ref_conv_opt_q7, 842
    SUCCESS, riscv_conv_opt_q7
    CSV, riscv_conv_fast_opt_q15, 137252
    CSV, ref_conv_fast_opt_q15, 249958
    SUCCESS, riscv_conv_fast_opt_q15
    all test are passed. Well done!

.. _design_app_lowpower:

lowpower
~~~~~~~~

This `lowpower application`_ is used to demonstrate how to use low-power feature of RISC-V
processor.

Timer interrupt is setup before enter to wfi mode, and global interrupt will be disabled,
so interrupt handler will not be entered, and will directly resume to next pc of wfi.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # Assume your processor has enabled low-power feature
    # cd to the low-power directory
    cd application/baremetal/lowpower
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun  9 2022, 11:23:14
    Download Mode: ILM
    CPU Frequency 15996354 Hz
    CSV, WFI Start/End, 178264/178289
    CSV, WFI Cost, 25

.. _design_app_smphello:

smphello
~~~~~~~~

This `smphello application`_ is used to demonstrate how to use baremetal SMP feature.

This demo requests the SMP cores share the same RAM and ROM, for example, in current
evalsoc system, ilm/dlm are private resource for cpu, only the DDR/SRAM memory are shared
resource for all the cpu.

And `RVA` atomic extension is required to run this application, this extension is used
to do spinlock in this example.

.. note::

    * It doesn't work with gd32vf103 processor.
    * **MUST** Need to enable I/D Cache in <Device.h> if I/D Cache present in CPU.
    * It needs at least a 2-Core SMP CPU

* Each hart must wait until all the harts stop printing(or just stay in ``while (1);`` after its job has finished),
  because the ``_postmain_fini`` will print some dummy '\0', which has no lock-protecting to UART causing ``corrupted-printing``

* ``spinlock lock`` should be ``volatile``, or else the compiler maybe optimize out the ``spinlock_unlock`` if
  more than one pair of ``spinlock_lock`` ``spinlock_unlock`` used in one function/branch, causing the lock unreleased

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # Use Nuclei UX900 SMP 2 Core RISC-V processor as example
    # application needs to run in ddr memory not in ilm memory
    # cd to the smphello directory
    cd application/baremetal/smphello
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval SMP=2 CORE=ux900 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval SMP=2 CORE=ux900 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May 30 2022, 15:38:00
    Download Mode: SRAM
    CPU Frequency 15998648 Hz
    Hello world from hart 0
    Hello world from hart 1
    All harts boot successfully!

.. _design_app_demo_nice:

demo_nice
~~~~~~~~~

.. note::

    * It doesn't work with gd32vf103 processor.
    * Need nice feature enabled, and Nuclei NICE hardware demo integrated such as evalsoc

This `demo_nice application`_ is used to demonstrate how to Nuclei NICE feature.

**NICE** is short for Nuclei Instruction Co-unit Extension, which is used to
support extensive customization and specialization.

**NICE** allows customers to create user-defined instructions, enabling the
integrations of custom hardware co-units that improve domain-specific
performance while reducing power consumption.

For more about **NICE** feature, please click `Nuclei User Extended Introduction`_.

* Mainly show how to use NICE intrinsic function with compiler.
* It only works with Nuclei RISC-V Processor with the hardware NICE demo integrated.


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # Use Nuclei UX900 RISC-V processor as example, hardware NICE demo integrated
    # cd to the demo_dsp directory
    cd application/baremetal/demo_nice
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=ux900 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=ux900 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May 28 2024, 13:32:18
    Download Mode: ILM
    CPU Frequency 49999631 Hz
    CPU HartID: 0

    Nuclei Nice Acceleration Demonstration
    Warning: This demo required CPU to implement Nuclei provided NICE Demo instructions.
            Otherwise this example will trap to cpu core exception!

    1. Print input matrix array
    the element of array is :
            10      30      90
            20      40      80
            30      90      120

    2. Do reference matrix column sum and row sum
    3. Do nice matrix column sum and row sum
    4. Compare reference and nice result
    5) Reference result:
    the sum of each row is :
                    130     140     240
    the sum of each col is :
                    60      160     290
    6) Nice result:
    the sum of each row is :
                    130     140     240
    the sum of each col is :
                    60      160     290
    7) Compare reference vs nice: PASS
    8. Performance summary
            normal:
                instret: 502, cycle: 502
            nice  :
                instret: 177, cycle: 177


.. _design_app_demo_vnice:

demo_vnice
~~~~~~~~~~

.. note::

    * It only works with Nuclei EvalSoC with Vector NICE demo instructions enabled.
    * Need vector nice feature enabled, and Nuclei NICE hardware demo integrated such as evalsoc

This `demo_vnice application`_ is used to demonstrate how to Nuclei Vector NICE feature.

**NICE** is short for Nuclei Instruction Co-unit Extension, which is used to
support extensive customization and specialization.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # Use Nuclei UX900 + Vector Nice RISC-V processor as example, hardware NICE demo integrated
    # cd to the demo_dsp directory
    cd application/baremetal/demo_vnice
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May 28 2024, 13:31:08
    Download Mode: ILM
    CPU Frequency 1000000716 Hz
    CPU HartID: 0
    1. Set array_normal_in1 array_normal_in1 array_vnice_in1 array_vnice_in2
    2. Do reference vector complex mul, store, load
    3. Do vector nice complex mul, store, load
    4. Compare reference and vnice result
    PASS
    5. Performance summary
            normal:
                instret: 22546, cycle: 22546
            vnice  :
                instret: 1010, cycle: 1010


.. _design_app_coremark:

coremark
~~~~~~~~

This `coremark benchmark application`_ is used to run EEMBC CoreMark Software.

EEMBC CoreMark Software is a product of EEMBC and is provided under the terms of the
CoreMark License that is distributed with the official EEMBC COREMARK Software release.
If you received this EEMBC CoreMark Software without the accompanying CoreMark License,
you must discontinue use and download the official release from www.coremark.org.

In Nuclei SDK, we provided code and Makefile for this ``coremark`` application.
You can also optimize the ``COMMON_FLAGS`` defined in coremark application Makefile
to get different score number.

* By default, this application runs for 800 iterations, you can also change this in Makefile.
  e.g. Change this ``-DITERATIONS=800`` to value such as ``-DITERATIONS=5000``
* macro **PERFORMANCE_RUN=1** is defined
* **STDCLIB ?= newlib_small** is added in its Makefile to enable float value print
* For different Nuclei CPU series, the benchmark options are different, currently
  you can pass ``CPU_SERIES=900`` to select benchmark options for 900 series, otherwise
  the benchmark options for 200/300/600/900 will be selected which is also the default value.

.. note::

   * Since for each SoC platforms, the CPU frequency is different, so user need to change
     the ``ITERATIONS`` defined in Makefile to proper value to let the coremark run at least
     10 seconds
   * For example, for the ``gd32vf103`` based boards supported in Nuclei SDK, we suggest
     to change ``-DITERATIONS=800`` to ``-DITERATIONS=5000``

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the coremark directory
    cd application/baremetal/benchmark/coremark
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd ARCH_EXT=_zba_zbb_zbc_zbs_zicond clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd ARCH_EXT=_zba_zbb_zbc_zbs_zicond upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May  6 2025, 16:33:08
    Download Mode: ILM
    CPU Frequency 16003563 Hz
    CPU HartID: 0
    Start to run coremark for 800 iterations
    2K performance run parameters for coremark.
    CoreMark Size    : 666
    Total ticks      : 207671961
    Total time (secs): 12.976789
    Iterations/Sec   : 61.648533
    Iterations       : 800
    Compiler version : GCC14.2.1 20240816
    Compiler flags   : -Ofast -fno-code-hoisting -fno-common -finline-functions -falign-functions=6 -falign-jumps=6 -falign-loops=4 -finline-limit=2001
    Memory location  : STACK
    seedcrc          : 0xe9f5
    [0]crclist       : 0xe714
    [0]crcmatrix     : 0x1fd7
    [0]crcstate      : 0x8e3a
    [0]crcfinal      : 0xcc42
    Correct operation validated. See readme.txt for run and reporting rules.



    CoreMark 1.0 : 61.648533 / GCC14.2.1 20240816 -Ofast -fno-code-hoisting -fno-common -finline-functions -falign-functions=6 -falign-jumps=6 -faligns

         (Iterations is: 800
         (total_ticks is: 207671961
     (*) Assume the core running at 1 MHz
         So the CoreMark/MHz can be calculated by:
         (Iterations*1000000/total_ticks) = 3.852229 CoreMark/MHz


    CSV, Benchmark, Iterations, Cycles, CoreMark/MHz
    CSV, CoreMark, 800, 207671961, 3.852
    IPC = Instret/Cycle = 184031355/207671961 = 0.886

.. _design_app_dhrystone:

dhrystone
~~~~~~~~~

This `dhrystone benchmark application`_ is used to run DHRYSTONE Benchmark Software, whose version is v2.1.

The Dhrystone benchmark program has become a popular benchmark for CPU/compiler performance measurement,
in particular in the area of minicomputers, workstations, PC's and microprocesors.

* It apparently satisfies a need for an easy-to-use integer benchmark;
* it gives a first performance indication which is more meaningful than MIPS numbers which,
  in their literal meaning (million instructions per second), cannot be used across different
  instruction sets (e.g. RISC vs. CISC).
* With the increasing use of the benchmark, it seems necessary to reconsider the benchmark and
  to check whether it can still fulfill this function.

In Nuclei SDK, we provided code and Makefile for this ``dhrystone`` application.
You can also optimize the ``COMMON_FLAGS`` defined in dhrystone application Makefile
to get different score number.

* **STDCLIB ?= newlib_small** is added in its Makefile to enable float value print
* You can change ``Number_Of_Runs`` in ``dhry_1.c`` to increate or decrease
  number of iterations

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the dhrystone directory
    cd application/baremetal/benchmark/dhrystone
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd ARCH_EXT=_zba_zbb_zbc_zbs clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd ARCH_EXT=_zba_zbb_zbc_zbs upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May  6 2025, 16:29:34
    Download Mode: ILM
    CPU Frequency 16004874 Hz
    CPU HartID: 0

    Dhrystone Benchmark, Version 2.1 (Language: C)

    Program compiled without 'register' attribute

    Please give the number of runs through the benchmark:
    Execution starts, 500000 runs through Dhrystone
    Execution ends

    Final values of the variables used in the benchmark:

    Int_Glob:            5
            should be:   5
    Bool_Glob:           1
            should be:   1
    Ch_1_Glob:           A
            should be:   A
    Ch_2_Glob:           B
            should be:   B
    Arr_1_Glob[8]:       7
            should be:   7
    Arr_2_Glob[8][7]:    500010
            should be:   Number_Of_Runs + 10
    Ptr_Glob->
      Ptr_Comp:          -1879032528
            should be:   (implementation-dependent)
      Discr:             0
            should be:   0
      Enum_Comp:         2
            should be:   2
      Int_Comp:          17
            should be:   17
      Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
            should be:   DHRYSTONE PROGRAM, SOME STRING
    Next_Ptr_Glob->
      Ptr_Comp:          -1879032528
            should be:   (implementation-dependent), same as above
      Discr:             0
            should be:   0
      Enum_Comp:         1
            should be:   1
      Int_Comp:          18
            should be:   18
      Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
            should be:   DHRYSTONE PROGRAM, SOME STRING
    Int_1_Loc:           5
            should be:   5
    Int_2_Loc:           13
            should be:   13
    Int_3_Loc:           7
            should be:   7
    Enum_Loc:            1
            should be:   1
    Str_1_Loc:           DHRYSTONE PROGRAM, 1'ST STRING
            should be:   DHRYSTONE PROGRAM, 1'ST STRING
    Str_2_Loc:           DHRYSTONE PROGRAM, 2'ND STRING
            should be:   DHRYSTONE PROGRAM, 2'ND STRING

     (*) User_Cycle for total run through Dhrystone with loops 500000:
    151000097
           So the DMIPS/MHz can be calculated by:
           1000000/(User_Cycle/Number_Of_Runs)/1757 = 1.884608 DMIPS/MHz


    CSV, Benchmark, Iterations, Cycles, DMIPS/MHz
    CSV, Dhrystone, 500000, 151000097, 1.884
    IPC = Instret/Cycle = 145000036/151000097 = 0.960

.. _design_app_dhrystone_v2.2:

dhrystone_v2.2
~~~~~~~~~~~~~~

This `dhrystone_v2.2 benchmark application`_ is used to run DHRYSTONE Benchmark Software, whose version is v2.2.

In Nuclei SDK, we provided code and Makefile for this ``dhrystone`` application.
You can also optimize the ``COMMON_FLAGS`` defined in dhrystone application Makefile
to get different score number.

* **STDCLIB ?= newlib_small** is added in its Makefile to enable float value print
* ``Number_Of_Runs`` will increase itself if running time is too small

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the dhrystone_v2.2 directory
    cd application/baremetal/benchmark/dhrystone_v2.2
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd ARCH_EXT=_zba_zbb_zbc_zbs clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd ARCH_EXT=_zba_zbb_zbc_zbs upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May  6 2025, 16:22:34
    Download Mode: ILM
    CPU Frequency 16006184 Hz
    CPU HartID: 0

    Dhrystone Benchmark, Version C, Version 2.2
    Program compiled without 'register' attribute
    Using time(), HZ=1

    Trying 50000 runs through Dhrystone:
    Measured time too small to obtain meaningful results

    Trying 500000 runs through Dhrystone:
    Final values of the variables used in the benchmark:

    Int_Glob:            5
            should be:   5
    Bool_Glob:           1
            should be:   1
    Ch_1_Glob:           A
            should be:   A
    Ch_2_Glob:           B
            should be:   B
    Arr_1_Glob[8]:       7
            should be:   7
    Arr_2_Glob[8][7]:    550010
            should be:   Number_Of_Runs + 10
    Ptr_Glob->
      Ptr_Comp:          -1879032368
            should be:   (implementation-dependent)
      Discr:             0
            should be:   0
      Enum_Comp:         2
            should be:   2
      Int_Comp:          17
            should be:   17
      Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
            should be:   DHRYSTONE PROGRAM, SOME STRING
    Next_Ptr_Glob->
      Ptr_Comp:          -1879032368
            should be:   (implementation-dependent), same as above
      Discr:             0
            should be:   0
      Enum_Comp:         1
            should be:   1
      Int_Comp:          18
            should be:   18
      Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
            should be:   DHRYSTONE PROGRAM, SOME STRING
    Int_1_Loc:           5
            should be:   5
    Int_2_Loc:           13
            should be:   13
    Int_3_Loc:           7
            should be:   7
    Enum_Loc:            1
            should be:   1
    Str_1_Loc:           DHRYSTONE PROGRAM, 1'ST STRING
            should be:   DHRYSTONE PROGRAM, 1'ST STRING
    Str_2_Loc:           DHRYSTONE PROGRAM, 2'ND STRING
            should be:   DHRYSTONE PROGRAM, 2'ND STRING

    Microseconds for one run through Dhrystone:       14.0
    Dhrystones per Second:                           71429

     (*) User_Cycle for total run through Dhrystone with loops 500000:
    128000082
           So the DMIPS/MHz can be calculated by:
           1000000/(User_Cycle/Number_Of_Runs)/1757 = 2.223248 DMIPS/MHz


    CSV, Benchmark, Iterations, Cycles, DMIPS/MHz
    CSV, Dhrystone_v2.2, 500000, 128000082, 2.223
    IPC = Instret/Cycle = 117500053/128000082 = 0.917


.. _design_app_whetstone:

whetstone
~~~~~~~~~

This `whetstone benchmark application`_ is used to run C/C++ Whetstone Benchmark Software
(Single or Double Precision), whose version is roy@roylongbottom.org.uk, 6 November 1996.

The Fortran Whetstone programs were the first general purpose benchmarks that set industry
standards of computer system performance. Whetstone programs also addressed the question
of the efficiency of different programming languages, an important issue not covered by
more contemporary standard benchmarks.

In Nuclei SDK, we provided code and Makefile for this ``whetstone`` application.
You can also optimize the ``COMMON_FLAGS`` defined in whetstone application Makefile
to get different score number.

* **STDCLIB ?= newlib_small** is added in its Makefile to enable float value print
* Extra **LDFLAGS := -lm** is added in its Makefile to include the math library


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the whetstone directory
    cd application/baremetal/benchmark/whetstone
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May  6 2025, 16:31:23
    Download Mode: ILM
    CPU Frequency 15984885 Hz
    CPU HartID: 0

    ##########################################
    Double Precision C Whetstone Benchmark Opt 3 32 Bit
    Calibrate
           0.37 Seconds          1   Passes (x 100)

    Use 8  passes (x 100)


              Double Precision C/C++ Whetstone Benchmark
    Loop content                  Result              MFLOPS      MOPS   Seconds

    N1 floating point -1.12441415430187974        12.486              0.012
    N2 floating point -1.12239951147853168        16.874              0.064
    N3 if then else    1.00000000000000000                   0.000    0.000
    N4 fixed point    12.00000000000000000                 120.022    0.021
    N5 sin,cos etc.    0.49907428465337039                   0.402    1.654
    N6 floating point  0.99999988495142078         9.600              0.449
    N7 assignments     3.00000000000000000                  71.982    0.021
    N8 exp,sqrt etc.   0.75095530233199781                   0.423    0.704

    MWIPS                                             27.355              2.925


    MWIPS/MHz                                          1.711              2.925


    CSV, Benchmark, MWIPS/MHz
    CSV, Whetstone, 1.711
    IPC = Instret/Cycle = 35858111/49362436 = 0.726


.. _design_app_whetstone_v1.2:

whetstone_v1.2
~~~~~~~~~~~~~~

This `whetstone_v1.2 benchmark application`_ is used to run C Converted Whetstone
Single or Double Precision Benchmark Version 1.2 22 March 1998, which has different
algorithm to this version `whetstone benchmark application`_ (they are incomparable).

In Nuclei SDK, we provided code and Makefile for this ``whetstone_v1.2`` application.
You can also optimize the ``COMMON_FLAGS`` defined in whetstone application Makefile
to get different score number.

* **STDCLIB ?= newlib_small** is added in its Makefile to enable float value print
* Extra **LDFLAGS := -lm** is added in its Makefile to include the math library


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the whetstone_v1.2 directory
    cd application/baremetal/benchmark/whetstone_v1.2
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300fd upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May  6 2025, 16:11:48
    Download Mode: ILM
    CPU Frequency 15984885 Hz
    CPU HartID: 0

    ##########################################
    Single Precision C Whetstone Benchmark Version 1.2      22 March 1998
    Nuclei SDK Build Time: May  6 2025, 16:13:56
    Download Mode: ILM
    CPU Frequency 16004874 Hz
    CPU HartID: 0

    ##########################################
    Double Precision C Whetstone Benchmark Version 1.2      22 March 1998

    Loops: 50000, Iterations: 1, Duration: 70 sec.
    C Converted Double Precision Whetstones: 71.4 MIPS

    CSV, Benchmark, MWIPS/MHz
    CSV, Whetstone_v1.2, 4.462
    IPC = Instret/Cycle = 704074177/1133874283 = 0.620


.. _design_app_demo_smode_eclic:

demo_smode_eclic
~~~~~~~~~~~~~~~~

This `demo_smode_eclic application`_ is used to demostrate how to use
the ECLIC API and Interrupt in supervisor mode with TEE.

.. note::

    * It doesn't work with gd32vf103 processor.
    * It needs Nuclei CPU configured with TEE feature and S-Mode ECLIC
    * In this application's Makefile, we provided comments in Makefile about optimization
      for code size, please refer to chapter :ref:`design_app_demo_eclic` for details.
    * Need to enable TEE in <Device.h> if TEE present in CPU.

* The timer interrupt and timer software interrupt are used
* The timer interrupt is registered as non-vector interrupt
* The timer software interrupt is registered as vector interrupt,
  and we enable its preemptive feature by using ``SAVE_IRQ_CSR_CONTEXT_S``
  and ``RESTORE_IRQ_CSR_CONTEXT_S`` in timer software interrupt handler
* The timer interrupt is triggered periodically
* The timer software interrupt is triggered in timer interrupt handler using
  ``SysTimer_SetHartSWIRQ`` function
* Interrupts occur in supervisor mode to which it drops from machine mode, and you can
  observe the difference from :ref:`design_app_demo_eclic` by console output
* In the application code, there is a macro called ``SWIRQ_INTLEVEL_HIGHER`` to
  control the timer software interrupt working feature:

  - If **SWIRQ_INTLEVEL_HIGHER=1**, the timer software interrupt level is higher than
    timer interrupt level, so when timer software interrupt is triggered, then timer
    software interrupt will be processed immediately, and timer interrupt will be preempted
    by timer software interrupt.

  - If **SWIRQ_INTLEVEL_HIGHER=0**, the timer software interrupt level is lower than
    timer interrupt level, so when timer software interrupt is triggered, then timer
    software interrupt will be processed after timer interrupt, and timer interrupt will
    not be preempted by timer software interrupt.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_smode_eclic directory
    cd application/baremetal/demo_smode_eclic
    # MUST: Your CPU configuration must has TEE configured
    # Since Nuclei SDK 0.7.0, if you are sure CFG_HAS_TEE is not defined in cpufeature.h, but you have TEE
    # you can pass extra make variable XLCFG_TEE=1 during make command to tell sdk
    # the TEE present, it will define CFG_HAS_TEE
    # Change macro SWIRQ_INTLEVEL_HIGHER value in demo_smode_eclic.c
    # to see different working mode of this demo
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 upload

**Expected output(SWIRQ_INTLEVEL_HIGHER=1) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Aug  5 2022, 15:05:52
    Download Mode: ILM
    CPU Frequency 15989145 Hz
    Current sp is 0x9000ffa0, so it is in Machine Mode!
    Drop to S-Mode now
    [IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!
    Current sp is 0x90000f40, so it is in Supervisor Mode!
    Initialize timer and start timer interrupt periodically
    Current sp is 0x90000d80, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 0 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 0 times
    Current sp is 0x90000d10, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    Current sp is 0x90000d80, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 1 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 1 times
    Current sp is 0x90000d10, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    Current sp is 0x90000d80, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 2 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 2 times
    Current sp is 0x90000d10, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    Current sp is 0x90000d80, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 3 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 3 times
    Current sp is 0x90000d10, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    [IN S-MODE TIMER INTERRUPT]timer interrupt end


**Expected output(SWIRQ_INTLEVEL_HIGHER=0) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Aug  5 2022, 15:09:46
    Download Mode: ILM
    CPU Frequency 15989145 Hz
    Current sp is 0x9000ffa0, so it is in Machine Mode!
    Drop to S-Mode now
    [IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!
    Current sp is 0x90000f50, so it is in Supervisor Mode!
    Initialize timer and start timer interrupt periodically
    Current sp is 0x90000d90, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 0 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run when timer interrupt finished
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 0 times
    Current sp is 0x90000ee0, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    Current sp is 0x90000d90, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 1 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run when timer interrupt finished
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 1 times
    Current sp is 0x90000ee0, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    Current sp is 0x90000d90, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 2 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run when timer interrupt finished
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 2 times
    Current sp is 0x90000ee0, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    Current sp is 0x90000d90, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 3 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run when timer interrupt finished
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 3 times
    Current sp is 0x90000ee0, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end

.. _design_app_demo_smode_plic:

demo_smode_plic
~~~~~~~~~~~~~~~

This `demo_smode_plic application`_ is a bare metal program demonstrating the
PLIC (Platform Level Interrupt Controller) functionality in RISC-V processor's
S-Mode (Supervisor Mode). The program shows how to switch from M-Mode to S-Mode
and handle UART interrupts in S-Mode.

.. note::

    * Ensure hardware supports required processor features
    * It needs Nuclei CPU configured with PLIC, S-Mode and PMP
    * Proper definitions in <Device>.h
    * Need to enable PLIC in <Device.h> if PLIC present in CPU

      - ``__PMP_PRESENT=1`` and ``__PLIC_PRESENT=1``

This demo will demostrate the following features:

- Demonstrates M-Mode to S-Mode transition
- Configures PMP to allow S-Mode access to all address spaces
- Registers and handles UART interrupts in S-Mode
- Supports UART receive interrupt handling

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_smode_plic directory
    cd application/baremetal/demo_smode_plic
    # MUST: Your CPU configuration must has PLIC/PMP/SMode configured
    # Since Nuclei SDK 0.7.0, if you are sure CFG_HAS_PLIC is not defined in cpufeature.h, but you have PLIC
    # you can pass extra make variable XLCFG_PLIC=1 during make command to tell sdk
    # the PLIC present, it will define CFG_HAS_PLIC
    # Clean the application first
    make SOC=evalsoc CORE=n900 clean
    # Build and upload the application
    make SOC=evalsoc CORE=n900 upload

.. code-block:: console

    Nuclei SDK Build Time: Apr 28 2025, 15:06:30
    Download Mode: ILM
    CPU Frequency 50002329 Hz
    CPU HartID: 0
    Current sp is 0x9000ff80, so it is in Machine Mode!
    Drop to S-Mode now
    [IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!
    Current sp is 0x900010c0, so it is in Supervisor Mode!
    You can press any key now to trigger uart receive interrupt
    Enter uart0 interrupt, you just typed: 1
    Enter uart0 interrupt, you just typed: 2


.. _design_app_demo_sstc:

demo_sstc
~~~~~~~~~

This `demo_sstc application`_ is used to demostrate how to use
the ECLIC API and Interrupt in supervisor mode with TEE and SSTC.

This demo is similar with :ref:`design_app_demo_smode_eclic`

.. note::

    * It doesn't work with gd32vf103 processor.
    * It needs Nuclei CPU configured with TEE feature and S-Mode ECLIC and SSTC feature

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_sstc directory
    cd application/baremetal/demo_sstc
    # MUST: Your CPU configuration must has TEE and SSTC configured
    # Assume you are using n300
    # Clean the application first
    make SOC=evalsoc CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc CORE=n300 upload

.. code-block:: console

    Nuclei SDK Build Time: Feb 21 2025, 11:12:45
    Download Mode: ILM
    CPU Frequency 15987179 Hz
    CPU HartID: 0
    Current sp is 0x9000ff70, so it is in Machine Mode!
    Drop to S-Mode now
    [IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!
    Current sp is 0x90001040, so it is in Supervisor Mode!
    Initialize timer and start timer interrupt periodically
    Current sp is 0x90000f50, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 0 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 0 times
    Current sp is 0x90000e10, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    Current sp is 0x90000f50, so it is in Supervisor Mode!
    -------------------
    [IN S-MODE TIMER INTERRUPT]timer interrupt hit 1 times
    [IN S-MODE TIMER INTERRUPT]trigger software interrupt
    [IN S-MODE TIMER INTERRUPT]software interrupt will run during timer interrupt
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt hit 1 times
    Current sp is 0x90000e10, so it is in Supervisor Mode!
    [IN S-MODE SOFTWARE INTERRUPT]software interrupt end
    [IN S-MODE TIMER INTERRUPT]timer interrupt end
    Current sp is 0x90000f50, so it is in Supervisor Mode!

.. _design_app_demo_spmp:

demo_spmp
~~~~~~~~~

This `demo_spmp application`_ is removed from ``0.8.0`` release since the ``sPMP``
hardware feature is upgraded to ``SMPU`` in nowadays Nuclei RISC-V CPU, please
refer to :ref:`design_app_demo_smpu`.

.. _design_app_demo_smpu:

demo_smpu
~~~~~~~~~

``SMPU`` is upgraded from ``sPMP`` to enable S-mode OS to limit the physical addresses accessible by
U-mode software on a hart. This `demo_smpu application`_ is used to demonstrate how to grant
physical memory privileges(read, write, execute) on each physical memory region by supervisor-mode control CSRs.

.. note::

    * It doesn't work with gd32vf103 processor.
    * It needs Nuclei CPU configured with TEE, PMP, SMPU feature
    * Need to enable PMP in <Device.h> if PMP present in CPU.
    * Need to enable TEE in <Device.h> if TEE present in CPU.
    * Need to enable SMPU in <Device.h> if smpu present in CPU.

* The `demo_smpu application`_ has many common design with `demo_spmp application`_, and you should first
  pay attention to Encoding of Permissions and Context Switching Optimization when changed to smpu
* Unlike sPMP, ``__set_SMPUSWITCHx`` should be called to activate the entries
* ``smpu_violation_fault_handler`` is registered, which is to execute when smpu violation
  exception occurs
* The SMPU is checked before the PMA checks and PMP checks
* There're three config structures, ``pmp_config`` inits that M-mode grants full permission
  of the whole address range on S and U mode; ``smpu_config_x`` sets protected executable
  address range as 2^12 bytes; ``smpu_config_rw`` sets protected data range as 2^12 bytes,
  and you can change the ``protection``, ``order``, ``base_addr`` according to your memory assignments
* SMPU has three kinds of rules: U-mode-only, S-mode-only, and Shared-Region rules. The S bit marks a rule
  as S-mode-only when set and U-mode-only when unset
* ``protection`` of smpu_config_x and smpu_config_rw should be assigned according to ``2.4. Encoding of Permissions``
  of Ssmpu spec
* Exception delegation from default M mode to S mode is also provided in this demo, when
  it violates smpu check. When exception occurs, the print info including ``scause``, ``sepc``
  can be observed by serial console, which explains the exception cause of smpu permission
  violation, and shows which asm instruction triggers the violation
* In the application code, there is a macro called ``TRIGGER_SMPU_VIOLATION_MODE`` to control the
  smpu working feature:

  - If **TRIGGER_SMPU_VIOLATION_MODE=INSTRUCTION_SMPU_EXCEPTION**, the unallowed memory is to
    execute, which triggers ``Instruction SMPU fault``, whose scause.EXCCODE = 12

  - If **TRIGGER_SMPU_VIOLATION_MODE=LOAD_SMPU_EXCEPTION**, the unallowed memory is to read,
    which triggers ``Load SMPU fault``, whose scause.EXCODE = 13

  - If **TRIGGER_SMPU_VIOLATION_MODE=STORE_SMPU_EXCEPTION**, the unallowed memory is to write,
    which triggers ``Store/AMO SMPU fault``, whose scause.EXCODE = 15

  - If **TRIGGER_SMPU_VIOLATION_MODE=EXECUTE_SHARED_DATA_REGION_EXCEPTION**, the shared R/W data region
    is to execute, which triggers ``Instruction SMPU fault``

  - If **TRIGGER_SMPU_VIOLATION_MODE=WRITE_READONLY_SHARED_DATA_EXCEPTION**, the shared Read-only data region
    is to write, which triggers ``Store/AMO SMPU fault``

  - If **TRIGGER_SMPU_VIOLATION_MODE=SHARE_CODE_DATA_REGION**, the shared code region is to execute, and the shared
    R/W data region is to read and write, both of which is allowed

  - If **TRIGGER_SMPU_VIOLATION_MODE=RUN_WITH_ENTRY_INACTIVE**, the code region and data reigon is set to inaccessible,
    but disable corresponpding entries, so the rules doesn't take effect and execution and read/write succeed

**How to run this application:**

.. code-block::shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_smpu directory
    cd application/baremetal/demo_smpu
    # MUST: Your CPU configuration must has TEE configured
    # Since Nuclei SDK 0.7.0, if you are sure CFG_HAS_SMPU is not defined in cpufeature.h, but you have SMPU
    # you can pass extra make variable XLCFG_SMPU=1 during make command to tell sdk
    # the SMPU present, it will define CFG_HAS_SMPU
    # Change macro TRIGGER_SMPU_VIOLATION_MODE value in demo_smpu.c
    # to see different working mode of this demo
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 upload


**Expected output(TRIGGER_SMPU_VIOLATION_MODE=INSTRUCTION_SMPU_EXCEPTION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun 18 2024, 18:36:40
    Download Mode: ILM
    CPU Frequency 16058613 Hz
    CPU HartID: 0
    ------smpu demo with trigger condition 0------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x0, order_out: 32
    Get smpu entry: index 0, prot_out: 0x9b, addr_out: 0x80004000, order_out: 12
    Get smpu entry: index 1, prot_out: 0x9b, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address 0x0x80004000
    Instruction SMPU fault occurs, cause: 0x1000000c, epc: 0x80004000

**Expected output(TRIGGER_SMPU_VIOLATION_MODE=LOAD_SMPU_EXCEPTION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun 18 2024, 18:39:13
    Download Mode: ILM
    CPU Frequency 16068116 Hz
    CPU HartID: 0
    ------smpu demo with trigger condition 1------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x0, order_out: 32
    Get smpu entry: index 0, prot_out: 0x9c, addr_out: 0x80004000, order_out: 12
    Get smpu entry: index 1, prot_out: 0x9c, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address 0x0x80004000
    ----protected_execute succeed!----
    Attempting to read protected_data[0] at 0x90000000
    Load SMPU fault occurs, cause: 0x1000000d, epc: 0x8000608c

**Expected output(TRIGGER_SMPU_VIOLATION_MODE=STORE_SMPU_EXCEPTION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun 18 2024, 18:40:00
    Download Mode: ILM
    CPU Frequency 16057630 Hz
    CPU HartID: 0
    ------smpu demo with trigger condition 2------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x0, order_out: 32
    Get smpu entry: index 0, prot_out: 0x9c, addr_out: 0x80004000, order_out: 12
    Get smpu entry: index 1, prot_out: 0x99, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address 0x0x80004000
    ----protected_execute succeed!----
    Attempting to read protected_data[0] at 0x90000000
    protected_data[0]: 0xAA succeed
    Attempting to write protected_data[0] at 0x90000000
    Store/AMO SMPU fault occurs, cause: 0x1000000f, epc: 0x800060b2

**Expected output(TRIGGER_SMPU_VIOLATION_MODE=EXECUTE_SHARED_DATA_REGION_EXCEPTION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun 18 2024, 18:40:39
    Download Mode: ILM
    CPU Frequency 16057630 Hz
    CPU HartID: 0
    ------smpu demo with trigger condition 3------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x0, order_out: 32
    Get smpu entry: index 0, prot_out: 0x1e, addr_out: 0x80004000, order_out: 12
    Get smpu entry: index 1, prot_out: 0x1e, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address 0x0x80004000
    Instruction SMPU fault occurs, cause: 0x1000000c, epc: 0x80004000

**Expected output(TRIGGER_SMPU_VIOLATION_MODE=WRITE_READONLY_SHARED_DATA_EXCEPTION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun 18 2024, 18:41:17
    Download Mode: ILM
    CPU Frequency 16057630 Hz
    CPU HartID: 0
    ------smpu demo with trigger condition 4------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x0, order_out: 32
    Get smpu entry: index 0, prot_out: 0x9a, addr_out: 0x80004000, order_out: 12
    Get smpu entry: index 1, prot_out: 0x9f, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address 0x0x80004000
    ----protected_execute succeed!----
    Attempting to read protected_data[0] at 0x90000000
    protected_data[0]: 0xAA succeed
    Attempting to write protected_data[0] at 0x90000000
    Store/AMO SMPU fault occurs, cause: 0x1000000f, epc: 0x800060b2

**Expected output(TRIGGER_SMPU_VIOLATION_MODE=SHARE_CODE_DATA_REGION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun 18 2024, 18:41:46
    Download Mode: ILM
    CPU Frequency 16068116 Hz
    CPU HartID: 0
    ------smpu demo with trigger condition 5------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x0, order_out: 32
    Get smpu entry: index 0, prot_out: 0x9a, addr_out: 0x80004000, order_out: 12
    Get smpu entry: index 1, prot_out: 0x1e, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address 0x0x80004000
    ----protected_execute succeed!----
    Attempting to read protected_data[0] at 0x90000000
    protected_data[0]: 0xAA succeed
    Attempting to write protected_data[0] at 0x90000000
    Won't run here if violates rules check!

**(Default)Expected output(TRIGGER_SMPU_VIOLATION_MODE=RUN_WITH_ENTRY_INACTIVE) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jun 18 2024, 18:42:19
    Download Mode: ILM
    CPU Frequency 16057630 Hz
    CPU HartID: 0
    ------smpu demo with trigger condition 6------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x0, order_out: 32
    Get smpu entry: index 0, prot_out: 0x18, addr_out: 0x80004000, order_out: 12
    Get smpu entry: index 1, prot_out: 0x18, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address 0x0x80004000
    ----protected_execute succeed!----
    Attempting to read protected_data[0] at 0x90000000
    protected_data[0]: 0xAA succeed
    Attempting to write protected_data[0] at 0x90000000
    Won't run here if violates rules check!

.. _design_app_demo_profiling:

demo_profiling
~~~~~~~~~~~~~~

This `demo_profiling application`_ is used to demonstrate how to use gprof or gcov
in Nuclei Studio.

This application itself is modified based on an opensource aes application, we add
gprof and gcov collection code to ``main.c``, it will dump gprof and gcov data in
console when main part code is executed.

.. note::

    * **WARNING** GCC 14 with nuclei sdk code coverage not working, need to modify ``Components/profiling/gcov.c``, changes need to be made like this https://github.com/Nuclei-Software/nuclei-sdk/commit/5aaae0d5a7
    * Introduced in Nuclei SDK 0.6.0, worked with Nuclei Studio >= 2024.02
    * Using gprof or gcov introduces instrument code into the original program,
      necessitating additional memory to store the collected data. This results in
      a slight increase in the program's memory footprint compared to its uninstrumented counterpart.
    * It cannot be directly used in command line mode, you should use it in Nuclei Studio.
    * Please check ``README.md`` about gcov and gprof support in https://github.com/Nuclei-Software/nuclei-sdk/tree/master/Components/profiling

Import or download Nuclei SDK 0.6.0 or later release NPK in Nuclei Studio, and then create a
project called ``demo_profiling`` based on ``app-nsdk_demo_profiling`` using
``Create Nuclei RISC-V C/C++ Project`` Wizard as below:

.. figure:: /asserts/images/create_demo_profiling_example.png
   :width: 80 %
   :align: center
   :alt: Create demo profiling example

And when example is created, assume you want to profiling the ``application`` folder, since
it is the core algorithm of this example, then you just need to do the following steps:

- Right click on the ``application`` folder, and click ``Properities``, and add extra options
  in ``C/C++ Build`` -> ``Settings`` -> ``GNU RISC-V Cross C Compiler`` -> ``Miscellaneous`` -> ``Other compiler flags``.

  - If you want to do gprof, you need to add ``-pg`` option.
  - If you want to do gcov, you need to add ``-coverage`` option.
- Open ``main.c``, and find ``TODO`` item, and comment ``gprof_collect(2);`` or ``gcov_collect(2);`` based on
  gprof or gcov you want to collect.
- If you want to collect gprof data, you also need to modify ``nuclei_sdk/Components/profiling/gprof_stub.c``,
  if you code already has a 1ms period timer interrupt, you should copy code in ``eclic_mtip_handler`` to do
  executing sampling, otherwise you can uncomment ``#define SAMPLE_USING_SYSTIMER``

Here I want to collect both gprof and gcov, so I modify it like below:

.. figure:: /asserts/images/add_profiling_options_in_nuclei_studio.png
   :width: 80 %
   :align: center
   :alt: Add profiling options in Nuclei Studio

.. figure:: /asserts/images/modify_profiling_example_code.png
   :width: 80 %
   :align: center
   :alt: Modify profiling example code

And then compile this example code, and run it using hardware or qemu, qemu is just function model,
so it didn't provide correct timing information.

When program runs, it will dump gprof and gcov data in console, and you can copy all the output as
a file called ``prof.log``, and use ``gprof_parse.py`` to parse the data, and generate a
gcov and gprof binary files.

.. figure:: /asserts/images/parse_profiling_log.png
   :width: 80 %
   :align: center
   :alt: Parse profiling log and generate gcda and gmon.out files

Then you can double click ``gmon.out`` and ``aes.gcda`` to check the gprof and gcov view in Nuclei Studio
like below:

.. figure:: /asserts/images/gprof_gcov_view_in_nuclei_studio.png
   :width: 80 %
   :align: center
   :alt: Gprof and gcov view in Nuclei Studio

About GProf view, please click https://help.eclipse.org/latest/topic/org.eclipse.linuxtools.gprof.docs/Linux_Tools_Project/GProf/User_Guide/GProf-View.html
to learn more.

About Gcov view, please click https://help.eclipse.org/latest/topic/org.eclipse.linuxtools.gcov.docs/Linux_Tools_Project/GCov/User_Guide/Gcov-main-view.html
to learn more.


.. _design_app_demo_pmp:

demo_pmp
~~~~~~~~

This `demo_pmp application`_ is used to demonstrate how to grant physical memory privileges
(read, write, execute) on each physical memory region by machine mode control CSRs.

.. note::

    * It doesn't work with gd32vf103 processor.
    * It needs Nuclei CPU configured with PMP feature
    * Need to enable PMP in <Device.h> if PMP present in CPU.

* ``pmp_violation_fault_handler`` is registered, which is to execute when pmp violation
  exception occurs
* There're two config structures, ``pmp_config_x`` sets protected executable address range
  as 2^12 bytes; ``pmp_config_rw`` sets protected readable/writable address range as 2^12
  bytes, and you can change the ``protection``, ``order``, ``base_addr`` according to your
  memory assignments
* When exception occurs, the print info including ``mcause``, ``mepc`` can be observed
  by serial console, which explains the exception cause of PMP permission violation, and
  shows which asm instruction triggers the violation
* In the application code, there is a macro called ``TRIGGER_PMP_VIOLATION_MODE`` to control the
  PMP working feature:

  - If **TRIGGER_PMP_VIOLATION_MODE=INSTRUCTION_FETCH_EXCEPTION**, the unallowed memory is to
    execute, which triggers ``Instruction access fault``, whose mcause.EXCCODE = 1 and mdcause = 1

  - If **TRIGGER_PMP_VIOLATION_MODE=LOAD_EXCEPTION**, the unallowed memory is to read,
    which triggers ``Load access fault``, whose mcause.EXCODE = 5 and mdcause = 1

  - If **TRIGGER_PMP_VIOLATION_MODE=STORE_EXCEPTION**, the unallowed memory is to write,
    which triggers ``Store/AMO access fault``, whose mcause.EXCODE = 7 and mdcause = 1

  - If **TRIGGER_PMP_VIOLATION_MODE=RUN_WITH_NO_PMP_CHECK**, no violation occurs

**How to run this application:**

.. code-block::shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the demo_pmp directory
    cd application/baremetal/demo_pmp
    # MUST: Your CPU configuration must has PMP configured
    # Change macro __PMP_PRESENT to 1 in <Device.h>
    #define __PMP_PRESENT             1
    # Change macro TRIGGER_PMP_VIOLATION_MODE value in demo_pmp.c
    # to see different working mode of this demo
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 upload

**Expected output(TRIGGER_PMP_VIOLATION_MODE=INSTRUCTION_FETCH_EXCEPTION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Aug 15 2022, 15:45:57
    Download Mode: ILM
    CPU Frequency 16006184 Hz
    ------PMP demo with trigger condition 0------
    Get pmp entry: index 0, prot_out: 0x9b, addr_out: 0x80004000, order_out: 12
    Get pmp entry: index 1, prot_out: 0x9b, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address
    Instruction access fault occurs, cause: 0x30000001, epc: 0x80004000

From disassembly code, MEPC refers to

.. code-block:: console

    80004000:	90002537          	lui	a0,0x90002


**Expected output(TRIGGER_PMP_VIOLATION_MODE=LOAD_EXCEPTION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Aug 15 2022, 15:45:57
    Download Mode: ILM
    CPU Frequency 16006184 Hz
    ------PMP demo with trigger condition 1------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x80004000, order_out: 12
    Get pmp entry: index 1, prot_out: 0x9a, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address
    ----protected_execute succeed!----
    Attempting to read protected_data[0]
    Load access fault occurs, cause: 0x30000005, epc: 0x80004022


From disassembly code, MEPC refers to

.. code-block:: console

    80004022:	00044583          	lbu	a1,0(s0) # 90000000 <_sp+0xffff0000>


**Expected output(TRIGGER_PMP_VIOLATION_MODE=STORE_EXCEPTION) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Aug 15 2022, 15:45:57
    Download Mode: ILM
    CPU Frequency 15998320 Hz
    ------PMP demo with trigger condition 2------
    Get pmp entry: index 0, prot_out: 0x9f, addr_out: 0x80004000, order_out: 12
    Get pmp entry: index 1, prot_out: 0x99, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address
    ----protected_execute succeed!----
    Attempting to read protected_data[0]
    protected_data[0]: 0xAA succeed
    Attempting to write protected_data[0]
    Store/AMO access fault occurs, cause: 0x30000007, epc: 0x80004044

From disassembly code, MEPC refers to

.. code-block:: console

    80004044:	00f40023          	sb	a5,0(s0)


**(Default)Expected output(TRIGGER_PMP_VIOLATION_MODE=RUN_WITH_NO_PMP_CHECK) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Aug 15 2022, 15:45:57
    Download Mode: ILM
    CPU Frequency 16006184 Hz
    ------PMP demo with trigger condition 3------
    Get pmp entry: index 0, prot_out: 0x1f, addr_out: 0x80004000, order_out: 12
    Get pmp entry: index 1, prot_out: 0x1b, addr_out: 0x90000000, order_out: 12
    Attempting to fetch instruction from protected address
    ----protected_execute succeed!----
    Attempting to read protected_data[0]
    protected_data[0]: 0xAA succeed
    Attempting to write protected_data[0]
    Won't run here if violates L R\W\X permission check!

.. _design_app_demo_cidu:

demo_cidu
~~~~~~~~~

This `demo_cidu application`_ is used to demonstrate External Interrupt Distribution
(external interrupt broadcast/first come first claim), Inter Core interrupt and Semaphore
of Cluster Interrupt Distribution Unit (CIDU).

This demo requests the SMP cores share the same RAM and ROM, for example, in current
evalsoc/demosoc system, ilm/dlm are private resource for cpu, only the DDR/SRAM memory are shared resource
for all the cpu.

.. note::

    * It doesn't work with gd32vf103 processor.
    * It needs Nuclei SMP CPU configured with CIDU feature
    * It needs Nuclei EvalSoC's uart and its interrupt, if you want to port it, you need to port uart driver of your SoC
    * Need to enable CIDU in <Device.h> if CIDU present in cluster.
    * Multicore SoC is needed.

* ``UART0`` receive is used as external interrupt, registered as ``eclic_uart0_int_handler``, which is the best choice
  for evalsoc/demosoc and is easy to trigger by writing the serial terminal
* ``UART0`` receive interrupt can be broadcast to all the cores or some, and also first coming first claim
  mode will ensure only the first responding core handle the interrupt service routine(ISR)
* Inter core interrupt shows likes this: core3 sends interrupt to core2, core2 sends interrupt to core1,
  core1 sends interrupt to core0, and core0 sends interrupt to core3, registered as ``eclic_inter_core_int_handler``,
  supposing the SoC is four cores, and etc.
* To demonstrate it will handle properly if multiple cores send interrupt to one core simultaneously,
  besides core2, core0 also sends interrupt to core1, supposing the SoC is four core
* To protect ``UART0`` resource when multicores want to access it(call ``printf``), semaphore is configured, which needs to
  be acquired successfully before accessing ``UART0``, and release it after job done
* ``ENABLE_FIRST_COME_FIRST_CLAIM_MODE`` is defined by default, you can comment it to just use broadcast mode

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # Use Nuclei UX900 SMP 2/4/8(4/8 is better) Core RISC-V processor as example
    # application needs to run in ddr memory not in ilm memory
    # cd to the demo_cidu directory
    cd application/baremetal/demo_cidu
    # Since Nuclei SDK 0.7.0, if you are sure CFG_HAS_IDU is not defined in cpufeature.h, but you have CIDU
    # you can pass extra make variable XLCFG_CIDU=1 during make command to tell sdk
    # the cidu present, it will define CFG_HAS_IDU
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval SMP=4 CORE=ux900 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval SMP=4 CORE=ux900 upload

**Expected output(inter core interrupt) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 10 2023, 18:39:17
    Download Mode: SRAM
    CPU Frequency 100602675 Hz
    CPU HartID: 0
    Core 3 has received interrupt from core 0
    Core 1 has received interrupt from core 0
    Core 2 has received interrupt from core 3
    Core 1 has received interrupt from core 2
    Core 0 has received interrupt from core 1

From output, each core sends interrupt in order, and core 1 has received interrupts from
both core 0 and core 2.


**Expected output(write anything to the serial terminal, enable first come first claim mode) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 10 2023, 18:44:45
    Download Mode: SRAM
    CPU Frequency 100612833 Hz
    CPU HartID: 0
    Core 3 has received interrupt from core 0
    Core 1 has received interrupt from core 0
    Core 2 has received interrupt from core 3
    Core 1 has received interrupt from core 2
    Core 0 has received interrupt from core 1
    Core 2 enters uart0_receive_handler
    Core 1 enters uart0_receive_handler
    Core 2 wants to process rx input
    Core 2 processed input:d
    Core 3 enters uart0_receive_handler
    Core 0 enters uart0_receive_handler
    Core 3 wants to process rx input
    Core 3 enters uart0_receive_handler
    Core 1 enters uart0_receive_handler
    Core 3 wants to process rx input
    Core 3 processed input:q
    Core 0 enters uart0_receive_handler
    Core 2 enters uart0_receive_handler
    Core 0 wants to process rx input
    Core 0 enters uart0_receive_handler
    Core 1 enters uart0_receive_handler
    Core 0 wants to process rx input
    Core 0 processed input:s
    Core 3 enters uart0_receive_handler
    Core 2 enters uart0_receive_handler
    Core 3 wants to process rx input
    Core 1 enters uart0_receive_handler
    Core 2 enters uart0_receive_handler
    Core 0 enters uart0_receive_handler
    Core 1 wants to process rx input
    Core 1 processed input:g
    Core 3 enters uart0_receive_handler
    Core 3 wants to process rx input

From output, though setting interrupt broadcasted to all(all the core enters the ISR), while only one core (the first one)
can claim the the interrupt(first come first claim) then process the uart0 input, others quit when find interrupt has been claimed.


**Expected output(write anything to the serial terminal, disable first come first claim mode) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 10 2023, 18:48:47
    Download Mode: SRAM
    CPU Frequency 100602675 Hz
    CPU HartID: 0
    Core 3 has received interrupt from core 0
    Core 1 has received interrupt from core 0
    Core 2 has received interrupt from core 3
    Core 1 has received interrupt from core 2
    Core 0 has received interrupt from core 1
    Core 2 enters uart0_receive_handler
    Core 0 enters uart0_receive_handler
    Core 2 wants to process rx input
    Core 2 processed input:q
    Core 0 wants to process rx input
    Core 1 enters uart0_receive_handler
    Core 1 wants to process rx input
    Core 3 enters uart0_receive_handler
    Core 3 wants to process rx input
    Core 3 enters uart0_receive_handler
    Core 0 enters uart0_receive_handler
    Core 1 enters uart0_receive_handler
    Core 2 enters uart0_receive_handler
    Core 0 wants to process rx input
    Core 0 processed input:w
    Core 1 wants to process rx input
    Core 3 wants to process rx input
    Core 2 wants to process rx input
    Core 2 enters uart0_receive_handler
    Core 0 enters uart0_receive_handler
    Core 1 enters uart0_receive_handler
    Core 1 wants to process rx input
    Core 1 processed input:e
    Core 0 wants to process rx input
    Core 2 wants to process rx input
    Core 3 enters uart0_receive_handler
    Core 3 wants to process rx input
    Core 3 enters uart0_receive_handler
    Core 1 enters uart0_receive_handler
    Core 3 wants to process rx input
    Core 3 processed input:r
    Core 0 enters uart0_receive_handler
    Core 1 wants to process rx input
    Core 0 wants to process rx input
    Core 2 enters uart0_receive_handler
    Core 2 wants to process rx input

From output, all the core enters the ISR(means broadcasted), while only one core can process
the uart0 input(semaphore used), when semaphore released, other core wants to handle the ISR job(means claim mode disabled),
but process nothing (keyboard input has been received and rx interrupt pending cleared) because it has been processed.

.. _design_app_demo_cache:

demo_cache
~~~~~~~~~~

.. note::

    * It doesn't work with gd32vf103 processor.
    * It needs Nuclei CPU configured with CCM feature

This `demo_cache application`_ is used to demonstrate how to understand cache mechanism.

This demo requests DCache, ICache and CCM(Cache Control and Maintenance), and needs to run in DDR/SRAM memory,
because cache will bypass when run in ilm, data in dlm(private resource for cpu).

.. note::
    * Need to enable DCache, ICACHE, CCM in <Device.h> if present in CPU.

* An arrary( ``ROW_SIZE`` * ``COL_SIZE`` ) called ``array_test`` is created to access its first element ``array_test[0][0]``
* Firstly, enable and invalidate all DCache, update ``array_test`` by writing a consant, the cache miss happens and will update ``array_test``'s
  mapping value in DCache, read out ``array_test[0][0]``; then disable the Dcache, init array_test in the ddr memory to different constant,
  read out ``array_test[0][0]``; after that, enable the DCache flushes DCache to ddr memory, read out ``array_test[0][0]``, and compare these ``array_test[0][0]`` value
* Again disable the Dcache, init array_test in the ddr memory, read out ``array_test[0][0]``; then enable the DCache, read out ``array_test[0][0]``,
  and compare with the one before
* **For further understanding**, if the CPU has configured ``HPM`` (Hardware Performance Monitor), observe the cache miss count by recording the cache miss
  of updating array_test with DCache invalid, then compared to updating array_test with keeping DCache valid; also, compare the cache miss
  count of updating array_test row by row with column by column
* ``BIG_ROW_SIZE`` can be defined to make the array size ``2048*64`` bytes, which is big to see the cache miss gap(performance gap) between
  updating ``array_test`` row by row and column by column
* In our evalsoc/demosoc, cache line size is 64 bytes generally, so ``array_test``'s ``COL_SIZE`` is 64 bytes for calculating the cache miss manually and easily
* When ``HPM`` used, because there's global variables in ``HPM_START`` and ``HPM_END`` , **these will bring 3 cache miss itself** (not considering cached)
* You can manage ICache apis like DCache, which skipped in this demo for less similar code
* Different compile optimization level such as -O2/-O0 effects cache miss

.. note::
    * There's ``printf`` hidden in ``HPM_END``, if there is another HPM_END before it, the ``printf`` will bring some cache miss

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # Use Nuclei UX900 Core RISC-V processor as example
    # application needs to run in ddr memory not in ilm memory
    # cd to the demo_cache directory
    cd application/baremetal/demo_cache
    # Since Nuclei SDK 0.7.0, if you are sure CFG_HAS_IOCC is not defined in cpufeature.h, but you have CCM
    # you can pass extra make variable XLCFG_CCM=1 during make command to tell sdk
    # the ccm present, it will define CFG_HAS_IOCC
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=ux900 DOWNLOAD=sram CCM_EN=1 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=ux900 DOWNLOAD=sram CCM_EN=1 upload

**Expected output(DISABLE_NMSIS_HPM defined) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jul 22 2025, 18:05:53
    Download Mode: DDR
    CPU Frequency 49993154 Hz
    CPU HartID: 0
    Benchmark initialized
    DCache Linesize is 64 bytes, ways is 2, setperway is 512, total size is 65536 bytes

    array_test 10 * 64 bytes

    ------Update array in memory------

    ------Update array to all 0xab in cache: array_update_by_row------
    CSV, array_update_by_row_cycle, 766

    -------Keep DCache valid, do array_update_by_row again-------
    CSV, array_update_by_row_cycle, 136

    -------Invalidate all the Dcache-------

    ------Update array to all 0xab in cache: array_update_by_col ------
    CSV, array_update_by_col_cycle, 441
    Read out array_test[0][0] 0xab in cache, then disable DCache

    ------Init array in memory to all 0x34------
    Read out array_test[0][0] 0x34 in memory, then enable Dcache
    After cache flushed to memory, array_test[0][0] in memory is 0xab

    ------Again init array in memory to all 0x34, then enable DCache------
    Read out array_test[0][0] 0x34 in memory
    Read out array_test[0][0] 0xab in cache, when mapped value in memory has changed

From output, ``array_test`` is updated in memory to all 0xab, and **cached in DCache** when miss happens,
then disable DCache, init array_test just in memory to all 0x34, **after cache flushed to memory**,
``array_test`` in memory is all 0xab same with ``array_test`` in DCache. **Disable DCache and init array_test
again**, ``array_test`` now (all 0x34) differs with cached array_test (all 0xab) after DCache enabled.


**Expected output(DISABLE_NMSIS_HPM undefined and CFG_HAS_HPM defined in cpufeature.h) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jul 22 2025, 17:42:24
    Download Mode: DDR
    CPU Frequency 49993154 Hz
    CPU HartID: 0
    Benchmark initialized
    DCache Linesize is 64 bytes, ways is 2, setperway is 512, total size is 65536 bytes

    array_test 10 * 64 bytes

    ------Update array in memory------
    High performance monitor initialized

    ------Update array to all 0xab in cache: array_update_by_row------
    CSV, array_update_by_row_cycle, 809
    HPM4:0xf0000021, array_update_by_row_dcache_miss, 19
    HPM3:0xf0000011, array_update_by_row_icache_miss, 36

    -------Keep DCache valid, do array_update_by_row again-------
    CSV, array_update_by_row_cycle, 147
    HPM4:0xf0000021, array_update_by_row_dcache_miss, 0
    HPM3:0xf0000011, array_update_by_row_icache_miss, 3

    -------Invalidate all the Dcache-------

    ------Update array to all 0xab in cache: array_update_by_col ------
    CSV, array_update_by_col_cycle, 436
    HPM4:0xf0000021, array_update_by_col_dcache_miss, 19
    Read out array_test[0][0] 0xab in cache, then disable DCache

    ------Init array in memory to all 0x34------
    Read out array_test[0][0] 0x34 in memory, then enable Dcache
    After cache flushed to memory, array_test[0][0] in memory is 0xab

    ------Again init array in memory to all 0x34, then enable DCache------
    Read out array_test[0][0] 0x34 in memory
    Read out array_test[0][0] 0xab in cache, when mapped value in memory has changed
    HPM4:0xf0000021, dcachemiss_readonebyte, 0

From output, ``HPM`` is enabled, cache miss is counted and ``array_test`` size is 10 * 64 bytes.
**At first, DCache is invalid**, the first time ``array_test`` update by row has 10 miss(HPM4 shows more,
because HPM4 and other execution it wraps bring some); **Keep DCache valid**, update array_test by row again,
cache miss decreases rapidly, which means ``array_test`` has already cached;
**Then invalidate all the Dcache**, array_test update by col seems has the same cache miss as update by row.


**Expected output(BIG_ROW_SIZE defined, DISABLE_NMSIS_HPM undefined and CFG_HAS_HPM defined in cpufeature.h) as below:**

.. code-block:: console

    Nuclei SDK Build Time: Jul 22 2025, 17:50:49
    Download Mode: DDR
    CPU Frequency 50000363 Hz
    CPU HartID: 0
    Benchmark initialized
    DCache Linesize is 64 bytes, ways is 2, setperway is 512, total size is 65536 bytes

    array_test 2048 * 64 bytes

    ------Update array in memory------
    High performance monitor initialized

    ------Update array to all 0xab in cache: array_update_by_row------
    CSV, array_update_by_row_cycle, 155917
    HPM4:0xf0000021, array_update_by_row_dcache_miss, 2068
    HPM3:0xf0000011, array_update_by_row_icache_miss, 37

    -------Keep DCache valid, do array_update_by_row again-------
    CSV, array_update_by_row_cycle, 187584
    HPM4:0xf0000021, array_update_by_row_dcache_miss, 2068
    HPM3:0xf0000011, array_update_by_row_icache_miss, 4

    -------Invalidate all the Dcache-------

    ------Update array to all 0xab in cache: array_update_by_col ------
    CSV, array_update_by_col_cycle, 620971
    HPM4:0xf0000021, array_update_by_col_dcache_miss, 16405
    Read out array_test[0][0] 0xab in cache, then disable DCache

    ------Init array in memory to all 0x34------
    Read out array_test[0][0] 0x34 in memory, then enable Dcache
    After cache flushed to memory, array_test[0][0] in memory is 0xab

    ------Again init array in memory to all 0x34, then enable DCache------
    Read out array_test[0][0] 0x34 in memory
    Read out array_test[0][0] 0xab in cache, when mapped value in memory has changed
    HPM4:0xf0000021, dcachemiss_readonebyte, 0

From output, ``array_test`` size is enlarged to ``2048 * 64`` bytes, which is **two times the size of DCache (1024 * 64 bytes)**.
Cache miss brought by ``HPM`` itself ignored, array update by col has **8 times cache miss(16405) as the array update by row has(2068)**.
That's because when first byte access brings one cache misse, **one cache line(64 bytes in this demo) is fetched to cache**,
and it works best if other 63 cached bytes can be accessed before getting dirty as soon as possible, as update by row does, so the cache miss
equals nearly to ``ROW_SIZE``, while when updated by col, every byte in ``ROW_SIZE`` * ``COL_SIZE`` will cause a cache miss! which is cache-unfriendly.
What's more, considering ``array_test`` size is two times the size of DCache, the cached data has been kicked out when ``do array_update_by_row again``,
so the cache miss is nearly the same as the first time.

.. _design_app_demo_stack_check:

demo_stack_check
~~~~~~~~~~~~~~~~

.. note::

    * It doesn't work with gd32vf103 processor.
    * It needs Nuclei CPU configured with stack check feature

This `demo_stack_check application`_ is used to demonstrate how to check stack overflow and underflow and track the ``sp``.

For now, this demo needs to run on **only 300 Series v4.2.0 or later**, which supports this ``Stack Check`` function.

.. note::
    * The Stack Check can work as expected only when the stack downwardly grows.

* ``STACK_TOP``, ``STACK_BOTTOM``, ``STACK_SIZE`` refers to stack's high/low address and size in bytes, which gets from the linker script
* ``stack_corrupt_exception_handler`` is registered as exception handler to process stack overflow and underflow
* A simple recursive function of calculating factorial is reformed, which will consume stack more or less by the ``n`` input, thus may cause
  overflow; a trick is used to cause underflow that when it iterates over, decrease the stack base value to make the underflow condition on purpose
* The ``sp`` has grown downwardly 0x50 bytes in the exception entry saving context, in this demo, add ``sp`` by 0x50 is the ``sp`` value that triggered overflow/underflow
* When it comes into exception and handle it over, the flow doesn't stop in it as usual, and ``pc`` continues to execute, which is on purpose to show
  ``overflow``, ``underflow`` and ``track sp`` mode in one-time run
* In ``sp track mode``, logging is enabled in ``factorial``, to show the ``sp`` value change; and the BOUND won't track sp(won't change) if sp is bigger in the second run

.. note::
    * Must set the BOUND and BASE before setting the check mode
    * Reserve 0x200 bytes for exception stack push/pop

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # Use Nuclei n300 Core RISC-V processor as example
    # cd to the demo_stack_check directory
    cd application/baremetal/demo_stack_check
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ddr CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ddr CORE=n300 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Oct 18 2023, 18:45:02
    Download Mode: ILM
    CPU Frequency 15996682 Hz
    CPU HartID: 0
    Stack's top high address: 0x90010000, stack's bottom low address: 0x9000fa00, stack size: 0x600

    --------OVERFLOW CHECK MODE--------
    BOUND register set to: 0x9000fa00
    BASE register set to: 0x90010000
    Stack overflow fault occurs at iteration 84, cause: 0x30000018, epc: 0x80000e90, sp: 0x9000f990

    --------UNDERFLOW CHECK MODE--------
    BASE register set to: 0x9000fd00
    Stack underflow fault occurs at iteration 1, cause: 0x30000019, epc: 0x80000fd0, sp: 0x9000fd00
    BASE register set to: 0x90010000

    --------TRACK SP MODE--------
    BOUND register set to: 0x90010000
    Iterations: 1, stack bound: 0x9000fdc0
    Iterations: 2, stack bound: 0x9000fd70
    Iterations: 3, stack bound: 0x9000fd20
    Iterations: 4, stack bound: 0x9000fcd0
    Iterations: 5, stack bound: 0x9000fc80
    Iterations: 6, stack bound: 0x9000fc30
    Iterations: 7, stack bound: 0x9000fbe0
    Iterations: 8, stack bound: 0x9000fb90
    Iterations: 9, stack bound: 0x9000fb40
    Iterations: 10, stack bound: 0x9000faf0
    Iterations: 11, stack bound: 0x9000faa0
    Iterations: 12, stack bound: 0x9000fa50
    Iterations: 13, stack bound: 0x9000fa00
    Iterations: 14, stack bound: 0x9000f9b0
    Iterations: 15, stack bound: 0x9000f960
    Iterations: 16, stack bound: 0x9000f910
    Iterations: 17, stack bound: 0x9000f8c0
    Iterations: 18, stack bound: 0x9000f870
    Calculate factorial over, the max stack used downwards to: 0x9000f820

    Rerun it. The BOUND won't track sp if sp is bigger:
    Iterations: 1, stack bound: 0x9000f820
    Iterations: 2, stack bound: 0x9000f820
    Iterations: 3, stack bound: 0x9000f820
    Iterations: 4, stack bound: 0x9000f820
    Iterations: 5, stack bound: 0x9000f820

    Stack check demo over!

.. _design_app_demo_pma:

demo_pma
~~~~~~~~

This `demo_pma application`_ is used to demonstrate how to set memory region to different attribute(``Device``/``Non-Cacheable``/``Cacheable``)

.. note::
    * PMA are split into three attributes: ``Device``/``Cacheable``/``Non-Cacheable``, which correspondingly the whole memory region are split into
    * Take care to set the region type and address range, which maybe causing function or performance issue!
    * ``NMSIS/Core/Include/core_feature_pma.h`` provides apis like ``PMA_DisableHwXXRegion``, ``PMA_EnableHwXXRegion`` to disable/enable
      hardware-defined regions, but please take care to use it, because maybe the region you disable will go to ``Device``
      (maybe covered by another bigger-range ``Device`` region!), then instruction fetch exception happens!

* Observe cycles taken when executing same task ``array_read_by_row``(read from ``array_test``, update into ``array_test_r``) by changing the same memory region to ``Non-Cacheable``/``Cacheable``
* Struct ``PMA_CONFIG`` is used to assign PMA, which consists of ``region_type`` ``region_base`` ``region_size``  ``region_enable``
* ``region_type`` could be ``PMA_REGION_TYPE_SECSHARE``, ``PMA_REGION_TYPE_NC``, ``PMA_REGION_TYPE_DEV``, ``PMA_REGION_TYPE_CA``
* ``region_base`` is base physical address, which needs to be 4K byte aligned
* ``region_size`` needs to be 4K byte aligned; the ``region_base`` should be integer multiples of ``region_size``
* ``region_enable`` enable(1) or disable(0) the region, could be ``PMA_REGION_ENA``, ``PMA_REGION_DIS``
* After ``pma_cfg`` is assigned, and give the ``entry_idx``, call ``PMA_SetRegion`` to take effect
* The ``entry_idx`` (0-n) depends on number of paired ``mattri(n)_mask`` and ``mattri(n)_base``, refer to Nuclei ISA specifications for max region entries
* The api will do aligning by 4KB(because region granularity is 4KB) to ``region_base`` and ``region_size`` forcely
* The regions can be overlapped as the priority: ``Non-Cacheable`` > ``Cacheable`` > ``Device``, , but especially be careful not to
  overlap software's instruction/data sections by Device, or overlap Device(like uart) memory by Cacheable
* ``PMA_GetRegion`` could retrieve the region info detail


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # Use Nuclei ux900 Core RISC-V processor as example
    # cd to the demo_pma directory
    cd application/baremetal/demo_pma
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=ux900 DOWNLOAD=sram CCM_EN=1 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=ux900 DOWNLOAD=sram CCM_EN=1 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May 23 2025, 15:02:30
    Download Mode: SRAM
    CPU Frequency 50005606 Hz
    CPU HartID: 0
    DCache Linesize is 64 bytes, ways is 2, setperway is 512, total size is 65536 bytes

    array_test size: 64 * 64 bytes, addr: 0xa0013000

    Set to NonCacheable region
    Region type: 0x4,region base addr: 0xa0013000, region size: 0x1000, region status: 1
    HPM4:0xf0000021, array_read_by_row_dcache_miss_noncacheable, 64

    Set to Cacheable region
    Region type: 0x0,region base addr: 0xa0013000, region size: 0x1000, region status: 1
    HPM4:0xf0000021, array_read_by_row_dcache_miss_cacheable, 128

From output, considering ``array_read_by_row_dcache_miss_noncacheable`` counting the common part cache miss including ``array_test_r`` which belongs to Cacheable.
So ``array_read_by_row_dcache_miss_cacheable`` minus ``array_read_by_row_dcache_miss_noncacheable``, we get exactly the cache miss(here is the row number 64) that
``array_test`` brings in Cacheable region, and it demonstrates ``array_test`` brings no cache miss in NonCacheable region as expected.

.. note::
    * In Nuclei Evalsoc core ux900 for example, the sram/ddr memory locates originally in hardware-defined Cacheable region(which configured by rtl configuration stage),
      So this demo first covers original attribute by ``NonCacheable``, then ``Cacheable`` (that's recovered)
    * As the prority: ``Non-Cacheable`` > ``Cacheable`` > ``Device``, it can't cover original attribute(``Cacheable``) by ``Device``!

FreeRTOS applications
---------------------

.. _design_app_freertos_demo:

demo
~~~~

This `freertos demo application`_ is to show basic freertos task functions.

* Two freertos tasks are created
* A software timer is created

In Nuclei SDK, we provided code and Makefile for this ``freertos demo`` application.

* **RTOS = FreeRTOS** is added in its Makefile to include FreeRTOS service
* The **configTICK_RATE_HZ** in ``FreeRTOSConfig.h`` is set to 100, you can change it
  to other number according to your requirement.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the freertos demo directory
    cd application/freertos/demo
    # Clean the application first
    make SOC=gd32vf103 clean
    # Build and upload the application
    make SOC=gd32vf103 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 21 2020, 14:56:00
    Download Mode: FLASHXIP
    CPU Frequency 109058823 Hz
    Before StartScheduler
    Enter to task_1
    task1 is running 0.....
    Enter to task_2
    task2 is running 0.....
    timers Callback 0
    timers Callback 1
    task1 is running 1.....
    task2 is running 1.....
    timers Callback 2
    timers Callback 3
    task1 is running 2.....
    task2 is running 2.....
    timers Callback 4
    timers Callback 5
    task1 is running 3.....
    task2 is running 3.....
    timers Callback 6
    timers Callback 7
    task1 is running 4.....
    task2 is running 4.....
    timers Callback 8
    timers Callback 9
    task1 is running 5.....
    task2 is running 5.....
    timers Callback 10
    timers Callback 11

.. _design_app_freertos_smpdemo:

smpdemo
~~~~~~~

This `freertos smpdemo application`_ is to show basic freertos smp task functions.

* x freertos tasks(different priorities) are created if your cpu has x cores according to the ``SMP=x`` settings
* A software timer is created
* Need to run using **DOWNLOAD=sram** mode

In Nuclei SDK, we provided code and Makefile for this ``freertos smpdemo`` application.

* **RTOS = FreeRTOS** is added in its Makefile to include FreeRTOS service
* The **configTICK_RATE_HZ** in ``FreeRTOSConfig.h`` is set to 100, you can change it
  to other number according to your requirement.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the freertos demo directory
    cd application/freertos/smpdemo
    # This need to run on NX900 SMPx2 CPU
    # Clean the application first
    make clean
    # Build and upload the application
    make upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May 28 2024, 13:17:41
    Download Mode: SRAM
    CPU Frequency 50322800 Hz
    CPU HartID: 0
    Startup FreeRTOS SMP on hartid 0
    Enter to task 1
    task 1 prio 1 is running 0 on hart 0.....
    Enter to task 0
    task 0 prio 0 is running 0 on hart 0.....
    task 1 prio 1 is running 1 on hart 1.....
    task 0 prio 0 is running 1 on hart 0.....
    task 1 prio 1 is running 2 on hart 1.....
    task 0 prio 0 is running 2 on hart 0.....
    task 1 prio 1 is running 3 on hart 1.....
    task 0 prio 0 is running 3 on hart 0.....
    task 1 prio 1 is running 4 on hart 1.....
    task 0 prio 0 is running 4 on hart 0.....
    task 1 prio 1 is running 5 on hart 0.....
    timers Callback 0 on hart 1
    task 0 prio 0 is running 5 on hart 1.....
    task 1 prio 1 is running 6 on hart 1.....
    task 0 prio 0 is running 6 on hart 0.....
    task 1 prio 1 is running 7 on hart 1.....
    task 0 prio 0 is running 7 on hart 0.....
    task 1 prio 1 is running 8 on hart 1.....
    task 0 prio 0 is running 8 on hart 0.....
    task 1 prio 1 is running 9 on hart 1.....
    task 0 prio 0 is running 9 on hart 0.....
    task 1 prio 1 is running 10 on hart 0.....
    timers Callback 1 on hart 1

UCOSII applications
-------------------

.. _design_app_ucosii_demo:

demo
~~~~

This `ucosii demo application`_ is show basic ucosii task functions.

* 4 tasks are created
* 1 task is created first, and then create 3 other tasks and then suspend itself

In Nuclei SDK, we provided code and Makefile for this ``ucosii demo`` application.

* **RTOS = UCOSII** is added in its Makefile to include UCOSII service
* The **OS_TICKS_PER_SEC** in ``os_cfg.h`` is by default set to 50, you can change it
  to other number according to your requirement.

.. note:

   * For Nuclei SDK release > v0.2.2, the UCOSII source code is replaced using the
     version from https://github.com/SiliconLabs/uC-OS2/, and application development
     for UCOSII is also changed, the ``app_cfg.h``, ``os_cfg.h`` and ``app_hooks.c`` files
     are required in application source code.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the ucosii demo directory
    cd application/ucosii/demo
    # Clean the application first
    make SOC=gd32vf103 clean
    # Build and upload the application
    make SOC=gd32vf103 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 21 2020, 15:00:35
    Download Mode: FLASHXIP
    CPU Frequency 108524271 Hz
    Start ucosii...
    create start task success
    start all task...
    task3 is running... 1
    task2 is running... 1
    task1 is running... 1
    task3 is running... 2
    task2 is running... 2
    task3 is running... 3
    task2 is running... 3
    task1 is running... 2
    task3 is running... 4
    task2 is running... 4
    task3 is running... 5
    task2 is running... 5
    task1 is running... 3
    task3 is running... 6
    task2 is running... 6
    task3 is running... 7
    task2 is running... 7
    task1 is running... 4
    task3 is running... 8
    task2 is running... 8
    task3 is running... 9
    task2 is running... 9
    task1 is running... 5
    task3 is running... 10
    task2 is running... 10
    task3 is running... 11
    task2 is running... 11
    task1 is running... 6
    task3 is running... 12
    task2 is running... 12


RT-Thread applications
----------------------

.. _design_app_rtthread_demo:

demo
~~~~

This `rt-thread demo application`_ is show basic rt-thread thread functions.

* main function is a pre-created thread by RT-Thread
* main thread will create 5 test threads using the same function ``thread_entry``

In Nuclei SDK, we provided code and Makefile for this ``rtthread demo`` application.

* **RTOS = RTThread** is added in its Makefile to include RT-Thread service
* The **RT_TICK_PER_SECOND** in ``rtconfig.h`` is by default set to `100`, you can change it
  to other number according to your requirement.


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the rtthread demo directory
    cd application/rtthread/demo
    # Clean the application first
    make SOC=gd32vf103 clean
    # Build and upload the application
    make SOC=gd32vf103 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Apr 14 2020, 10:14:30
    Download Mode: FLASHXIP
    CPU Frequency 108270000 Hz

    \ | /
    - RT -     Thread Operating System
    / | \     3.1.3 build Apr 14 2020
    2006 - 2019 Copyright by rt-thread team
    Main thread count: 0
    thread 0 count: 0
    thread 1 count: 0
    thread 2 count: 0
    thread 3 count: 0
    thread 4 count: 0
    thread 0 count: 1
    thread 1 count: 1
    thread 2 count: 1
    thread 3 count: 1
    thread 4 count: 1
    Main thread count: 1
    thread 0 count: 2
    thread 1 count: 2
    thread 2 count: 2
    thread 3 count: 2
    thread 4 count: 2
    thread 0 count: 3
    thread 1 count: 3
    thread 2 count: 3
    thread 3 count: 3
    thread 4 count: 3
    Main thread count: 2
    thread 0 count: 4
    thread 1 count: 4

.. _design_app_rtthread_msh:

msh
~~~

This `rt-thread msh application`_ demonstrates msh shell in serial console which is a component of rt-thread.

* ``MSH_CMD_EXPORT(nsdk, msh nuclei sdk demo)`` exports a command ``nsdk`` to msh shell

In Nuclei SDK, we provided code and Makefile for this ``rtthread msh`` application.

* **RTOS = RTThread** is added in its Makefile to include RT-Thread service
* **RTTHREAD_MSH := 1** is added in its Makefile to include RT-Thread msh component
* The **RT_TICK_PER_SECOND** in ``rtconfig.h`` is by default set to `100`, you can change it
  to other number according to your requirement.
* To run this application in :ref:`design_soc_evalsoc`, the SoC clock frequency must be above 16MHz,
  if run in 8MHz, uart read is not correct due to bit error in uart rx process.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the rtthread msh directory
    cd application/rtthread/msh
    # Clean the application first
    make SOC=gd32vf103 clean
    # Build and upload the application
    make SOC=gd32vf103 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Dec 23 2020, 16:39:21
    Download Mode: FLASHXIP
    CPU Frequency 108810000 Hz

    \ | /
    - RT -     Thread Operating System
    / | \     3.1.3 build Dec 23 2020
    2006 - 2019 Copyright by rt-thread team
    Hello RT-Thread!
    msh >help
    RT-Thread shell commands:
    list_timer       - list timer in system
    list_mailbox     - list mail box in system
    list_sem         - list semaphore in system
    list_thread      - list thread
    version          - show RT-Thread version information
    ps               - List threads in the system.
    help             - RT-Thread shell help.
    nsdk             - msh nuclei sdk demo

    msh >ps
    thread   pri  status      sp     stack size max used left tick  error
    -------- ---  ------- ---------- ----------  ------  ---------- ---
    tshell     6  ready   0x00000178 0x00001000    09%   0x00000008 000
    tidle      7  ready   0x00000078 0x0000018c    30%   0x00000020 000
    main       2  suspend 0x000000b8 0x00000200    35%   0x00000013 000
    msh >nsdk
    Hello Nuclei SDK!
    msh >

.. _design_app_rtthread_demo_smode:

demo_smode
~~~~~~~~~~

This `rt-thread demo smode application`_ is show how to use rt-thread in S-Mode.

It is similar to the normal rt-thread demo, but rt-thread itself is running in S-Mode,
so we have to do some PMP and TEE configuration in M-Mode before go to S-Mode.

The main feature required is the TEE, and SSTC is also preferred.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the rtthread demo_smode directory
    cd application/rtthread/demo_smode
    # Clean the application first
    # Assume you are using n300
    make SOC=evalsoc CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc CORE=n300 upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: Feb 21 2025, 11:12:24
    Download Mode: ILM
    CPU Frequency 16005857 Hz
    CPU HartID: 0
    Set ECLIC Timer S-Mode Interrupt and Software Timer S-Mode Interrupt to be executed in S-Mode
    Drop to S-Mode to prepare RT-Thread Environment

     \ | /
    - RT -     Thread Operating System
     / | \     3.1.5 build Feb 21 2025
     2006 - 2020 Copyright by rt-thread team
    Main thread count: 0
    thread 0 count: 0
    thread 1 count: 0
    thread 2 count: 0
    thread 3 count: 0
    thread 4 count: 0
    thread 0 count: 1
    thread 1 count: 1
    thread 2 count: 1
    thread 3 count: 1
    thread 4 count: 1
    Main thread count: 1
    thread 0 count: 2
    thread 1 count: 2
    thread 2 count: 2
    thread 3 count: 2
    thread 4 count: 2
    thread 0 count: 3
    thread 1 count: 3
    thread 2 count: 3
    thread 3 count: 3
    thread 4 count: 3
    Main thread count: 2

ThreadX applications
--------------------

.. _design_app_threadx_demo:

demo
~~~~

This `threadx demo application`_ is show basic ThreadX thread functions.

This threadx demo is modified based on https://github.com/eclipse-threadx/threadx/blob/v6.4.1_rel/samples/demo_threadx.c

In Nuclei SDK, we provided code and Makefile for this ``threadx demo`` application.

* **RTOS = ThreadX** is added in its Makefile to include ThreadX service
* The **TX_INCLUDE_USER_DEFINE_FILE** macro is defined in Makefile, so you can include customized user configuration
  file ``tx_user.h``


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei SDK environment
    # cd to the threadx demo directory
    cd application/threadx/demo
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei SDK Build Time: May 28 2024, 13:26:41
    Download Mode: ILM
    CPU Frequency 50322800 Hz
    CPU HartID: 0
    thread 6_7 is running, current is 6, thread 6 counter 1, thread 7 counter 1
    thread 6_7 is running, current is 7, thread 6 counter 2, thread 7 counter 1
    thread 6_7 is running, current is 6, thread 6 counter 2, thread 7 counter 2
    thread 6_7 is running, current is 7, thread 6 counter 3, thread 7 counter 2
    thread 6_7 is running, current is 6, thread 6 counter 3, thread 7 counter 3
    thread 6_7 is running, current is 7, thread 6 counter 4, thread 7 counter 3
    thread 6_7 is running, current is 6, thread 6 counter 4, thread 7 counter 4
    thread 6_7 is running, current is 7, thread 6 counter 5, thread 7 counter 4


.. _helloworld application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/helloworld
.. _cpuinfo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/cpuinfo
.. _demo_timer application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_timer
.. _demo_clint_timer application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_clint_timer
.. _demo_eclic application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_eclic
.. _demo_plic application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_plic
.. _demo_dsp application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_dsp
.. _smphello application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/smphello
.. _lowpower application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/lowpower
.. _demo_nice application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_nice
.. _demo_vnice application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_vnice
.. _coremark benchmark application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/benchmark/coremark
.. _dhrystone benchmark application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/benchmark/dhrystone
.. _dhrystone_v2.2 benchmark application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/benchmark/dhrystone_v2.2
.. _whetstone benchmark application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/benchmark/whetstone
.. _whetstone_v1.2 benchmark application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/benchmark/whetstone_v1.2
.. _freertos demo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/freertos/demo
.. _freertos smpdemo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/freertos/smpdemo
.. _ucosii demo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/ucosii/demo
.. _rt-thread demo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/rtthread/demo
.. _rt-thread demo smode application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/rtthread/demo_smode
.. _rt-thread msh application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/rtthread/msh
.. _threadx demo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/threadx/demo
.. _demo_smode_eclic application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_smode_eclic
.. _demo_smode_plic application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_smode_plic
.. _demo_sstc application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_sstc
.. _demo_spmp application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_spmp
.. _demo_smpu application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_smpu
.. _demo_pmp application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_pmp
.. _demo_profiling application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_profiling
.. _demo_cidu application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_cidu
.. _demo_cache application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_cache
.. _demo_stack_check application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_stack_check
.. _demo_pma application: https://github.com/Nuclei-Software/nuclei-sdk/tree/master/application/baremetal/demo_pma
.. _Nuclei User Extended Introduction: https://doc.nucleisys.com/nuclei_spec/isa/nice.html
