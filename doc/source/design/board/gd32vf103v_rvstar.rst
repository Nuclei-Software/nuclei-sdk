.. _design_board_gd32vf103v_rvstar:

GD32VF103V RV-STAR Kit
======================

.. _design_board_gd32vf103v_rvstar_overview:

Overview
--------

This GD32VF103V RV-STAR Kit is an arduino compatiable board from Nuclei
using GD32VF103VBT6 as main MCU.

.. _figure_design_board_gd32vf103v_rvstar_1:

.. figure:: /asserts/images/gd32vf103v_rvstar_board.jpg
    :width: 80 %
    :align: center
    :alt: GD32VF103V RV-STAR Board

    GD32VF103V RV-STAR Board

Click `GD32VF103V RV-STAR Development Kit`_ to access the documents
of this board.

Click online `RV-STAR Development Board Overview`_ to get basic information
of this board.

.. _design_board_gd32vf103v_rvstar_setup:

Setup
~~~~~

Follow the guide in `GD32VF103V RV-STAR Development Kit`_ to setup the board,
make sure the following items are set correctly:

* Connect the USB Type-C port on board to your PC in order to download and
  debug programs and monitor the UART message.
* Following steps in board user manual to setup JTAG drivers for your development environment

.. _design_board_gd32vf103v_rvstar_usage:

How to use
~~~~~~~~~~

For **GD32VF103V RV-STAR** board, the **DOWNLOAD** and **CORE** variables
are fixed to ``flashxip`` and ``n205``.

You can find its linker script in ``SoC/gd32vf103/Board/gd32vf103v_rvstar/Source/GCC/``

.. note::

    There are now two versions of RV-STAR boards, we use **BDVER** to distinguish
    board version, for RV-STAR board, default **BDVER** is ``v2``.

    * **BDVER=v1**: This is the first version of RV-STAR board, which is sold out by 2020.09.30.
      It will choose the openocd configuration file located in
      *SoC/gd32vf103/Board/gd32vf103v_rvstar/openocd_gd32vf103.cfg*
    * **BDVER=v2**: This is the second version, and it has a extra chip placed in **U3** which
      contains the on-board debugger driver, when using with **Nuclei OpenOCD >= 0.10.0-15**,
      there is no need for user to install driver using **HBird_Driver.exe**.
      It will choose the openocd configuration file located in
      *SoC/gd32vf103/Board/gd32vf103v_rvstar/openocd_gd32vf103_v2.cfg*

To run this application in GD32VF103V RV-STAR board in Nuclei SDK,
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

If you want to run for RV-STAR Board V1 (**BDVER=v1**), just change
**BDVER ?= v2** to **BDVER ?= v1** in *Build/Makefile.soc.gd32vf103* or pass
extra **BDVER=v1** in every command you run above.


.. _GD32VF103V RV-STAR Development Kit: https://nucleisys.com/developboard.php
.. _RV-STAR Development Board Overview: https://doc.nucleisys.com/nuclei_board_labs/hw/hw.html#rv-star
