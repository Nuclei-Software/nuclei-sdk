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

In Nuclei N100 SDK, we support the following boards based on **Nuclei evalsoc** SoC, see:

* :ref:`design_board_nuclei_fpga_eval`, default Board when this SoC selected.

.. _design_soc_evalsoc_usage:

Usage
-----

.. note::

    In latest N100 CPU RTL generation flow, it will also generate an Nuclei N100 SDK to match CPU
    and EvalSoC RTL configuration, please use the generated Nuclei N100 SDK to evaluate your
    CPU and EvalSoC feature.

    The generated Nuclei SDK by **nuclei_gen** will do the following tasks:

    - Generate ``SoC/evalsoc/cpufeature.mk``: which will define **CORE**, **ARCH_EXT**, **QEMU_SOCCFG** or **SIMULATION** default value.
    - Generate ``SoC/evalsoc/Common/Include/cpufeature.h``: which will define current cpu feature macros.
    - Generate ``SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/evalsoc.memory``: which will define the sram/flash base address and size and mtvt and mtvec address.
    - Modify ``SoC/evalsoc/Board/nuclei_fpga_eval/openocd_evalsoc.cfg``: Mainly change ``workmem_base/workmem_size/flashxip_base/xipnuspi_base`` to adapt the evalsoc configuration.

    If you want to use the generated Nuclei SDK by **nuclei_gen** In Nuclei Studio IDE, you need to zip it first,
    and then **import** it using ``RV-Tools -> NPK Package Management`` in Nuclei Studio IDE's menu, and when
    creating a IDE project using ``New Nuclei RISC-V C/C++ Project``, please select the correct sdk and version which
    you can check it in the ``<SDK>/npk.yml`` file, and in the project example configuration wizard window, you should
    click the **SDK gen by nuclei_gen**, and configure the **Nuclei RISC-V Core** and **ARCH Extensions**, **Nuclei Cache Extensions**
    according to your configured CPU ISA, and CPU feature defined in generated ``cpufeature.h``.
    Currently you still need to modify IAR linker script by yourself, it is not automatically modified.

    In latest evalsoc for n100, the ``interrupt vector table`` + ``reset_vector`` + ``exception_entry`` will be placed tightly in the top of an RO/RW memory.

    **It will looks like this as below:**

.. code-block:: shell

    Disassembly of section .init:       -> top of RO/RW memory

    a0000000 <vector_base>:             -> vector table for interrupt, which is the MTVT
        ...                                eg. for this case, there are 30 external interrupts(CFG_IRQ_NUM),
                                           totally 32 interrupts

    a0000080 <_reset_vector>:           -> reset vector right following the vector_table array, size may variable according to the external interrupt number count
    a0000080:	0080006f          	j	a0000088 <_start>       -> the reset vector code, just jump to real startup code
    a0000084:	0bc0006f          	j	a0000140 <exc_entry>    -> exception entry(MTVEC), just jump to real exception handling code

    a0000088 <_start>:
    a0000088:	30047073          	csrc	mstatus,8


    **NOTE**: Since evalsoc implementation is just a reference, you can customized your vector table, reset vector, exception entry as you want,
    but you **MUST** modify the startup code and linker script code to match your real CPU design.


If you want to use this **Nuclei evalsoc SoC** in Nuclei N100 SDK, you need to set the
:ref:`develop_buildsystem_var_soc` Makefile variable to ``evalsoc``.

.. code-block:: shell

    # Choose SoC to be evalsoc
    # the following command will build application
    # using default evalsoc SoC based board
    # defined in Build System and application Makefile
    make SOC=evalsoc all


.. _Nuclei: https://nucleisys.com/
