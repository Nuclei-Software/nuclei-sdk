.. _design_soc_hbird:

HummingBird SoC
===============

HummingBird SoC is an evaluation FPGA SoC from Nuclei
for customer to evaluate Nuclei Process Core.

.. _design_soc_hbird_overview:

Overview
--------

To easy user to evaluate Nuclei Processor Core, the prototype
SoC (called Hummingbird SoC) is provided for evaluation purpose.

This prototype SoC includes:

* Processor Core, it can be Nuclei N class, NX class or UX class Processor Core.
* On-Chip SRAMs for instruction and data.
* The SoC buses.
* The basic peripherals, such as UART, GPIO, SPI, I2C, etc.

With this prototype SoC, user can run simulations, map it into the FPGA board,
and run with real embedded application examples.

The SoC diagram can be checked as below :ref:`figure_design_soc_hbird_1`

.. _figure_design_soc_hbird_1:

.. figure:: /asserts/images/hbird_soc_diagram.png
    :width: 80 %
    :align: center
    :alt: HummingBird SoC Diagram

    HummingBird SoC Diagram

The SoC memory map for SoC resources is as below :ref:`figure_design_soc_hbird_2`

.. _figure_design_soc_hbird_2:

.. figure:: /asserts/images/hbird_soc_memory_map.png
    :width: 80 %
    :align: center
    :alt: HummingBird SoC Memory Map

    HummingBird SoC Memory Map

If you want to learn more about this evaluation SoC, please get the
``<Nuclei_Eval_SoC_Intro.pdf>`` from `Nuclei`_.


.. _design_soc_hbird_boards:

Supported Boards
----------------

In Nuclei SDK, we support the following two boards based on **HummingBird** SoC, see:

* :ref:`design_board_hbird_eval`

.. _design_soc_hbird_usage:

Usage
-----

If you want to use this **HummingBird** SoC in Nuclei SDK, you need to set the
:ref:`develop_buildsystem_var_soc` Makefile variable to ``hbird``.

.. code-block:: shell

    make SOC=hbird


.. _Nuclei: https://nucleisys.com/
