.. _design_soc_evalsoc:

Nuclei Eval SoC
===============

.. note::

    Nuclei CPU IP now with iregion feature will use totally new evaluation SoC,
    this SoC is different from previous Demo SoC, please take care.

    Nuclei DemoSoC is now removed in 0.5.0 release, and please use evalsoc now.

Nuclei Eval SoC is an evaluation FPGA SoC from Nuclei
for customer to evaluate Nuclei RISC-V Process Core, and it is a successor for Demo SoC.

.. _design_soc_evalsoc_overview:

Overview
--------

To easy user to evaluate Nuclei Processor Core, the prototype
SoC (called Nuclei Eval SoC) is provided for evaluation purpose.

This prototype SoC includes:

* Processor Core, it can be Nuclei N class, NX class or UX class Processor Core.
* On-Chip SRAMs for instruction and data.
* The SoC buses.
* The basic peripherals, such as UART, SPI etc.

With this prototype SoC, user can run simulations, map it into the FPGA board,
and run with real embedded application examples.

If you want to learn more about this evaluation SoC, please get the
``<Nuclei_Eval_SoC_Intro.pdf>`` from `Nuclei`_.


.. _design_soc_evalsoc_boards:

Supported Boards
----------------

In Nuclei SDK, we support the following boards based on **Nuclei evalsoc** SoC, see:

* :ref:`design_board_nuclei_fpga_eval`, default Board when this SoC selected.

.. _design_soc_evalsoc_usage:

Usage
-----

If you want to use this **Nuclei evalsoc SoC** in Nuclei SDK, you need to set the
:ref:`develop_buildsystem_var_soc` Makefile variable to ``evalsoc``.

Extra make variables supported only in this SoC:
  * **RUNMODE**: it is used internally by Nuclei, used to control ILM/DLM/ICache/DCache enable or disable
    via make variable, please check ``SoC/evalsoc/runmode.mk`` for details. It is not functional by default,
    unless you set a non-empty variable to this ``RUNMODE`` variable, it can be used with different **ILM_EN/DLM_EN/IC_EN/DC_EN/CCM_EN**.
  * **L2_EN**: it is used internally by Nuclei, used to control L2 cache enable or disable.
  * **LDSPEC_EN**: it is used internally by Nuclei, used to control load speculative enable or disable.
  * **BPU_EN**: it is used internally by Nuclei, used to control branch prediction unit enable or disable.


.. code-block:: shell

    # Choose SoC to be evalsoc
    # the following command will build application
    # using default evalsoc SoC based board
    # defined in Build System and application Makefile
    make SOC=evalsoc clean
    make SOC=evalsoc all


.. _Nuclei: https://nucleisys.com/
