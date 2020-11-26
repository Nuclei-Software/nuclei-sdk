.. _design_board_sipeed_longan_nano:

Sipeed Longan Nano
======================

.. _design_board_sipeed_longan_nano_overview:

Overview
--------

The Sipeed Longan Nano is a board made by Sipeed using a GD32VF103CBT6 as main MCU.
Is is similar to the well known STM32-based "Blue Pill" board.

.. _figure_design_sipeed_longan_nano_1:

.. figure:: /asserts/images/sipeed_longan_nano_board.png
    :width: 80 %
    :align: center
    :alt: Sipeed Longan Nano Board

    Sipeed Longan Nano Board.

.. _design_board_sipeed_longan_nano_pinout:

Pinout
~~~~~~

The pinout of Sipeeds Logan Nano is shown in the following picture

.. _figure_design_sipeed_longan_nano_2:

.. figure:: /asserts/images/sipeed_longan_nano_pinout.jpg
    :width: 80 %
    :align: center
    :alt: Sipeed Longan Nano Pinout

    Sipeed Longan Nano Pinout.

.. _design_board_sipeed_longan_nano_schematic:

Schematic
~~~~~~~~~

.. _figure_design_sipeed_longan_nano_3:

.. figure:: /asserts/images/sipeed_longan_nano_schematic.png
    :width: 80 %
    :align: center
    :alt: Sipeed Longan Nano Schematic

    Sipeed Longan Nano Schematic.

.. _design_board_sipeed_longan_nano_resources:

Resources
~~~~~~~~~

Click `Sipeed Longan Nano Documentation`_ to get all information about this board from Sipeeds website.

.. _design_board_sipeed_longan_nano_setup:

Setup
~~~~~

To setup the board, make sure the following items are set correctly:

* Power up the board by  either the USB-C port **or** the by the debugger.

* The default serial port is USART0, whitch is also available at the debug header. See :ref:`figure_design_sipeed_longan_nano_2`

.. _design_board_sipeed_longan_nano_usage:

How to use
~~~~~~~~~~

For **Sipeed Longan Nano** board, the **DOWNLOAD** and **CORE** variables
are fixed to ``flashxip`` and ``n205``.

* There are two versions of this board avail. One with 64k flash and one with 128k flash.

  - You have to change the linker script to set the right flash size. By default, 128k flash is set.
  
* You can find its linker script in ``SoC/gd32vf103/Board/gd32vf103c_longan_nano/Source/GCC/``

  - ``gcc_gd32vf103_flashxip.ld``: Linker script file for ``DOWNLOAD=flashxip``

* If you want to specify your own modified linker script, you can follow steps described in :ref:`develop_appdev_linkscript`

* You can find its openocd configuration file in ``SoC/gd32vf103/Board/gd32vf103c_longan_nano/openocd_gd32vf103.cfg``

To run this application in Sipeed Longan Nano board in Nuclei SDK, you just need to use this **SOC** and **BOARD** variables.

.. code-block:: shell

    # Clean the application
    make SOC=gd32vf103 BOARD=gd32vf103c_longan_nano clean
    # Build the application
    make SOC=gd32vf103 BOARD=gd32vf103c_longan_nano all
    # Upload the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103c_longan_nano upload
    # Debug the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103c_longan_nano debug

.. _Sipeed Longan Nano Documentation: https://longan.sipeed.com/en/
