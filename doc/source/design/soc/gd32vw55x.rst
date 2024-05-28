.. _design_soc_gd32vw55x:

GD32VW55x SoC
=============

GD32VW55x SoC is an RISC-V WiFi/BLE MCU from `GigaDevice Semiconductor`_
in the world which is based on Nuclei RISC-V N300 Processor.

If you want to learn more about it, please click
https://www.gigadevice.com/about/news-and-event/news/gigadevice-launches-gd32vw553-series

.. _design_soc_gd32vw55x_overview:

Overview
--------

The new GD32VW553 series integrates up to 4MB Flash, 320KB SRAM, and 32KB configurable
Instruction Cache (I-Cache) to greatly improve CPU processing efficiency.
The GD32VW553, delivering excellent wireless performance, is also equipped with
rich universal wired interfaces, including three U(S)ART, two I2C, one SPI, one four-wire QSPI,
and up to 29 programmable GPIO pins. Its built-in components include two 32-bit general-purpose timers,
two 16-bit general-purpose timers, four 16-bit basic timers, one PWM advanced timer,
and one 12-bit ADC. The power supply voltage ranges from 1.8 V to 3.6 V and it offers high temperature
up to 105℃ to meet the application scenarios such as industrial control interconnection,
lighting equipment, and socket panels.


.. _design_soc_gd32vw55x_boards:

Supported Boards
----------------

In Nuclei SDK, we support the following four boards based on **GD32VW55x** SoC, see:

* :ref:`design_board_gd32vw553h_eval`, default Board when this SoC selected.

.. _design_soc_gd32vw55x_usage:

Usage
-----

If you want to use this **GD32VW55x** SoC in Nuclei SDK, you need to set the
:ref:`develop_buildsystem_var_soc` Makefile variable to ``gd32vw55x``.

Extra make variables supported only in this SoC(see ``SoC/gd32vw55x/build.mk``):
  * **SYSCLK**: ``160000000`` by default, means 160MHz system clock will be selected during ``SystemInit`` function,
    it will define macro ``SYSTEM_CLOCK=$(SYSCLK)`` which is used in ``system_gd32vw55x.c``.
  * **CLKSRC**: empty by default, available choices are ``hxtal`` and ``irc16m``, means select to use HXTAL PLL
    or IRC16M PLL, it will define macro ``CLOCK_USING_$(CLKSRC)``, such as ``CLOCK_USING_HXTAL``

.. code-block:: shell

    # Choose SoC to be gd32vw55x
    # the following command will build application
    # using default gd32vw55x SoC based board
    # defined in Build System and application Makefile
    make SOC=gd32vw55x clean
    make SOC=gd32vw55x all

.. note::

    * Since this ``gd32vw55x`` SoC is a real chip, it is using Nuclei RISC-V
      N300 core, so the **CORE** is fixed to ``n307fd``

.. _GigaDevice Semiconductor: https://www.gigadevice.com/
