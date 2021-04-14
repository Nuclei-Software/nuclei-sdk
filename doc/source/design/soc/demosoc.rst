.. _design_soc_demosoc:

Nuclei Demo SoC
===============

.. note::

    Since Hummingbird is already taken by the opensource
    Hummingbird E203 SoC, we just rename Hummingbird SoC in Nuclei SDK
    to Nuclei Demo SoC to make it more clear.


Nuclei Demo SoC is an evaluation FPGA SoC from Nuclei
for customer to evaluate Nuclei RISC-V Process Core.

.. _design_soc_demosoc_overview:

Overview
--------

To easy user to evaluate Nuclei Processor Core, the prototype
SoC (called Nuclei Demo SoC) is provided for evaluation purpose.

This prototype SoC includes:

* Processor Core, it can be Nuclei N class, NX class or UX class Processor Core.
* On-Chip SRAMs for instruction and data.
* The SoC buses.
* The basic peripherals, such as UART, GPIO, SPI, I2C, etc.

With this prototype SoC, user can run simulations, map it into the FPGA board,
and run with real embedded application examples.

The SoC diagram can be checked as below :ref:`figure_design_soc_demosoc_1`

.. _figure_design_soc_demosoc_1:

.. figure:: /asserts/images/demosoc_soc_diagram.png
    :width: 80 %
    :align: center
    :alt: Nuclei Demo SoC Diagram

    Nuclei Demo SoC Diagram

The SoC memory map for SoC resources is as below :ref:`figure_design_soc_demosoc_2`

.. _figure_design_soc_demosoc_2:

.. figure:: /asserts/images/demosoc_soc_memory_map.png
    :width: 80 %
    :align: center
    :alt: Nuclei Demo SoC Memory Map

    Nuclei Demo SoC Memory Map

If you want to learn more about this evaluation SoC, please get the
``<Nuclei_Eval_SoC_Intro.pdf>`` from `Nuclei`_.


.. _design_soc_demosoc_boards:

Supported Boards
----------------

In Nuclei SDK, we support the following two boards based on **Nuclei demosoc** SoC, see:

* :ref:`design_board_nuclei_fpga_eval`

.. _design_soc_demosoc_usage:

Usage
-----

If you want to use this **Nuclei demosoc SoC** in Nuclei SDK, you need to set the
:ref:`develop_buildsystem_var_soc` Makefile variable to ``demosoc``.

.. code-block:: shell

    # Choose SoC to be demosoc
    # the following command will build application
    # using default demosoc SoC based board
    # defined in Build System and application Makefile
    make SOC=demosoc all


.. _Nuclei: https://nucleisys.com/
