.. _design_board_nuclei_fpga_eval:

Nuclei FPGA Evaluation Kit
==========================

.. _design_board_nuclei_fpga_eval_overview:

Overview
--------

Nuclei have customized different FPGA evaluation boards (called Nuclei FPGA Evaluation Kit),
which can be programmed with Nuclei Demo/Eval SoC FPGA bitstream.

* **Nuclei FPGA Evaluation Kit, 100T version**

  This **100T** version is a very early version which widely used since 2019, it has a
  Xilinx XC7A100T FPGA chip on the board.

.. _figure_design_board_nuclei_fpga_eval_1:

    .. figure:: /asserts/images/nuclei_eval_board.jpg
        :width: 70 %
        :align: center
        :alt: Nuclei FPGA Evaluation Kit, 100T Version

        Nuclei FPGA Evaluation Kit, 100T Version

* **Nuclei FPGA Evaluation Kit, DDR 200T version**

  This **DDR 200T** version is a latest version which provided since 2020.09, it has a
  Xilinx XC7A200T FPGA chip on the board, and the onboard DDR could be connected to
  Nuclei RISC-V Core.

  This board is a choice to replace the *100T version*, and it could be use to evaluate
  any Nuclei RISC-V core.

  We also use this version of board to evaluate Nuclei UX class core which can
  run Linux on it, it you want to run Linux on this board, please refer to `Nuclei Linux SDK`_.

.. _figure_design_board_nuclei_fpga_eval_2:

    .. figure:: /asserts/images/nuclei_ddr200t.png
        :width: 70 %
        :align: center
        :alt: Nuclei FPGA Evaluation Kit, DDR 200T Version

        Nuclei FPGA Evaluation Kit, DDR 200T Version

* **Nuclei FPGA Evaluation Kit, MCU 200T version**

  This **MCU 200T** version is a latest version which provided since 2020.09, it has a
  Xilinx XC7A200T FPGA chip on the board, but there is no DDR chip on the board.

  This board is a choice to replace the *100T version*, and it could be use to evaluate
  any Nuclei RISC-V core with don't use DDR.

.. _figure_design_board_nuclei_fpga_eval_3:

    .. figure:: /asserts/images/nuclei_mcu200t.jpg
        :width: 70 %
        :align: center
        :alt: Nuclei FPGA Evaluation Kit, MCU 200T Version

        Nuclei FPGA Evaluation Kit, MCU 200T Version

There are also other fpga board we supported, such as KU060 and VCU118 board, please contact
with our sales for details.

Click `Nuclei FPGA Evaluation Kit Board Documents`_ to access the documents of these boards.

.. _design_board_nuclei_fpga_eval_setup:

Setup
-----

Follow the guide in `Nuclei FPGA Evaluation Kit Board Documents`_ to setup the board,
make sure the following items are set correctly:

* Use **Nuclei FPGA debugger** to connect the **MCU-JTAG** on board to your PC
  in order to download and debug programs and monitor the UART message.
* Power on the board using USB doggle(for 100T) or DC 12V Power(for MCU 200T or DDR 200T).
* The Nuclei FPGA SoC FPGA bitstream with Nuclei RISC-V evaluation core inside
  is programmed to FPGA on this board.
* Following steps in `debugger kit manual`_ to setup JTAG drivers for your development environment

.. _design_board_nuclei_fpga_eval_use:

How to use
----------

For **Nuclei FPGA Evaluation board**:

* ``evalsoc`` can run on this fpga board, please choose the correct SoC.

* **DOWNLOAD** support all the modes list in :ref:`develop_buildsystem_var_download`

  - You can find default used linker scripts for different download modes in ``SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/``.

    - ``evalsoc.memory``: Memory information for evalsoc
    - ``gcc_evalsoc_sram.ld``: Linker script file for ``DOWNLOAD=sram``

  - If you want to specify your own modified linker script, you can follow steps described in :ref:`develop_appdev_linkscript`
  - If you want to change the base address or size of SRAM of linker script file,
    you can adapt the ``evalsoc.memory`` file.

* **CORE** support all the cores list in :ref:`develop_buildsystem_var_core`

* Its openocd configuration file can be found in ``SoC/evalsoc/Board/nuclei_fpga_eval/openocd_evalsoc.cfg``

To run this application in Nuclei FPGA Evaluation board in Nuclei N100 SDK,
you just need to use this **SOC** and **BOARD** variables.

.. code-block:: shell

    ### For evalsoc
    # Clean the application with DOWNLOAD=sram CORE=n100
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=sram CORE=n100 clean
    # Build the application with DOWNLOAD=sram CORE=n100
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=sram CORE=n100 all
    # Upload the application using openocd and gdb with DOWNLOAD=sram CORE=n100
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=sram CORE=n100 upload
    # Debug the application using openocd and gdb with DOWNLOAD=sram CORE=n100
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=sram CORE=n100 debug
    ### For evalsoc
    # Clean the application with DOWNLOAD=sram CORE=n100
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=sram CORE=n100 clean
    # Upload the application using openocd and gdb with DOWNLOAD=sram CORE=n100
    make SOC=evalsoc BOARD=nuclei_fpga_eval DOWNLOAD=sram CORE=n100 upload

If you want to try other toolchain, such as nuclei llvm or terapines zcc, you can pass an extra :ref:`develop_buildsystem_var_toolchain` make variable.

.. note::

   * You can change the value passed to **CORE** according to
     the Nuclei Demo SoC Evaluation Core the Nuclei FPGA SoC you have.
   * You can also change the value passed to **DOWNLOAD** to run
     program in different modes.
   * The FreeRTOS and UCOSII demos maybe not working in ``flashxip``
     download mode in Nuclei FPGA board due to program running in Flash is really too slow.
     If you want to try these demos, please use ``sram`` download mode.

.. _Nuclei FPGA Evaluation Kit Board Documents: https://nucleisys.com/developboard.php
.. _Memory Section: https://sourceware.org/binutils/docs/ld/MEMORY.html
.. _Nuclei Linux SDK: https://github.com/Nuclei-Software/nuclei-linux-sdk
.. _debugger kit manual: https://www.nucleisys.com/theme/package/Nuclei_FPGA_DebugKit_Intro.pdf
