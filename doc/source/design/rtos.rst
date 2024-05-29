.. _design_rtos:

RTOS
====

.. _design_rtos_overview:

Overview
--------

In Nuclei SDK, we have support four most-used RTOSes in the world,
**FreeRTOS**, **UCOSII**, **ThreadX** and **RT-Thread from China**.

Our RTOS port require Nuclei ECLIC interrupt controller, please make sure
your Nuclei CPU is configured with ECLIC present.

If you want to use RTOS in your application, you can choose one
of the supported RTOSes.

.. note::

    When you want to develop RTOS application in Nuclei SDK, please
    don't reconfigure ``SysTimer`` and ``SysTimer Software Interrupt``,
    since it is already used by RTOS portable code.

.. _design_rtos_freertos:

FreeRTOS
--------

`FreeRTOS`_ is a market-leading real-time operating system (RTOS) for
microcontrollers and small microprocessors.

In our FreeRTOS portable code, we are using ``SysTimer Interrupt`` as RTOS SysTick
Interrupt, and using ``SysTimer Software Interrupt`` to do task switch.

These two interrupts are kept as lowest level, and ``SysTimer Interrupt``
is initialized as non-vector interrupt, and ``SysTimer Software Interrupt``
is initialized as vector interrupt and interrupt handler implemented using asm code.

In our FreeRTOS porting, we also allow FreeRTOS configuration variable
``configMAX_SYSCALL_INTERRUPT_PRIORITY`` which can be find in https://www.freertos.org/a00110.html.

The ``configMAX_SYSCALL_INTERRUPT_PRIORITY`` should be set to be a
absolute interrupt level range from 1 to (2^lvlbits-1) while ``lvlbits = min(nlbits, CLICINTCTLBITS)``.

If you set configMAX_SYSCALL_INTERRUPT_PRIORITY to value above the accepted
value range, it will use the max value.

If you want to learn about how to use FreeRTOS APIs, you need to go to
its website to learn the FreeRTOS documentation in its website.

In Nuclei SDK, if you want to use **FreeRTOS** in your application, you need
to add ``RTOS = FreeRTOS`` in your application Makefile.

And in your application code, you need to do the following things:

* Add FreeRTOS configuration file -> ``FreeRTOSConfig.h``
* Include FreeRTOS header files

Now we also support FreeRTOS SMP version, about SMP version, please refer to
https://www.freertos.org/symmetric-multiprocessing-introduction.html , and we also
provide freertos smpdemo example in our SDK, you can find it in
``application\freertos\smpdemo``.

To use FreeRTOS SMP version for 2 Core SMP CPU, you need to add ``SMP = 2`` in your application Makefile.
And also you need to make sure your application code is placed and run on shared memory which can be
accessed by both CPUs. When ``SMP=2`` is specified, it will define extra requried macro called ``configNUMBER_OF_CORES``,
for details, please check ``OS/FreeRTOS/build.mk``.

.. note::

    * You can check the ``application\freertos\`` for freertos application reference
    * From Nuclei SDK 0.6.0, we introduced FreeRTOS SMP support, both Nuclei RV32 and RV64 processors are supported.
    * Current version of FreeRTOS used in Nuclei SDK is ``V11.1.0``
    * If you want to change the OS ticks per seconds, you can change the ``configTICK_RATE_HZ``
      defined in ``FreeRTOSConfig.h``

More information about FreeRTOS get started, please click
https://www.freertos.org/FreeRTOS-quick-start-guide.html

.. _design_rtos_ucosii:

UCOSII
------

`UCOSII`_ a priority-based preemptive real-time kernel for microprocessors,
written mostly in the programming language C. It is intended for use in embedded systems.

In our UCOSII portable code, we are using ``SysTimer Interrupt`` as RTOS SysTick
Interrupt, and using ``SysTimer Software Interrupt`` to do task switch.

If you want to learn about ``UCOSII``, please click https://www.micrium.com/books/ucosii/

We are using the opensource version of UC-OS2 source code from https://github.com/SiliconLabs/uC-OS2,
with optimized code for Nuclei RISC-V processors.

In Nuclei SDK, if you want to use **UCOSII** in your application, you need
to add ``RTOS = UCOSII`` in your application Makefile.

And in your application code, you need to do the following things:

* Add UCOSII application configuration header file -> ``app_cfg.h`` and ``os_cfg.h``
* Add application hook source file -> ``app_hooks.c``
* Include UCOSII header files

.. note::

    * You can check the ``application\ucosii\`` for ucosii application reference
    * The UCOS-II application configuration template files can also be found in
      https://github.com/SiliconLabs/uC-OS2/tree/master/Cfg/Template
    * Current version of UCOSII used in Nuclei SDK is ``V2.93.00``
    * If you want to change the OS ticks per seconds, you can change the ``OS_TICKS_PER_SEC``
      defined in ``os_cfg.h``


.. warning::

   * For Nuclei SDK release > v0.2.2, the UCOSII source code is replaced using the
     version from https://github.com/SiliconLabs/uC-OS2/, and application development
     for UCOSII is also changed, the ``app_cfg.h``, ``os_cfg.h`` and ``app_hooks.c`` files
     are required in application source code.


.. _design_rtos_rtthread:

RT-Thread
---------

`RT-Thread`_ was born in 2006, it is an open source, neutral,
and community-based real-time operating system (RTOS).

RT-Thread is mainly written in C language, easy to understand and easy
to port(can be quickly port to a wide range of mainstream MCUs and module chips).

It applies object-oriented programming methods to real-time system design,
making the code elegant, structured, modular, and very tailorable.

In our support for RT-Thread, we get the source code of RT-Thread from a project
called `RT-Thread Nano`_, which only provide kernel code of RT-Thread, which is easy
to be intergated with Nuclei SDK.

In our RT-Thread portable code, we are using ``SysTimer Interrupt`` as RTOS SysTick
Interrupt, and using ``SysTimer Software Interrupt`` to do task switch.

And also the ``rt_hw_board_init`` function is implemented in our portable code.

If you want to learn about ``RT-Thread``, please click:

* For Chinese version, click https://www.rt-thread.org/document/site/
* For English version, click https://github.com/RT-Thread/rt-thread#documentation

In Nuclei SDK, if you want to use **RT-Thread** in your application, you need
to add ``RTOS = RTThread`` in your application Makefile.

And in your application code, you need to do the following things:

* Add RT-Thread application configuration header file -> ``rtconfig.h``
* Include RT-Thread header files
* If you want to enable RT-Thread MSH feature, just add ``RTTHREAD_MSH := 1`` in
  your application Makefile.

.. note::

    * You can check the ``application\rtthread\`` for rtthread application reference
    * In RT-Thread, the ``main`` function is created as a RT-Thread thread,
      so you don't need to do any OS initialization work, it is done before ``main``
    * We also provide good support directly through RT-Thread official repo,
      you can check Nuclei processor support for RT-Thread in `RT-Thread BSP For Nuclei`_.

.. _design_rtos_threadx:

ThreadX
-------

`Eclipse ThreadX`_ offers a vendor-neutral, open source, safety certified OS for real-time applications,
all under a permissive license. It stands alone as the first and only RTOS with this unique blend of
attributes to meet a wide range of needs that will benefit industry adopters, developers and end users alike.

Microsoft has contributed the Azure RTOS technology to the Eclipse Foundation.
With the Eclipse Foundation as its new home, Azure RTOS now becomes Eclipse ThreadX – an advanced embedded
development suite including a small but powerful operating system that provides reliable, ultra-fast performance
for resource-constrained devices.

ThreadX is IEC 61508, IEC 62304, ISO 26262, and EN 50128 conformance certified by SGS-TÜV Saar.
ThreadX has also achieved EAL4+ Common Criteria security certification.
These certifications are a big differentiator, and are unprecedented in the industry.
They are a game changer, as there are currently no open source RTOS's which have them.

In our ThreadX portable code, we are using ``SysTimer Interrupt`` as RTOS SysTick
Interrupt, and using ``SysTimer Software Interrupt`` to do task switch.

If you want to learn about ``Eclipse ThreadX``, please click:

* For introduction of Eclipse ThreadX, click https://eclipse-foundation.blog/2023/11/21/introducing-eclipse-threadx/
* For ThreadX documentation, click https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/threadx/index.md

In Nuclei SDK, if you want to use **ThreadX** in your application, you need
to add ``RTOS = ThreadX`` in your application Makefile.

And in your application code, you need to do the following things:

* Add ThreadX application configuration header file -> ``tx_user.h``
* Include ThreadX header files

.. note::

    * You can check the ``application\threadx\`` for threadx application reference
    * Currently we only support single core version, the SMP version is not yet supported.

.. _FreeRTOS: https://www.freertos.org/
.. _UCOSII: https://www.micrium.com/
.. _RT_Thread: https://www.rt-thread.org/
.. _RT-Thread Nano: https://github.com/RT-Thread/rtthread-nano
.. _Eclipse ThreadX: https://github.com/eclipse-threadx/threadx
.. _RT-Thread BSP For Nuclei: https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuclei/
