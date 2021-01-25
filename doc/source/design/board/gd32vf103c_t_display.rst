.. _design_board_ttgo_t_display:

TTGO T-Display-GD32
===================

.. _design_board_ttgo_t_display_overview:

Overview
--------

The `TTGO T-Display-GD32`_ is a minimal board from LilyGo using the GD32VF103CBT6 as main MCU.

.. _figure_design_board_ttgo_t_display_1:

.. figure:: /asserts/images/ttgo_t_display_board.jpg
    :width: 80 %
    :align: center
    :alt: TTGO T-Display-GD32 Board

    TTGO T-Display-GD32 Board

.. _design_board_ttgo_t_display_setup:

Setup
-----

Wire your JTAG debugger as following. Below table assumes the Sipeed USB-JTAG/TTL RISC-V Debugger. With other brands the pin namings should be the same. You also need to power up the board via USB.

+----------+---------------------+
| Debugger | TTGO T-Display-GD32 |
+==========+=====================+
| GND      | GND                 |
+----------+---------------------+
| RXD      | PA9                 |
+----------+---------------------+
| TXD      | PA10                |
+----------+---------------------+
| NC       |                     |
+----------+---------------------+
| GND      | GND (optional)      |
+----------+---------------------+
| TDI      | PA15                |
+----------+---------------------+
| RST      | RST                 |
+----------+---------------------+
| TMS      | PA13                |
+----------+---------------------+
| TDO      | PB3                 |
+----------+---------------------+
| TCK      | PA14                |
+----------+---------------------+

.. _design_board_ttgo_t_display_usage:

How to use
----------

For **TTGO T-Display-GD32** board, the **DOWNLOAD** and **CORE** variables
are fixed to ``flashxip`` and ``n205``.

* You can find its linker script in ``SoC/gd32vf103/Board/gd32vf103c_t_display/Source/GCC/gcc_gd32vf103_flashxip.ld``
* If you want to specify your own modified linker script, you can follow steps described in :ref:`develop_appdev_linkscript`
* You can find its openocd configuration file in ``SoC/gd32vf103/Board/gd32vf103c_t_display/openocd_gd32vf103.cfg``

To run this application in TTGO T-Display-GD32 board in Nuclei SDK,
you just need to use this **SOC** and **BOARD** variables.

.. code-block:: shell

    # Clean the application
    make SOC=gd32vf103 BOARD=gd32vf103c_t_display clean
    # Build the application
    make SOC=gd32vf103 BOARD=gd32vf103c_t_display all
    # Upload the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103c_t_display upload
    # Debug the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103c_t_display debug

.. _TTGO T-Display-GD32: http://www.lilygo.cn/prod_view.aspx?TypeId=50033&Id=1251&FId=t3:50033:3
