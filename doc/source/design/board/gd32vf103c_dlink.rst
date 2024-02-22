.. _design_board_gd32vf103c_dlink:

GD32VF103C DLink Debugger
=========================

.. _design_board_gd32vf103c_dlink_overview:

Overview
--------

This GD32VF103C DLink Debugger is used to debug Nuclei RISC-V CPU from Nuclei
using **GD32VF103CVBT6** as main MCU.

.. _figure_design_board_gd32vf103c_dlink_1:

.. figure:: /asserts/images/gd32vf103c_dlink_board.jpg
    :width: 80 %
    :align: center
    :alt: GD32VF103C DLink Debugger Board

    GD32VF103C DLink Debugger Board

Click https://github.com/nuclei-Software/nuclei-dlink to learn more about
Nuclei DLink project.

.. _design_board_gd32vf103c_dlink_setup:

Setup
-----


.. _design_board_gd32vf103c_dlink_usage:

How to use
----------

For **GD32VF103C DLink Debugger** board, the **DOWNLOAD** and **CORE** variables
are fixed to ``flashxip`` and ``n205``.

* You can find its linker script in ``SoC/gd32vf103/Board/gd32vf103c_dlink/Source/GCC/``

  - ``gcc_gd32vf103_flashxip.ld``: Linker script file for ``DOWNLOAD=flashxip``

* If you want to specify your own modified linker script, you can follow steps described in :ref:`develop_appdev_linkscript`
* You can find its openocd configuration file in ``SoC/gd32vf103/Board/gd32vf103c_dlink/openocd_gd32vf103.cfg``

To run this application in GD32VF103C DLink Debugger board in Nuclei SDK,
you just need to use this **SOC** and **BOARD** variables.

.. code-block:: shell

    # Clean the application
    make SOC=gd32vf103 BOARD=gd32vf103c_dlink clean
    # Build the application
    make SOC=gd32vf103 BOARD=gd32vf103c_dlink all
    # Upload the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103c_dlink upload
    # Debug the application using openocd and gdb
    make SOC=gd32vf103 BOARD=gd32vf103c_dlink debug

