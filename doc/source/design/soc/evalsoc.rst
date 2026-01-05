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

In Nuclei SDK, we support the following boards based on **Nuclei Evaluation SoC**, see:

* :ref:`design_board_nuclei_fpga_eval`, default Board when this SoC selected.

.. _design_soc_evalsoc_usage:

Usage
-----

.. note::

    To ensure compatibility when using Nuclei EvalSoC(FPGA), please verify with our Application Engineer (AE) the specific CPU configuration to confirm if the EvalSoC's CPU possesses the features you intend to test.
    You can utilize the :ref:`design_app_cpuinfo` application to determine the available CPU features on your system and cross-reference this information with the Nuclei ISA specifications.

.. note::

    In latest CPU RTL generation flow, it will also generate an Nuclei SDK to match CPU
    and EvalSoC RTL configuration, please use the generated Nuclei SDK to evaluate your
    CPU and EvalSoC feature.

    The generated Nuclei SDK by **nuclei_gen** will do the following tasks:

    - Generate ``SoC/evalsoc/cpufeature.mk``: which will define **CORE**, **ARCH_EXT**, **QEMU_SOCCFG** or **SIMULATION** default value.
    - Generate ``SoC/evalsoc/Common/Include/cpufeature.h``: which will define current cpu feature macros.
    - Generate ``SoC/evalsoc/evalsoc.json``: which will define current qemu soc configuration according to the evalsoc and cpu configuration.
    - Generate ``SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/evalsoc.memory``: which will define the ilm/dlm/flash/ddr/sram base address and size.
    - Modify ``SoC/evalsoc/Board/nuclei_fpga_eval/openocd_evalsoc.cfg``: Mainly change ``workmem_base/workmem_size/flashxip_base/xipnuspi_base`` to adapt the evalsoc configuration.

    If you want to use the generated Nuclei SDK by **nuclei_gen** In Nuclei Studio IDE, you need to zip it first,
    and then **import** it using ``RV-Tools -> NPK Package Management`` in Nuclei Studio IDE's menu, and when
    creating a IDE project using ``New Nuclei RISC-V C/C++ Project``, please select the correct sdk and version which
    you can check it in the ``<SDK>/npk.yml`` file, and in the project example configuration wizard window, you should
    configure the **Nuclei RISC-V Core** and **ARCH Extensions**, **Nuclei Cache Extensions**
    according to your configured CPU ISA, and CPU feature defined in generated ``cpufeature.h``.

    **WARNING**: Currently you still need to modify IAR linker script(``*.icf``) by yourself, it is not automatically modified.

If you want to use this **Nuclei Evaluation SoC** in Nuclei SDK, you need to set the
:ref:`develop_buildsystem_var_soc` Makefile variable to ``evalsoc``.

.. note::

    IAR support is done by prebuilt IAR projects not through Makefile based build system, please check https://github.com/Nuclei-Software/nuclei-sdk/blob/master/ideprojects/iar/README.md for detailed usage.

Extra make variables supported only in this SoC and used internally only by Nuclei, not designed for widely used:
  * **RUNMODE**: it is used internally by Nuclei, used to control ILM/DLM/ICache/DCache enable or disable
    via make variable, please check ``SoC/evalsoc/runmode.mk`` for details. It is not functional by default,
    unless you set a non-empty variable to this ``RUNMODE`` variable, it can be used with different **ILM_EN/DLM_EN/IC_EN/DC_EN/CCM_EN**.
  * **L2_EN**: it is used internally by Nuclei, used to control L2 cache enable or disable, introduced in 0.6.0 release.
  * **LDSPEC_EN**: it is used internally by Nuclei, used to control load speculative enable or disable, introduced in 0.6.0 release.
  * **BPU_EN**: it is used internally by Nuclei, used to control branch prediction unit enable or disable, introduced in 0.6.0 release.
  * **ECC_EN**: it is used internally by Nuclei, used to control (ilm/dlm/L1 I/Dcache)ecc unit enable or disable, introduced in 0.7.0 release.
  * **XLCFG_xxx** make variables such as **XLCFG_CIDU**, **XLCFG_CCM**, **XLCFG_TEE** and **XLCFG_SMPU** which are used to overwrite default macros defined in ``cpufeature.h`` which will affect **XXX_PRESENT** macros in ``evalsoc.h``, introduced in 0.7.0 release.
  * **CODESIZE**: it is used to control whether remove all template routine code for interrupt and exception and banner print code to measure basic code size requirement for evalsoc when ``CODESIZE=1``
  * **SYSCLK**: it is used together with ``CODESIZE=1`` to overwrite default ``SYSTEM_CLOCK`` macro value for different bitstream, eg. ``SYSCLK=50000000 CODESIZE=1``, it will set default SYSTEM_CLOCK to 50000000.
  * **QEMU_MC_EXTOPT** is used to pass extra options to Nuclei Qemu ``-M`` machine options for evalsoc,
    please dont pass any extra ``,`` to this make variable, you can pass such as ``QEMU_MC_EXTOPT=debug=1`` but not pass ``QEMU_MC_EXTOPT=,debug=1``
  * **QEMU_CPU_EXTOPT** is used to pass extra options to Nuclei Qemu ``-cpu`` cpu options for evalsoc,
    please dont pass any extra ``,`` to this make variable, you can pass such as ``QEMU_CPU_EXTOPT=vlen=512`` but
    not pass ``QEMU_CPU_EXTOPT=,vlen=512``
  * **XLCFG_ECLIC**: it is used to control ECLIC configuration, when ``XLCFG_ECLIC=0`` it will undefine ``CFG_HAS_CLIC``, when ``XLCFG_ECLIC=1`` it will define ``CFG_HAS_CLIC``, and when ``XLCFG_ECLIC=2`` it will define both ``CFG_HAS_ECLICV2`` and ``CFG_HAS_CLIC`` for ECLICv2 support, introduced in 0.9.0 release.
  * **ECLIC_HWCTX**: it is used to enable ECLIC hardware context auto-save feature, when ``ECLIC_HWCTX=1`` it will define ``ECLIC_HW_CTX_AUTO`` macro, introduced in 0.9.0 release.

.. code-block:: shell

    # Choose SoC to be evalsoc
    # the following command will build application
    # using default evalsoc SoC based board
    # defined in Build System and application Makefile
    make SOC=evalsoc info # you can check current working SDK configuration information
    make SOC=evalsoc clean
    make SOC=evalsoc all

.. _design_soc_evalsoc_eclicv2:

ECLICv2 Support
---------------

Starting from version 0.9.0, Nuclei EvalSoC provides support for ECLICv2 with hardware context auto-save feature.

**Features:**

* Hardware context auto-save/restore for non-vector interrupt and exception handlers
* Automatic task stack pointer switching
* Support for both M-mode and S-mode operation
* Conditional compilation for backward compatibility

**Configuration:**

To enable ECLICv2 support with hardware context auto-save:

* **MUST**: Nuclei RISC-V CPU must have ECLICv2 feature configured
* Set ``XLCFG_ECLIC=2`` to enable ECLICv2 configuration or ``cpufeature.h`` contains ``CFG_HAS_ECLICV2`` macro
* Set ``ECLIC_HWCTX=1`` to enable hardware context auto-save feature
* When both options are enabled, the hardware will automatically save/restore context and handle stack pointer switching

**Example:**

.. code-block:: shell

    # Enable ECLICv2 with hardware context auto-save
    # eg. you are running on Nuclei N300 RISC-V CPU with ECLICv2 feature present
    # Test baremetal eclic v2 auto-save/restore feature, trap sp auto switch not enabled
    cd application/baremetal/demo_eclic
    # Test freertos eclic v2 auto-save/restore feature, trap sp auto switch enabled, since interrupt/exception need seperated stack besides task stack
    # cd application/freertos/demo
    make SOC=evalsoc CORE=n300 XLCFG_ECLIC=2 ECLIC_HWCTX=1 clean
    make SOC=evalsoc CORE=n300 XLCFG_ECLIC=2 ECLIC_HWCTX=1 all

.. _design_soc_evalsoc_ecc:

ECC Error Injection
-------------------

The ECC (Error Checking and Correction) mechanism supports:

- Detection and correction of **single-bit errors**,
- Raising an exception upon detection of a **double-bit error**,
- **No guarantee** of correct behavior when **three or more bits** are corrupted.

To verify ECC functionality, ECC error injection is used to simulate memory errors.
This technique bypasses the hardware-generated ECC code and instead writes a deliberately
corrupted ECC code into memory.

Depending on the hardware implementation, **only one** of the following two injection methods is
supported. This choice is **fixed in hardware** and **not configurable by software**.
Therefore, before performing ECC error injection tests, you must determine which method your target
hardware supports.

**Supported Injection Modes:**

1. **Direct Write Mode (Legacy)**

   Manually compute the full ECC code, including intentional errors, and write the complete data+ECC word directly to memory.

2. **XOR Mode (New)**

   Provide an **ECC mask** instead of a full ECC code. The hardware automatically XORs this mask with the correct (golden) ECC code to flip the specified bits, thereby injecting errors.

**Configuration:**

To run applications with appropriate injection mode:

- Use **Direct Write Mode** if: ``XLCFG_ECC=1`` is set, **or** ``cpufeature.h`` defines ``CFG_HAS_ECC`` but **does not define** ``CFG_ECC_CODE_XOR``.
- Use **XOR Mode** if: ``XLCFG_ECC=2`` is set, **or** ``cpufeature.h`` defines both ``CFG_HAS_ECC`` and ``CFG_ECC_CODE_XOR``.

**Example:**

For more information, refer to the :ref:`design_app_demo_ecc` application.

.. code-block:: shell

    # change to application/baremetal/demo_ecc
    cd application/baremetal/demo_ecc
    # Example 1: Test ECC error injection on ILM/DLM using Direct Write Mode
    # (e.g., N300 core with legacy ECC support)
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=n300 DOWNLOAD=ilm XLCFG_ECC=1 clean
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=n300 DOWNLOAD=ilm XLCFG_ECC=1 all
    # Example 2: Test ECC error injection on cache using XOR Mode
    # (e.g., UX900 core with modern ECC support)
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=ux900 DOWNLOAD=ddr CCM_EN=1 XLCFG_ECC=2 clean
    make SOC=evalsoc BOARD=nuclei_fpga_eval CORE=ux900 DOWNLOAD=ddr CCM_EN=1 XLCFG_ECC=2 all

.. _Nuclei: https://nucleisys.com/
