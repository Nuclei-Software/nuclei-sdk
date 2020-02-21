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

.. _design_rtos_freertos:

FreeRTOS
--------

`FreeRTOS`_ is a market-leading real-time operating system (RTOS) for
microcontrollers and small microprocessors.

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
