.. _design_rtos:

RTOS
====

.. _design_rtos_overview:

Overview
--------

In Nuclei SDK, we have support two used RTOSes in the world,
one is **FreeRTOS**, and another one is **UCOSII**.

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
is initialized as vector interrupt.

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

.. note::

    You can check the ``application\freertos\demo`` for reference

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

In Nuclei SDK, if you want to use **UCOSII** in your application, you need
to add ``RTOS = UCOSII`` in your application Makefile.

And in your application code, you need to do the following things:

* Add UCOSII application configuration file -> ``app_cfg.h``
* Include UCOSII header files

.. note::

    You can check the ``application\ucosii\demo`` for reference


.. _FreeRTOS: https://www.freertos.org/
.. _UCOSII: https://www.micrium.com/
