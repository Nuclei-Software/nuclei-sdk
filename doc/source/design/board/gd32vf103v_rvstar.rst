.. _design_board_gd32vf103v_rvstar:

GD32VF103V RVStar Kit
=====================

.. _design_board_gd32vf103v_rvstar_overview:

Overview
--------

This GD32VF103V RVStar Kit is an arduino compatiable board from Nuclei
using GD32VF103VBT6 as main MCU.

.. _figure_design_board_gd32vf103v_rvstar_1:

.. figure:: /asserts/images/gd32vf103v_rvstar_board.jpg
    :width: 80 %
    :align: center
    :alt: GD32VF103V RVStar Board

    GD32VF103V RVStar Board

Click `GD32VF103V RVStar Development Kit`_ to access the documents
of this board.

.. _design_board_gd32vf103v_rvstar_setup:

Setup
~~~~~

Follow the guide in `GD32VF103V RVStar Development Kit`_ to setup the board,
make sure the following items are set correctly:

* Connect the USB Type-C port on board to your PC in order to download and
  debug programs and monitor the UART message.
* Following steps in board user manual to setup JTAG drivers for your development environment

.. _design_board_gd32vf103v_rvstar_usage:

How to use
~~~~~~~~~~

For **GD32VF103V-RVSTAR** board, the :**DOWNLOAD**: and **CORE** variables
are fixed to ``flashxip`` and ``n205``.

To run this application in GD32VF103V-RVSTAR board in Nuclei SDK,
you just need to use this **SOC** and **BOARD** variables.

.. code-block:: shell

    # Clean the application
    make SOC=gd32vf103 BOARD=gd32vf103v_rvstar clean
    # Build the application
    make SOC=gd32vf103 BOARD=gd32vf103v_rvstar all
    # Upload the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103v_rvstar upload
    # Debug the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103v_rvstar debug


.. _GD32VF103V RVStar Development Kit: https://nucleisys.com/developboard.php
