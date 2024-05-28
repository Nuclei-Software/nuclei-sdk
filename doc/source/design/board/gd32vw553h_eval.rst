.. _design_board_gd32vw553h_eval:

GD32VW553H Evaluation Kit
=========================

.. _design_board_gd32vw553h_eval_overview:

Overview
--------

This GD32VW553H Evaluation Kit is an evaluation board from gigadevice
using GD32VW553HM as main MCU.

If you want to learn about this board, please click `GD32VW553H EVAL Board Documents`_.


.. _design_board_gd32vw553h_eval_setup:

Setup
-----

Follow the guide in `GD32VW553H EVAL Board Documents`_ to setup the board,
make sure the following items are set correctly:

* Connect the GD-Link on board to your PC in order to download and debug programs.
* Connect the ``USART`` to your PC as UART communication.
* Following steps in board user manual to setup JTAG drivers for your development environment

.. _design_board_gd32vw553h_eval_use:

How to use
----------

For **GD32VW553H-EVAL** board:

* **DOWNLOAD**: ``flashxip`` by default, and you can also choose ``sram`` download mode

  - You can find its linker script in ``SoC/gd32vw55x/Board/gd32vw553h_eval/Source/GCC/``
  - ``gcc_gd32vw55x_flashxip.ld``: Linker script file for ``DOWNLOAD=flashxip``
  - ``gcc_gd32vw55x_sram.ld``: Linker script file for ``DOWNLOAD=sram``
  - If you want to specify your own modified linker script, you can follow steps described in :ref:`develop_appdev_linkscript`

* **CORE**: ``n307fd`` by default, this by default is ``rv32imafdc`` arch, but you can also choose ``n300`` or ``n300f``
* **ARCH_EXT**: ``_zba_zbb_zbc_zbs_xxldspn1x`` by default, you can pass less extensions such as ``_zba_zbb_zbc_zbs``

* You can find its openocd configuration file in ``SoC/gd32vw55x/Board/gd32vw553h_eval/openocd_gd32vw55x.cfg``

To run this application in GD32VW553H-EVAL board in Nuclei SDK,
you just need to use this **SOC** and **BOARD** variables.

.. code-block:: shell

    # Clean the application
    make SOC=gd32vw55x BOARD=gd32vw553h_eval clean
    # Build the application
    make SOC=gd32vw55x BOARD=gd32vw553h_eval all
    # Upload the application using openocd and gdb
    make SOC=gd32vw55x BOARD=gd32vw553h_eval upload
    # Debug the application using openocd and gdb
    make SOC=gd32vw55x BOARD=gd32vw553h_eval debug


.. _GD32VW553H EVAL Board Documents: https://www.gd32mcu.com/en/download/8?kw=GD32VW5
