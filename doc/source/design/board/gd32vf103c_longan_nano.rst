.. _design_board_sipeed_longan_nano:

Sipeed Longan Nano
======================

.. _design_board_sipeed_longan_nano_overview:

Overview
--------

The Sipeed Longan Nano is a board made by Sipeed using a GD32VF103CBT6 as main MCU.
Is is similar to the well known STM32-based **Blue Pill** board.

.. _figure_design_sipeed_longan_nano_1:

.. figure:: /asserts/images/sipeed_longan_nano_board.png
    :width: 80 %
    :align: center
    :alt: Sipeed Longan Nano Board

    Sipeed Longan Nano Board.


.. _design_board_sipeed_longan_nano_versions:

Versions
--------

There are two versions of this board available.

* GD32VF103C\ **B**\ T6 with 128k Flash / 32k RAM

* GD32VF103C\ **8**\ T6 with 64k Flash / 20k RAM. This is sometimes called the **lite** version.

If you want to buy one, carefully take a look at the description because sometimes they are offered
with the GD32VF103CB controller, but they only contain the GD32VF103C8 controller.


.. _design_board_sipeed_longan_nano_pinout:

Pinout
------

The pinout of Sipeed Logan Nano is shown in the following picture

.. _figure_design_sipeed_longan_nano_2:

.. figure:: /asserts/images/sipeed_longan_nano_pinout.jpg
    :width: 80 %
    :align: center
    :alt: Sipeed Longan Nano Pinout

    Sipeed Longan Nano Pinout.

.. _design_board_sipeed_longan_nano_schematic:

Schematic
---------

.. _figure_design_sipeed_longan_nano_3:

.. figure:: /asserts/images/sipeed_longan_nano_schematic.png
    :width: 80 %
    :align: center
    :alt: Sipeed Longan Nano Schematic

    Sipeed Longan Nano Schematic.

.. _design_board_sipeed_longan_nano_resources:

Resources
---------

Click `Sipeed Longan Nano Documentation`_ to get all information about this board from Sipeed website.

.. _design_board_sipeed_longan_nano_setup:

Setup
-----

To setup the board, make sure the following items are set correctly:

* Power up the board by  either the USB-C port **or** the by the debugger.

* The default serial port is USART0, whitch is also available at the debug header. See :ref:`figure_design_sipeed_longan_nano_2`

.. _design_board_sipeed_longan_nano_usage:

How to use
----------

For **Sipeed Longan Nano** board, the **DOWNLOAD** and **CORE** variables
are fixed to ``flashxip`` and ``n205``. The **VARIANT** variable can be used for choosing a board variant.

* You can find its linker scripts in ``SoC/gd32vf103/Board/gd32vf103c_longan_nano/Source/GCC/``

  - ``gcc_gd32vf103xb_flashxip.ld``: Linker script file for ``DOWNLOAD=flashxip`` and 128k flash, this is the default.
  - ``gcc_gd32vf103x8_flashxip.ld``: Linker script file for ``DOWNLOAD=flashxip`` and 64k flash, the **lite** version,
    you can pass extra ``VARIANT=lite`` via make command to select this linker script.

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

To build for the **lite** variant you also need to set the **VARIANT** variable.

.. code-block:: shell

    # Build the application
    make SOC=gd32vf103 BOARD=gd32vf103c_longan_nano VARIANT=lite all

.. _design_board_sipeed_longan_nano_extensions:

Extensions
----------

There are three extensions on the board:

* On the back of the circuit board there is a socket for a micro SD card.

  - The SD-card is connected to SPI1.

* On the front there is a socket for a small LCD which is offered by some sellers.

  - The LCD is connected to SPI0.

  - The controller on the LCD is similar to Sitronix' ST7735.

* One RGB-LED

  - The red LED is controlled via PC13. This LED can be addressed by LED3 or LEDR.

  - The green LED is controlled via PA1. This LED can be addressed by LED1 or LEDG.

  - The blue LED is controlled via PA2 This LED can be addressed by LED2 or LEDB.

There are two buttons on the board. One is the reset button and the other is to activate the internal bootloader. Unfortunately, none of these buttons can be used as user inputs.

.. _Sipeed Longan Nano Documentation: https://longan.sipeed.com/en/
