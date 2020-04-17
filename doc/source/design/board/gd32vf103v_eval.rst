.. _design_board_gd32vf103v_eval:

GD32VF103V Evaluation Kit
=========================

.. _design_board_gd32vf103v_eval_overview:

Overview
--------

This GD32VF103V Evaluation Kit is an evaluation board from gigadevice
using GD32VF103VBT6 as main MCU.

.. _figure_design_board_gd32vf103v_eval_1:

.. figure:: /asserts/images/gd32vf103v_eval_board.jpg
    :width: 80 %
    :align: center
    :alt: GD32VF103V-EVAL Board

    GD32VF103V-EVAL Board


If you want to learn about this board, please click `GD32VF103V EVAL Board Documents`_.


.. _design_board_gd32vf103v_eval_setup:

Setup
~~~~~

Follow the guide in `GD32VF103V EVAL Board Documents`_ to setup the board,
make sure the following items are set correctly:

* Connect the GD-Link on board to your PC in order to download and debug programs.
* Select the correct boot mode and then power on, the LEDPWR will turn on, which indicates the power supply is ready
* Connect the ``COM0`` to your PC
* Following steps in board user manual to setup JTAG drivers for your development environment

.. _design_board_gd32vf103v_eval_use:

How to use
~~~~~~~~~~

For **GD32VF103V-EVAL** board, the **DOWNLOAD** and **CORE** variables
are fixed to ``flashxip`` and ``n205``.

To run this application in GD32VF103V-EVAL board in Nuclei SDK,
you just need to use this **SOC** and **BOARD** variables.

.. code-block:: shell

    # Clean the application
    make SOC=gd32vf103 BOARD=gd32vf103v_eval clean
    # Build the application
    make SOC=gd32vf103 BOARD=gd32vf103v_eval all
    # Upload the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103v_eval upload
    # Debug the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103v_eval debug


.. _GD32VF103V EVAL Board Documents: https://github.com/riscv-mcu/GD32VF103_Demo_Suites/tree/master/GD32VF103V_EVAL_Demo_Suites/Docs
