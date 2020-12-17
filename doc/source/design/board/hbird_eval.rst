.. _design_board_hbird_eval:

HummingBird Evaluation Kit
==========================

.. _design_board_hbird_eval_overview:

Overview
--------

Nuclei have customized different FPGA evaluation boards (called Hummingbird Evaluation Kit),
which can be programmed with HummingBird SoC FPGA bitstream.

* **HummingBird Evaluation Kit, 100T version**

  This **100T** version is a very early version which widely used since 2019, it has a
  Xilinx XC7A100T FPGA chip on the board.

.. _figure_design_board_hbird_eval_1:

    .. figure:: /asserts/images/hbird_eval_board.jpg
        :width: 70 %
        :align: center
        :alt: HummingBird FPGA Evaluation Kit, 100T Version

        HummingBird FPGA Evaluation Kit, 100T Version

* **HummingBird Evaluation Kit, DDR 200T version**

  This **DDR 200T** version is a latest version which provided since 2020.09, it has a
  Xilinx XC7A200T FPGA chip on the board, and the onboard DDR could be connected to
  Nuclei RISC-V Core.

  This board is a choice to replace the *100T version*, and it could be use to evaluate
  any Nuclei RISC-V core.

  We also use this version of board to evaluate Nuclei UX class core which can
  run Linux on it, it you want to run Linux on this board, please refer to `Nuclei Linux SDK`_.

.. _figure_design_board_hbird_eval_2:

    .. figure:: /asserts/images/hbird_ddr200t.png
        :width: 70 %
        :align: center
        :alt: HummingBird FPGA Evaluation Kit, DDR 200T Version

        HummingBird FPGA Evaluation Kit, DDR 200T Version

* **HummingBird Evaluation Kit, MCU 200T version**

  This **MCU 200T** version is a latest version which provided since 2020.09, it has a
  Xilinx XC7A200T FPGA chip on the board, but there is no DDR chip on the board.

  This board is a choice to replace the *100T version*, and it could be use to evaluate
  any Nuclei RISC-V core with don't use DDR.

.. _figure_design_board_hbird_eval_3:

    .. figure:: /asserts/images/hbird_mcu200t.jpg
        :width: 70 %
        :align: center
        :alt: HummingBird FPGA Evaluation Kit, MCU 200T Version

        HummingBird FPGA Evaluation Kit, MCU 200T Version

Click `HummingBird FPGA Evaluation Kit Board Documents`_ to access the documents of these boards.

.. _design_board_hbird_eval_setup:

Setup
-----

Follow the guide in `HummingBird FPGA Evaluation Kit Board Documents`_ to setup the board,
make sure the following items are set correctly:

* Use **Hummingbird debugger** to connect the **MCU-JTAG** on board to your PC
  in order to download and debug programs and monitor the UART message.
* Power on the board using USB doggle(for 100T) or DC 12V Power(for MCU 200T or DDR 200T).
* The HummingBird SoC FPGA bitstream with Nuclei RISC-V evaluation core inside
  is programmed to FPGA on this board.
* Following steps in `debugger kit manual`_ to setup JTAG drivers for your development environment

.. _design_board_hbird_eval_use:

How to use
----------

For **HummingBird Evaluation board**:

* **DOWNLOAD** support all the modes list in :ref:`develop_buildsystem_var_download`

  - You can find default used linker scripts for different download modes in ``SoC/hbird/Board/hbird_eval/Source/GCC/``

    - ``gcc_hbird_ilm.ld``: Linker script file for ``DOWNLOAD=ilm``
    - ``gcc_hbird_flash.ld``: Linker script file for ``DOWNLOAD=flash``
    - ``gcc_hbird_flashxip.ld``: Linker script file for ``DOWNLOAD=flashxip``
    - ``gcc_hbird_ddr.ld``: Linker script file for ``DOWNLOAD=ddr``. **Caution**:
      This download mode can be only used when DDR is connect to Nuclei RISC-V Core

  - If you want to specify your own modified linker script, you can follow steps described in :ref:`develop_appdev_linkscript`
  - If you want to change the base address or size of ILM, DLM, RAM, ROM or Flash of linker script file,
    you can adapt the `Memory Section`_ in the linker script file it according to your SoC memory information.

* **CORE** support all the cores list in :ref:`develop_buildsystem_var_core`

* Its openocd configuration file can be found in ``SoC/hbird/Board/hbird_eval/openocd_hbird.cfg``

To run this application in HummingBird Evaluation board in Nuclei SDK,
you just need to use this **SOC** and **BOARD** variables.

.. code-block:: shell

    # Clean the application with DOWNLOAD=ilm CORE=n307
    make SOC=hbird BOARD=hbird_eval DOWNLOAD=ilm CORE=n307 clean
    # Build the application with DOWNLOAD=ilm CORE=n307
    make SOC=hbird BOARD=hbird_eval DOWNLOAD=ilm CORE=n307 all
    # Upload the application using openocd and gdb with DOWNLOAD=ilm CORE=n307
    make SOC=hbird BOARD=hbird_eval DOWNLOAD=ilm CORE=n307 upload
    # Debug the application using openocd and gdb with DOWNLOAD=ilm CORE=n307
    make SOC=hbird BOARD=hbird_eval DOWNLOAD=ilm CORE=n307 debug

.. note::

   * You can change the value passed to **CORE** according to
     the Nuclei Evaluation Core the HummingBird SoC you have.
   * You can also change the value passed to **DOWNLOAD** to run
     program in different modes.
   * The FreeRTOS and UCOSII demos maybe not working in ``flashxip``
     download mode in HummingBird board due to program running in Flash is really too slow.
     If you want to try these demos, please use ``ilm`` or ``flash``
     download mode.


.. _HummingBird FPGA Evaluation Kit Board Documents: https://nucleisys.com/developboard.php
.. _Memory Section: https://sourceware.org/binutils/docs/ld/MEMORY.html
.. _Nuclei Linux SDK: https://github.com/Nuclei-Software/nuclei-linux-sdk
.. _debugger kit manual: https://www.nucleisys.com/theme/package/Nuclei_FPGA_DebugKit_Intro.pdf
