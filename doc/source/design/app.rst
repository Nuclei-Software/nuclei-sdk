.. _design_app:

Application
===========

.. _design_app_overview:

Overview
--------

In Nuclei N100 SDK, we just provided applications which can
run in different boards without any changes in code to
demonstrate the baremetal service, freertos service and
ucosii service features.

The provided applications can be divided into three categories:

* Bare-metal applications: Located in ``application/baremetal``

* FreeRTOS applications: Located in ``application/freertos``

* UCOSII applications: Located in ``application/ucosii``

* RTThread applications: Located in ``application/rtthread``

If you want to develop your own application in Nuclei N100 SDK, please click
:ref:`develop_appdev` to learn more about it.

The following applications are running Nuclei Eval SoC.

.. note::

    * Only benchmark/helloworld can run on Nuclei Qemu >= 2024.02 now.
    * Most of the application demostrated below using ``SOC=evalsoc``,
      you can easily change it to other SoC such as evalsoc by change it to
      ``SOC=evalsoc``
    * Some applications may not be able to be run on your SoC using Nuclei CPU
      due to lack of cpu feature required to run on it.
    * Almost all the applications required Nuclei CPU configured with
      irqc and System Timer hardware feature.
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

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the helloworld directory
    cd application/baremetal/helloworld
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Feb 21 2020, 12:24:22
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


.. _design_app_demo_timer:

demo_timer
~~~~~~~~~~

This `demo_timer application`_ is used to demonstrate how to use
the CORE TIMER API including the Timer Interrupt and Timer Software Interrupt.

* Both interrupts are registered as non-vector interrupt.
* First the timer interrupt will run for 10 times
* Then the software timer interrupt will start to run for 10 times

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the demo_timer directory
    cd application/baremetal/demo_timer
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Feb 21 2020, 12:52:37
    Download Mode: FLASHXIP
    CPU Frequency 108794117 Hz
    init timer and start
    MTimer IRQ handler 1
    MTimer IRQ handler 2
    MTimer IRQ handler 3
    MTimer IRQ handler 4
    MTimer IRQ handler 5
    MTimer IRQ handler 6
    MTimer IRQ handler 7
    MTimer IRQ handler 8
    MTimer IRQ handler 9
    MTimer IRQ handler 10
    MTimer SW IRQ handler 1
    MTimer SW IRQ handler 2
    MTimer SW IRQ handler 3
    MTimer SW IRQ handler 4
    MTimer SW IRQ handler 5
    MTimer SW IRQ handler 6
    MTimer SW IRQ handler 7
    MTimer SW IRQ handler 8
    MTimer SW IRQ handler 9
    MTimer SW IRQ handler 10
    MTimer msip and mtip interrupt test finish and pass

.. _design_app_demo_irqc:

demo_irqc
~~~~~~~~~

This `demo_irqc application`_ is used to demonstrate how to use
the irqc API and Interrupt.

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

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the demo_irqc directory
    cd application/baremetal/demo_irqc
    # Change macro SWIRQ_INTLEVEL_HIGHER value in demo_irqc.c
    # to see different working mode of this demo
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output(SWIRQ_INTLEVEL_HIGHER=1) as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Feb 21 2020, 16:35:58
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

    Nuclei N100 SDK Build Time: Feb 21 2020, 16:35:58
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

.. _design_app_demo_extirq:

demo_extirq
~~~~~~~~~~~

This `demo_extirq application`_ is used to demonstrate how to use low-power feature of RISC-V
processor.

Timer interrupt is setup before enter to wfi mode, and global interrupt will be disabled,
so interrupt handler will not be entered, and will directly resume to next pc of wfi.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # Assume your processor has enabled low-power feature
    # cd to the low-power directory
    cd application/baremetal/lowpower
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Jun  9 2022, 11:23:14
    Download Mode: ILM
    CPU Frequency 15996354 Hz
    CSV, WFI Start/End, 178264/178289
    CSV, WFI Cost, 25


.. _design_app_lowpower:

lowpower
~~~~~~~~

This `lowpower application`_ is used to demonstrate how to use low-power feature of RISC-V
processor.

Timer interrupt is setup before enter to wfi mode, and global interrupt will be disabled,
so interrupt handler will not be entered, and will directly resume to next pc of wfi.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # Assume your processor has enabled low-power feature
    # cd to the low-power directory
    cd application/baremetal/lowpower
    # Clean the application first
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 clean
    # Build and upload the application
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=ilm CORE=n300 upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Jun  9 2022, 11:23:14
    Download Mode: ILM
    CPU Frequency 15996354 Hz
    CSV, WFI Start/End, 178264/178289
    CSV, WFI Cost, 25


.. _design_app_coremark:

coremark
~~~~~~~~

This `coremark benchmark application`_ is used to run EEMBC CoreMark Software.

EEMBC CoreMark Software is a product of EEMBC and is provided under the terms of the
CoreMark License that is distributed with the official EEMBC COREMARK Software release.
If you received this EEMBC CoreMark Software without the accompanying CoreMark License,
you must discontinue use and download the official release from www.coremark.org.

In Nuclei N100 SDK, we provided code and Makefile for this ``coremark`` application.
You can also optimize the ``COMMON_FLAGS`` defined in coremark application Makefile
to get different score number.

* By default, this application runs for 15 iterations, you can also change this in Makefile.
  e.g. Change this ``-DITERATIONS=15`` to value such as ``-DITERATIONS=20``
* macro **PERFORMANCE_RUN=1** is defined
* **STDCLIB ?= newlib_small** is added in its Makefile to enable float value print

.. note::

    N100's time and cycle counter is only 24bit, easy to overflow, so make sure
    the iteration is small.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the coremark directory
    cd application/baremetal/benchmark/coremark
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Mar 30 2020, 18:08:53
    Download Mode: FLASHXIP
    CPU Frequency 107190000 Hz
    Start to run coremark for 5000 iterations
    2K performance run parameters for coremark.
    CoreMark Size    : 666
    Total ticks      : 1622809457
    Total time (secs): 15.139593
    Iterations/Sec   : 330.259868
    Iterations       : 5000
    Compiler version : GCC9.2.0
    Compiler flags   : -O2 -flto -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting -fno-common -funroll-loops -finline-functions -falign-functions=4 -falign-jumps=4 -falign-loops=4
    Memory location  : STACK
    seedcrc          : 0xe9f5
    [0]crclist       : 0xe714
    [0]crcmatrix     : 0x1fd7
    [0]crcstate      : 0x8e3a
    [0]crcfinal      : 0xbd59
    Correct operation validated. See readme.txt for run and reporting rules.
    CoreMark 1.0 : 330.259868 / GCC9.2.0 -O2 -flto -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting -fno-common -funroll-loops -finline-functions -falign-functions=4 -falign-jumps=4 -falign-loops=4 / STACK


    Print Personal Added Addtional Info to Easy Visual Analysis

        (Iterations is: 5000
        (total_ticks is: 1622809457
    (*) Assume the core running at 1 MHz
        So the CoreMark/MHz can be caculated by:
        (Iterations*1000000/total_ticks) = 3.081076 CoreMark/MHz

.. _design_app_dhrystone:

dhrystone
~~~~~~~~~

This `dhrystone benchmark application`_ is used to run DHRYSTONE Benchmark Software.

The Dhrystone benchmark program has become a popular benchmark for CPU/compiler performance measurement,
in particular in the area of minicomputers, workstations, PC's and microprocesors.

* It apparently satisfies a need for an easy-to-use integer benchmark;
* it gives a first performance indication which is more meaningful than MIPS numbers which,
  in their literal meaning (million instructions per second), cannot be used across different
  instruction sets (e.g. RISC vs. CISC).
* With the increasing use of the benchmark, it seems necessary to reconsider the benchmark and
  to check whether it can still fulfill this function.

In Nuclei N100 SDK, we provided code and Makefile for this ``dhrystone`` application.
You can also optimize the ``COMMON_FLAGS`` defined in dhrystone application Makefile
to get different score number.

* **STDCLIB ?= newlib_small** is added in its Makefile to enable float value print

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the dhrystone directory
    cd application/baremetal/benchmark/dhrystone
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Feb 21 2020, 14:23:55
    Download Mode: FLASHXIP
    CPU Frequency 108801980 Hz

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
      Ptr_Comp:          536883352
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
      Ptr_Comp:          536883352
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
    223500116
           So the DMIPS/MHz can be caculated by:
           1000000/(User_Cycle/Number_Of_Runs)/1757 = 1.273270 DMIPS/MHz

.. _design_app_whetstone:

whetstone
~~~~~~~~~

This `whetstone benchmark application`_ is used to run C/C++ Whetstone Benchmark Software
(Single or Double Precision).

The Fortran Whetstone programs were the first general purpose benchmarks that set industry
standards of computer system performance. Whetstone programs also addressed the question
of the efficiency of different programming languages, an important issue not covered by
more contemporary standard benchmarks.

In Nuclei N100 SDK, we provided code and Makefile for this ``whetstone`` application.
You can also optimize the ``COMMON_FLAGS`` defined in whetstone application Makefile
to get different score number.

* **STDCLIB ?= newlib_small** is added in its Makefile to enable float value print
* Extra **LDFLAGS := -lm** is added in its Makefile to include the math library


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the whetstone directory
    cd application/baremetal/benchmark/whetstone
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Feb 21 2020, 14:50:15
    Download Mode: FLASHXIP
    CPU Frequency 109069306 Hz

    ##########################################
    Single Precision C Whetstone Benchmark Opt 3 32 Bit
    Calibrate
           1.96 Seconds          1   Passes (x 100)
           9.81 Seconds          5   Passes (x 100)

    Use 5  passes (x 100)

              Single Precision C/C++ Whetstone Benchmark

    Loop content                  Result              MFLOPS      MOPS   Seconds

    N1 floating point -1.12475013732910156         1.053              0.091
    N2 floating point -1.12274742126464844         1.053              0.638
    N3 if then else    1.00000000000000000               108527.617    0.000
    N4 fixed point    12.00000000000000000                   5.630    0.280
    N5 sin,cos etc.    0.49909299612045288                   0.109    3.829
    N6 floating point  0.99999982118606567         1.082              2.493
    N7 assignments     3.00000000000000000                 419.794    0.002
    N8 exp,sqrt etc.   0.75110614299774170                   0.075    2.492

    MWIPS                                              5.089              9.825


    MWIPS/MHz                                          0.046              9.825



FreeRTOS applications
---------------------

.. _design_app_freertos_demo:

demo
~~~~

This `freertos demo application`_ is to show basic freertos task functions.

* Two freertos tasks are created
* A software timer is created

In Nuclei N100 SDK, we provided code and Makefile for this ``freertos demo`` application.

* **RTOS = FreeRTOS** is added in its Makefile to include FreeRTOS service
* The **configTICK_RATE_HZ** in ``FreeRTOSConfig.h`` is set to 100, you can change it
  to other number according to your requirement.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the freertos demo directory
    cd application/freertos/demo
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Feb 21 2020, 14:56:00
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


UCOSII applications
-------------------

.. _design_app_ucosii_demo:

demo
~~~~

This `ucosii demo application`_ is show basic ucosii task functions.

* 4 tasks are created
* 1 task is created first, and then create 3 other tasks and then suspend itself

In Nuclei N100 SDK, we provided code and Makefile for this ``ucosii demo`` application.

* **RTOS = UCOSII** is added in its Makefile to include UCOSII service
* The **OS_TICKS_PER_SEC** in ``os_cfg.h`` is by default set to 50, you can change it
  to other number according to your requirement.

.. note:

   * For Nuclei N100 SDK release > v0.2.2, the UCOSII source code is replaced using the
     version from https://github.com/SiliconLabs/uC-OS2/, and application development
     for UCOSII is also changed, the ``app_cfg.h``, ``os_cfg.h`` and ``app_hooks.c`` files
     are required in application source code.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the ucosii demo directory
    cd application/ucosii/demo
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Feb 21 2020, 15:00:35
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

In Nuclei N100 SDK, we provided code and Makefile for this ``rtthread demo`` application.

* **RTOS = RTThread** is added in its Makefile to include RT-Thread service
* The **RT_TICK_PER_SECOND** in ``rtconfig.h`` is by default set to `100`, you can change it
  to other number according to your requirement.


**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the rtthread demo directory
    cd application/rtthread/demo
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Apr 14 2020, 10:14:30
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

In Nuclei N100 SDK, we provided code and Makefile for this ``rtthread msh`` application.

* **RTOS = RTThread** is added in its Makefile to include RT-Thread service
* **RTTHREAD_MSH := 1** is added in its Makefile to include RT-Thread msh component
* The **RT_TICK_PER_SECOND** in ``rtconfig.h`` is by default set to `100`, you can change it
  to other number according to your requirement.
* To run this application in :ref:`design_soc_evalsoc`, the SoC clock frequency must be above 16MHz,
  if run in 8MHz, uart read is not correct due to bit error in uart rx process.

**How to run this application:**

.. code-block:: shell

    # Assume that you can set up the Tools and Nuclei N100 SDK environment
    # cd to the rtthread msh directory
    cd application/rtthread/msh
    # Clean the application first
    make SOC=evalsoc clean
    # Build and upload the application
    make SOC=evalsoc upload

**Expected output as below:**

.. code-block:: console

    Nuclei N100 SDK Build Time: Dec 23 2020, 16:39:21
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
    Hello Nuclei N100 SDK!
    msh >


.. _helloworld application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/baremetal/helloworld
.. _demo_timer application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/baremetal/demo_timer
.. _demo_irqc application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/baremetal/demo_irqc
.. _demo_extirq application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/baremetal/demo_extirq
.. _lowpower application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/baremetal/lowpower
.. _coremark benchmark application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/baremetal/benchmark/coremark
.. _dhrystone benchmark application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/baremetal/benchmark/dhrystone
.. _whetstone benchmark application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/baremetal/benchmark/whetstone
.. _freertos demo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/freertos/demo
.. _ucosii demo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/ucosii/demo
.. _rt-thread demo application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/rtthread/demo
.. _rt-thread msh application: https://github.com/Nuclei-Software/nuclei-sdk/tree/develop_n100/application/rtthread/msh
