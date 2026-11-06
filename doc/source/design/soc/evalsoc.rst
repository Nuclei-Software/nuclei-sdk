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
  * **ECC_EN**: it is used internally by Nuclei, used to control (ilm/dlm/L1 I/Dcache)ecc unit enable or disable, introduced in 0.7.0 release. It also supports TLB and L2 cache ECC configuration, introduced in the 1.0.0 release.
  * **CODESIZE**: it is used to control whether remove all template routine code for interrupt and exception and banner print code to measure basic code size requirement for evalsoc when ``CODESIZE=1``
  * **SYSCLK**: it is used together with ``CODESIZE=1`` to overwrite default ``SYSTEM_CLOCK`` macro value for different bitstream, eg. ``SYSCLK=50000000 CODESIZE=1``, it will set default SYSTEM_CLOCK to 50000000.
  * **QEMU_MC_EXTOPT** is used to pass extra options to Nuclei Qemu ``-M`` machine options for evalsoc,
    please dont pass any extra ``,`` to this make variable, you can pass such as ``QEMU_MC_EXTOPT=debug=1`` but not pass ``QEMU_MC_EXTOPT=,debug=1``
  * **QEMU_CPU_EXTOPT** is used to pass extra options to Nuclei Qemu ``-cpu`` cpu options for evalsoc,
    please dont pass any extra ``,`` to this make variable, you can pass such as ``QEMU_CPU_EXTOPT=vlen=512`` but
    not pass ``QEMU_CPU_EXTOPT=,vlen=512``
  * **XLCFG_xxx** make variables such as **XLCFG_CIDU**, **XLCFG_CCM**, **XLCFG_TEE** and **XLCFG_SMPU** which are used to overwrite default macros defined in ``cpufeature.h`` which will affect **XXX_PRESENT** macros in ``evalsoc.h``, introduced in 0.7.0 release, and these variables now moved from ``runmode.mk`` to a new ``cpuconfig.mk`` and if ``cpufeature.mk`` is generated by ``nuclei_gen``, then it will not be effective from 0.9.0 release, please see ``cpufeature.h`` header file.
  * **XLCFG_ECLIC**: it is used to control ECLIC configuration in ``cpufeature.h``, when ``XLCFG_ECLIC=0`` it will undefine ``CFG_HAS_CLIC``, when ``XLCFG_ECLIC=1`` it will define ``CFG_HAS_CLIC``, and when ``XLCFG_ECLIC=2`` it will define both ``CFG_HAS_ECLICV2`` and ``CFG_HAS_CLIC`` for ECLICv2 support, introduced in 0.9.0 release.
  * **ECLIC_HWCTX**: it is used to enable ECLIC hardware context auto-save feature, when ``ECLIC_HWCTX=1`` it will define ``ECLIC_HW_CTX_AUTO`` macro, introduced in 0.9.0 release. Note the hardware saves the context on the mtvec path only, the RNMI entry takes the ``CSR_MNVEC`` path and always saves the GPR context in software.
  * **XLCFG_DBLTRP**: it is used to enable the Smrnmi/Smdbltrp (resumable NMI and M-mode double trap) support, when ``XLCFG_DBLTRP=1`` it will define ``CFG_HAS_ISA_DBLTRP`` in ``cpufeature.h`` and ``__DBLTRP_PRESENT`` in ``evalsoc.h``. Only set it when the target CPU really implements the extensions, otherwise a double trap enters the critical-error state instead of being delivered to the RNMI handler (see :ref:`design_app_demo_dbltrp`).

.. code-block:: shell

    # Choose SoC to be evalsoc
    # the following command will build application
    # using default evalsoc SoC based board
    # defined in Build System and application Makefile
    make SOC=evalsoc info # you can check current working SDK configuration information
    make SOC=evalsoc clean
    make SOC=evalsoc all

.. _design_soc_evalsoc_build_modularization:

Build System Modularization
---------------------------

Starting from version 0.9.0, the Nuclei EvalSoC build system has been refactored to improve modularity by splitting the original ``build.mk`` file into multiple specialized makefiles:

**Modularized Components:**

* **build.mk (original)**: Now focuses primarily on core build-related configurations and OpenOCD debugging configurations, including linker script handling, compiler flags, and debug interface setup.
* **cpuconfig.mk**: Contains CPU configuration options (``XLCFG_xxx``) that control which CPU features are enabled/disabled at build time, affecting ``cpufeature.h`` and ``evalsoc.h`` definitions to adapt for different CPU RTL configurations without changing source code.
* **qemu.mk**: Contains QEMU emulation options including machine and CPU configurations for functional simulation.
* **xlmodel.mk**: Contains Nuclei Near Cycle Model (xlmodel) emulation options for near-cycle accurate model simulation.
* **runmode.mk**: Contains **RUN mode** options such as ``IC_EN/DC_EN`` to control ILM/DLM/ICACHE/DCACHE enable or disable during ``_premain_init`` for internal testing purposes.

**Benefits:**

* Improved maintainability by separating concerns into dedicated files
* Better organization of configuration options
* Easier to extend and customize individual components
* Allows selective implementation - users without QEMU or xlmodel support can reference the build system without implementing those features
* Facilitates internal development and regression workflows by enabling more adaptable configuration flows
* Reduces customer porting effort by isolating platform-specific configurations

**Note:** When ``cpufeature.mk`` is generated by ``nuclei_gen``, the ``XLCFG_xxx`` variables in ``cpuconfig.mk`` will not be effective, as the generated file takes precedence.

.. _design_soc_evalsoc_socfeature:

SoC Feature Configuration (socfeature.h)
------------------------------------------

Starting from version 1.0.0, a new ``socfeature.h`` header is introduced in each SoC
directory (eg. ``SoC/evalsoc/Common/Include/socfeature.h``) to define the **SoC level**
features, which is a companion of ``cpufeature.h`` that defines the **CPU level** features.

Every SoC in Nuclei SDK ships a ``socfeature.h``:

* ``evalsoc``: contains the real mhartid/cluster configuration macros (see below), it
  describes a multiple cluster capable SoC layout.
* ``gd32vf103``/``gd32vw55x``: single cluster SoCs, only keep the default macro values
  as a placeholder. It is required because the shared RTOS porting assembly code (eg.
  ``FreeRTOS portasm.S`` and ``ThreadX context.S``/``interrupt.S``) includes
  ``socfeature.h`` to get ``CFG_SOC_LOCAL_HARTID_MASK`` for extracting the local hart id
  from the ``mhartid`` CSR, same as how those files include ``cpufeature.h``.

Currently ``socfeature.h`` contains the mhartid/cluster related configuration macros:

* ``CFG_SOC_CLUSTERID_OFS``: the bit offset of the cluster id field in the ``mhartid`` CSR.
  It is also the number of bits used to encode the local hart id inside one cluster.

For a multiple cluster SoC system, the ``mhartid`` CSR layout is defined by the SoC
architecture as below::

    mhartid[x:CFG_SOC_CLUSTERID_OFS]     : cluster id of the hart
    mhartid[CFG_SOC_CLUSTERID_OFS-1:0]   : local hart id inside the cluster

For example, when ``CFG_SOC_CLUSTERID_OFS`` is ``8`` (the default value, compatible
with the nuclei subsystem reference design), ``mhartid[7:0]`` is the local hart id
inside the cluster, and ``mhartid[31:8]`` is the cluster id.

A derived mask macro is also provided to extract the local hart id field from a full
``mhartid`` value, which can be used both in C code and in assembly code:

* ``CFG_SOC_LOCAL_HARTID_MASK``: ``(((1 << CFG_SOC_CLUSTERID_OFS) - 1) & 0x7FF)``
  (a plain integer constant without any C type suffix, so that it can be used
  directly both in C code and in assembly code, eg. ``andi reg, reg, CFG_SOC_LOCAL_HARTID_MASK``)
  to get the local hart id you can do ``mhartid & CFG_SOC_LOCAL_HARTID_MASK`` (or call the NMSIS
  API ``__get_local_hart_id()`` which does this internally), to get the cluster id you can do
  ``mhartid >> CFG_SOC_CLUSTERID_OFS`` (or call ``__get_cluster_id()``)
  This mask is clamped to ``0x7FF`` (``2047``): the mask is used with the RISC-V ``andi``
  instruction in the SMP startup assembly, which only accepts a 12-bit signed immediate
  (max ``+2047``), and the actual local hart id of the CPU is never that large, so a larger
  value would fail to assemble. It can be overridden via ``-DCFG_SOC_LOCAL_HARTID_MASK=<value>``
  when the default is not suitable (guarded by ``#ifndef``).

``socfeature.h`` is provided by the SoC vendor together with its SoC support: the evalsoc one is
created by Nuclei (see the file header comment), and when you integrate your own SoC into Nuclei
SDK, you should create/update this file according to your real SoC architecture. If the real
mhartid layout of your SoC is different from the default ``8``, you can override it temporarily
via the compiler option ``-DCFG_SOC_CLUSTERID_OFS=<value>`` (eg.
``make APP_COMMON_FLAGS=-DCFG_SOC_CLUSTERID_OFS=7``).

**When you integrate your own SoC**, please follow the evalsoc implementation as the reference:

* copy the ``evalsoc`` ``socfeature.h`` to your SoC's ``Common/Include/`` directory and rename
  the guard if needed;
* set ``CFG_SOC_CLUSTERID_OFS`` to the real cluster id bit offset of your SoC's ``mhartid``
  layout (the number of bits used to encode the local hart id inside one cluster); for a single
  cluster SoC which has no cluster id field, keep the default ``8`` which is harmless;
* make sure your SoC ``<Device>.h`` (eg. ``evalsoc.h``) includes this ``socfeature.h`` before
  ``nmsis_core.h``, so that your SoC value takes precedence over the NMSIS fallback (the NMSIS
  ``core_feature_base.h`` only provides a default fallback for the case that NMSIS is used
  without any SoC header);
* the shared RTOS porting assembly files (``FreeRTOS portasm.S``, ``ThreadX context.S``/
  ``interrupt.S``) and the GCC/IAR startup files include ``socfeature.h`` automatically, so as
  long as this file exists in your SoC's ``Common/Include`` directory (which is already in the
  include path), no extra work is needed for them.

.. note::

    NMSIS ``core_feature_base.h`` also defines a default fallback value ``8`` for
    ``CFG_SOC_CLUSTERID_OFS`` (guarded by ``#ifndef``), so that NMSIS can be used
    alone without any SoC header. When the SoC ``socfeature.h`` is included, the
    value defined there takes precedence.

**APIs affected in NMSIS:** (for the ``_s`` variants the CSR is ``shartid`` instead of ``mhartid``)

* ``__get_cluster_id()`` / ``__get_cluster_id_s()``: now returns ``mhartid >> CFG_SOC_CLUSTERID_OFS``
* ``__get_hart_id()`` / ``__get_hart_id_s()``: still returns the full ``mhartid``/``shartid`` value
* ``__get_local_hart_id()`` / ``__get_local_hart_id_s()``: **new** API, returns the local hart id
  inside the current cluster (``mhartid & CFG_SOC_LOCAL_HARTID_MASK``), the value ``BOOT_HARTID``
  compares against
* ``__get_hart_index()`` / ``__get_hart_index_s()``: now returns ``(mhartid & CFG_SOC_LOCAL_HARTID_MASK) - __HARTID_OFFSET``

``BOOT_HARTID`` is now compared against the **local hart id** inside the current
cluster (returned by ``__get_local_hart_id()``), so its valid range is
``[0, 2^CFG_SOC_CLUSTERID_OFS)``, see the definition in ``evalsoc.h``.

.. note::

    To avoid confusion, there are four different hart related concepts:

    * **full hart id** (``__get_hart_id()``): the full ``mhartid`` value, includes both the
      cluster id and the local hart id, used to identify a hart globally.
    * **cluster id** (``__get_cluster_id()`` = ``mhartid >> CFG_SOC_CLUSTERID_OFS``): identifies
      the cluster which the hart belongs to.
    * **local hart id** (``__get_local_hart_id()`` = ``mhartid & CFG_SOC_LOCAL_HARTID_MASK``):
      identifies a hart inside the current cluster, it is what ``BOOT_HARTID`` is compared against.
    * **hart index** (``__get_hart_index()`` = ``local_hart_id - __HARTID_OFFSET``):
      used only to address the per-hart resources, such as the system timer software interrupt
      (MSIP) and timer compare registers, the per-core arrays in SMP OS ports, the CIDU/SMPCC
      core/client id and the PLIC context id. In most SoCs ``__HARTID_OFFSET`` is 0, so hart
      index equals local hart id.

    When you need to check whether the current hart is the boot hart, compare the **local hart id**
    (``__get_local_hart_id()``) with ``BOOT_HARTID``; when you need to address a per-hart resource,
    use the **hart index** (``__get_hart_index()``).

    The ``BOOT_HARTID`` and ``HARTID_OFS`` make variables which control the boot local hart id and
    the hart index offset are documented in the :ref:`develop_buildsystem_var_boot_hartid` and
    :ref:`develop_buildsystem_var_hartid_ofs` sections of the build system document.

.. note::

    Some practical questions which are often asked about the four concepts above:

    * **When does the local hart id differ from the hart index?** In the default and most common
      case (a SMP cluster whose harts are numbered continuously from 0), the two are the same and
      ``__HARTID_OFFSET`` is 0. They differ only when the harts actually implemented in your cluster
      do not start from local hart id 0, eg. a cluster which really has 4 harts with local hart
      ids 3-6 (hart 0-2 are not implemented): then you set ``__HARTID_OFFSET=3`` so that the hart
      index is 0-3, keeping the per-hart resources addressed continuously from 0. If your cluster
      has no such hole at the beginning, just leave ``__HARTID_OFFSET`` at its default 0.

    * **When does the ``mhartid`` actually contain a non-zero cluster id?** Only on a real
      multi-cluster chip. On a single cluster SoC (such as the single-hart GD32 SoCs, or a normal
      evalsoc SMP run with 1 cluster), the cluster id field is 0 and the ``mhartid`` is just the
      local hart id, so ``__get_hart_id()`` and ``__get_local_hart_id()`` return the same value and
      ``__get_cluster_id()`` returns 0.

    * **How does a multiple cluster SoC map to the Nuclei SDK?** In Nuclei SDK each cluster of a
      multiple cluster SoC is treated as its own SoC directory (eg. ``SoC/<clusterN>``), each with
      its own ``socfeature.h`` which describes the mhartid layout (``CFG_SOC_CLUSTERID_OFS``) of that
      cluster. A single build only runs the harts of one cluster, and ``BOOT_HARTID`` only selects
      the boot hart inside that cluster.

    * **Which API should I use in my code?** For most application code you only need
      ``__get_local_hart_id()`` (to know which hart you are, or to compare with ``BOOT_HARTID``) and
      ``__get_cluster_id()`` (if you need to know which cluster you are in). Use ``__get_hart_id()``
      only when you need the full raw ``mhartid`` value (eg. to display or to pass to something that
      expects a globally unique hart id), and use ``__get_hart_index()`` only when you address a
      per-hart resource (SysTimer, PLIC context, CIDU/SMPCC core/client arrays, per-core arrays in
      SMP OS ports).

.. _design_soc_evalsoc_startup:

Startup Code and Reset Vector
-----------------------------

The EvalSoC startup has to do two things that pull in opposite directions: run code **before**
``.data``/``.bss`` exist (so it can not rely on initialized globals or on a C runtime), and
eventually hand control to the C runtime, whose code and data may live somewhere else than the
ROM the core is executing from, and which in ``flash`` mode has to be copied into place first.
This section describes how the startup is laid out to do both.

Where the core starts
~~~~~~~~~~~~~~~~~~~~~

On reset the core starts executing at a fixed address, the base of the ROM region, and it has no
idea that a linker script exists. Which memory that is depends on the download mode, and the
linker scripts express it with the ``ROM`` region alias:

.. list-table::
   :header-rows: 1

   * - ``DOWNLOAD``
     - ``ROM`` region
     - ``.text`` placement
     - copied by the startup
   * - ``flash``
     - flash (``0x20000000``)
     - ILM
     - yes
   * - ``flashxip``
     - flash (``0x20000000``)
     - flash, in place
     - no
   * - ``sram``
     - SRAM ROM window
     - SRAM, in place
     - no
   * - ``ddr``
     - DDR ROM window
     - DDR, in place
     - no
   * - ``ilm``
     - ILM
     - ILM, in place
     - no

The ``.init`` output section is always allocated at ``ROM`` with ``>ROM AT>ROM`` (VMA == LMA), so
it is executed in place from the ROM region and never copied, in every mode. That is the one
property the startup relies on: its own code is executable from the first instruction after reset.
The startup itself is the ``.text.init.startup`` input collected into ``.init``, and C code that
needs the same property (``__sync_harts()``) uses the plain ``.text.init`` input.

Note that ``.text`` is not the only thing that may be copied: in ``flash`` and ``flashxip`` the
``.data`` section has a load address in flash and a run address in RAM/DLM (``_data_lma != _data``),
so ``__init_common`` copies it as well. In ``sram``/``ddr``, which place ``.text`` and ``.data`` in
the same region, and in ``ilm``, whose ``DATA_LMA`` is the DLM the data already runs from, the
copy is skipped.

Why the vector table skips slot 0 in flash mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``.init`` does not necessarily start with ``_start``: when ``CFG_HAS_CLIC`` is set, the vector
table ``.text.vtable`` is placed first. That is why the reset behaviour differs between download
modes, and why the two groups of linker scripts disagree about where ``.text.vtable`` goes:

* In ``flashxip``/``ilm``/``sram``/``ddr``, ``.text.vtable`` is collected **inside ``.init``**,
  i.e. at the ROM base before the startup. On reset the core therefore lands on **slot 0 of the
  vector table**, not on ``_start``, and slot 0 has to be an actual instruction that transfers
  control: ``j _start``.
* In ``flash``, ``.text.vtable`` is collected **inside ``.text``**, which is linked to ILM
  (``CODERAM``) and only populated by ``__init_common`` later. The ROM base therefore holds
  ``_start`` itself, the reset never goes through the vector table, and slot 0 is free to be a
  normal vector entry pointing at ``default_intexc_handler``.

When ``CFG_HAS_CLIC`` is not set the vector table is not emitted at all, ``.init`` starts with the
startup in every mode, and ``VECTOR_TABLE_REMAPPED`` has no effect.

The two behaviours are selected at compile time by ``VECTOR_TABLE_REMAPPED``, which
``SoC/evalsoc/build.mk`` defines **only** for ``DOWNLOAD=flash``::

    ifeq ($(DOWNLOAD), flash)
    COMMON_FLAGS += -DVECTOR_TABLE_REMAPPED
    endif

``startup_evalsoc.S`` uses it to emit the right thing in slot 0::

    vector_base:
    #ifndef VECTOR_TABLE_REMAPPED
        j _start                    /* 0: Reserved, jump to _start for the reset */
        .align LOG_REGBYTES
    #else
        DECLARE_INT_HANDLER default_intexc_handler
    #endif

The table is a pointer array with one ``REGBYTES`` sized slot per interrupt, so entry *n* must sit
at ``vector_base + n * REGBYTES``. Slot 0 holds a 4 byte ``j`` instead of a pointer, and
``.align LOG_REGBYTES`` pads it up to a full slot: on RV32 ``REGBYTES`` is 4 and no padding is
needed, on RV64 it is 8 and the assembler inserts a 4 byte ``nop``.

Slot 0 is a reset-time concern only, never an interrupt dispatch path. Straight after reset stage
1 points ``mtvec`` at ``early_exc_entry``, and the real handlers are installed later, from
``_premain_init()``: it calls ``Interrupt_Init()``, which routes to ``ECLIC_Interrupt_Init()`` when
ECLIC is present, and that writes ``mtvec`` = ``exc_entry`` and ``mtvt`` = ``vector_base``. So the
``j _start`` in slot 0 is only ever executed as the very first instruction after reset.

The startup stages
~~~~~~~~~~~~~~~~~~

``startup_evalsoc.S`` is organised as three stages inside ``_start`` plus a tail, and the
important design property is that **control flows by falling through from one block into the
next, not by jumping** (when ``SMP_CPU_CNT`` is not set):

.. code-block:: text

    _start:
        Stage 1            disable interrupts, set up gp/tp/sp, mmisc_ctl, mtvec
                           = early_exc_entry
        Stage 2            enable FPU/VPU state, enable I/D caches, mcountinhibit
                           (in SMP builds, also gates the non boot harts)
        |                  (falls through)
        v
    __init_common:         copy .text (LMA -> VMA) when they differ,
                           copy .data, zero .bss                 [boot hart only]
        |                  (falls through)
        v
    _start_premain:        SystemInit, __libc_init_array          [boot hart only]
        |                  (falls through)
        v
    __skip_init:           __sync_harts, _premain_init, main()/smp_main()

Stages 1 and 2 are the part that runs before anything is initialised, and they are both inside
``_start``. Stage 3 is the first one to touch memory sections, and in the source it sits in
``__init_common``. In an SMP build the gate that sends the non boot harts away is at the end of
``_start``: they branch straight to ``__skip_init``, where ``__sync_harts()`` holds them until the
boot hart has finished the copies, so that no hart executes half copied code or reads
uninitialised data. See :ref:`design_soc_evalsoc_startup_smp` for the handshake itself.

Note that the blocks that fall through also ``call`` into ``.text``: ``_start_premain`` calls
``SystemInit()`` and ``__libc_init_array()``, ``__skip_init`` calls ``_premain_init()`` and
``main()``. Falling through only describes how control moves between the blocks inside ``.init``;
what they call is already copied and runnable by then.

.. _design_soc_evalsoc_startup_debug:

Debugging the startup in flash mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``.text`` copy has a consequence for the debugger that is easy to trip over: **a software
breakpoint set on a function in ``.text`` can be silently wiped out by the copy**, because the
copy writes over the very memory the breakpoint lives in.

``__init_common`` copies ``.text`` from its load address ``_text_lma`` (flash) to its run address
``_text`` (ILM) with a plain ``lw``/``sw`` loop:

.. code-block:: text

    la a0, _text_lma
    la a1, _text
    beq a0, a1, 2f          /* no copy when LMA == VMA */
      ...
    1:
      lw t0, (a0)
      sw t0, (a1)           /* overwrites whatever is at the VMA */
      ...

A software breakpoint works by having the debugger replace the instruction at the target address
with ``ebreak``. If that address is in ``.text`` (``0x80000000`` and up, i.e. ILM), and the
breakpoint is already in place when ``__init_common`` runs, the ``sw`` stores the original
instruction back over the ``ebreak``. The breakpoint is gone and execution runs straight past it,
with no error to tell you it happened.

This is specific to the modes that copy ``.text``. In ``flash`` mode ``_text_lma`` is in flash
while ``_text`` is ILM, so ``_text_lma != _text`` and the copy always runs. In
``flashxip``/``ilm``/``sram``/``ddr`` the two are equal and the ``beq`` skips it, so software
breakpoints in ``.text`` are unaffected there.

.. code-block:: text

    # .text not yet copied          # after the copy
    0x800006ea: 0xdeadbeef           0x800006ea: 0xf14027f3   <- csrr a5,mhartid

The ``.data`` copy does not destroy breakpoints, it only overwrites data: in ``flash`` and
``flashxip`` the section is copied from flash into RAM as well, so a value you poke into an
initialised variable before the copy is silently replaced too.

Where the breakpoint survives
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Anything that lives in ``.init`` is executed in place from flash and never copied, so software
breakpoints there are safe. That is the practical way to get control early:

* ``_start`` and ``_start_premain`` are in ``.init``. ``_start_premain`` is the last block in
  ``.init``, i.e. it runs **after** the ``.text``/``.data`` copy and before anything in ``.text``
  is reached, which makes it the natural place to stop first.
* Every function in ``.text`` is subject to the copy, ``main()``, ``_premain_init()``,
  ``SystemInit()`` and ``ECLIC_Interrupt_Init()`` included.

Stop at ``_start_premain`` with a software breakpoint (safe, it is in ``.init``), then set the
``.text`` breakpoint once you are there.

Because GDB inserts software breakpoints when it resumes, not when you type ``break``, the order
you do things matters:

* **Set the ``.text`` breakpoint after the copy has happened.** Stop at ``_start_premain`` first,
  then set the breakpoint on ``main``. By then the copy is done, so nothing overwrites it, and
  ``continue`` lands on it normally. This is the "break at ``_start_premain``, then break at
  ``main``" recipe.
* **Or use a hardware breakpoint on ``.text``**, with ``hbreak`` instead of ``break``. A hardware
  breakpoint is kept in a trigger register, not in memory, so the copy cannot erase it and you can
  set it before reset. Note the number of hardware triggers is limited by the core.

.. note::

    Do not conclude from a working setup that the problem does not exist: whether a ``.text``
    software breakpoint survives depends on whether it was inserted before or after the copy, and
    the debugger re-inserts breakpoints on every resume. The failure mode is a silently skipped
    breakpoint, not an error, so a run that seems fine may simply have re-inserted the breakpoint
    after the copy had already happened.

Section layout
~~~~~~~~~~~~~~

The startup code and the few C functions that need the same property, such as ``__sync_harts()``
in ``system_evalsoc.c``, all have to end up in ``.init``, with ``_start`` first. The design gives
each source its own input section name and lets the linker script impose the order, instead of
relying on the order of the objects on the link command line:

.. code-block:: text

    .init                                   (VMA == LMA == ORIGIN(ROM))
    +-----------------------------------+
    | .text.vtable / .text.vtable_s     |   vector table, when CFG_HAS_CLIC
    | .text.init.startup                |   <-- the startup_evalsoc.S code
    |   _start <-- reset vector         |
    |   __init_common, _start_premain,  |
    |   __skip_init, __amp_wait,        |
    |   smp_main, early_exc_entry       |
    | .text.init                        |   <-- __sync_harts() and friends
    | .init                             |
    +-----------------------------------+

``startup_evalsoc.S`` puts its code in ``.section .text.init.startup``; its vector table keeps its
own ``.text.vtable`` section (see above), so the startup assembly feeds two of the inputs shown
here. The five evalsoc GCC linker scripts place the startup input with ``KEEP`` ahead of
``*(.text.init)``::

  .init :
  {
    *(.text.vtable)
    *(.text.vtable_s)
    KEEP (*(.text.init.startup))
    *(.text.init)
    KEEP (*(SORT_NONE(.init)))
    . = ALIGN(4);
  } >ROM AT>ROM

Why a separate section, and why the whole file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Two alternatives were considered and rejected:

* **Rely on the object order, with all assembly objects ahead of the C/C++ ones on the link
  command line.** This is what the build system did before, and it is fragile: ``ld`` collects
  ``*(.text.init)`` in command line order, so the reset vector silently depends on the build
  system keeping that ordering forever. It broke as soon as the output directory rework
  (``O=`` support) regrouped the objects. The failure is also easy to miss, because it only shows
  up when a *C* object lands in ``.text.init`` ahead of the assembly startup: with ``-flto`` the
  whole ``system_evalsoc.c`` is folded into the LTO object, so ``application/baremetal/benchmark/dhrystone``
  in ``DOWNLOAD=flash`` mode got ``__sync_harts()`` - an empty ``ret`` when ``SMP_CPU_CNT`` is not
  set, 2 bytes - at the ROM base, executed that ``ret`` on reset and jumped to ``0``.
* **Give only ``_start`` its own section.** Wrong, for the reason given above: the blocks fall
  through into each other, so a section boundary between ``_start`` and ``__init_common`` makes
  ``ld`` insert fill in between and the core runs into that fill right after ``_start``.

If you add code that must run before ``.data``/``.bss`` are initialized, put it in ``.text.init``
(the plain one) and make sure it does not touch those sections. Do not add a second section
between ``.text.init.startup`` and ``.text.init``, and do not remove the ``KEEP``: without it
``--gc-sections`` or a reordered link can drop or move the reset vector again.

.. note::

    The IAR projects do not use ``.text.init`` at all. There the reset entry comes from IAR's own
    ``cstartup.s`` runtime code, which lands in ``.cstartup``, and the ``*.icf`` files pin it with
    ``"CSTARTUP32" : place at start of ROM_region32 { ro section .alias.hwreset,
    ro section .cstartup }``. ``SoC/evalsoc/Common/Source/IAR/startup_evalsoc.c`` only provides the
    vector table (``.mintvec``), ``__low_level_init()`` and ``smp_main()``, and ``__sync_harts()``
    is reached from the IAR startup path as well. The effect - the reset entry is pinned to the
    start of the ROM region - is the same, reached through a different mechanism.

.. _design_soc_evalsoc_startup_smp:

SMP startup synchronization
---------------------------

In an SMP build the harts do not all run the same startup path. Only the boot hart runs
``__init_common`` (the ``.text``/``.data`` copy and the ``.bss`` zeroing) and
``_start_premain``. A non boot hart is sent away at the end of ``_start`` straight to
``__skip_init``, so it reaches ``__sync_harts()`` with nothing copied yet. It must not go on
until the boot hart is done: running half copied code is a store access fault inside ILM, and
reading ``.data``/``.bss`` that is not ready shows up later as random behaviour instead of a
clean fault.

The handshake uses the CLINT MSIP bits, one per hart. They are the only shared state available
at that point: writable, reachable from every hart, and usable before ``.data``/``.bss`` exist,
which is why ``__sync_harts()`` can not use a plain C variable or a semaphore. The function
itself lives in ``.init``, executed in place from ROM, so it runs and can spin even before the
copy.

The protocol, driven entirely by the boot hart
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

    boot hart                                  peer hart
    ----------                                 ----------
    copy .text/.data, zero .bss
    enable L2 / SMP / IOCP
    __SMP_RWMB()                               (spinning on its own MSIP)
    for each peer i:
        CLINT_MSIP[i] = 1
    __SMP_RWMB()
    ... continues with no wait ...             while (CLINT_MSIP[me] == 0);
                                               CLINT_MSIP[me] = 0;
                                               __SMP_RWMB();
                                               ... continues

The peer's own MSIP bit is the "code is ready" flag for that peer. The boot hart sets every
peer bit once the copies are done and then carries on immediately, it never waits for anybody.
A peer spins on its own bit, clears it, and continues. No peer has to report back.

Giving each hart a bit of its own is what makes the boot hart non blocking. A bit shared by all
peers would have to stay set until the last one had observed it, so the boot hart would have to
wait for every peer before dropping it, and a peer arriving after that would spin on a cleared
bit forever. With a dedicated bit per hart, each bit stays set until its own consumer takes it,
so a slow or late peer never delays the boot hart, and a bit left set for a hart that never runs
is simply never consumed. This is also why ``SMP_CPU_CNT`` does not have to match the real hart
count exactly.

There is no timeout and none should be added. The boot hart never blocks, so it can not time out,
and a peer only waits for a bit that the boot hart is guaranteed to set once the copies finish, so
a timeout there would only mask a real bug. A spin loop also depends only on every hart being able
to make progress, not on how fast it does, which is why this works under ``qemu -icount`` where
the harts are time sliced by instruction count.

.. warning::

    The CLINT MSIP model in Nuclei qemu 2025.10 is broken: a write to a MSIP bit by one hart is
    not reliably observed by another hart polling that bit. With that qemu the SMP examples hang,
    with the peers stuck in ``__sync_harts()`` waiting for a bit the boot hart has already set.
    The same image boots all the harts on Nuclei qemu 2026.10 and later, so use 2026.10 or later
    to run the SMP examples on qemu. This is a limitation of that qemu version, not of the
    handshake described here.

.. _design_soc_evalsoc_eclicv2:

ECLICv2 Support
---------------

Starting from version 0.9.0, Nuclei EvalSoC provides support for ECLICv2 with hardware context auto-save feature.

.. warning::

    ECLICv2 and the double trap feature (Smrnmi/Smdbltrp, see
    :ref:`design_app_demo_dbltrp`) **can not coexist** in the same CPU. Do not
    build with ``XLCFG_ECLIC=2`` together with ``XLCFG_DBLTRP=1``, the
    combination triggers a compile warning in ``cpufeature.h``.

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

.. note::

   **XLCFG_ECC** is only valid when ``cpufeature.h`` and ``cpufeature.mk`` is not generated by ``nuclei_gen``, it will define macro to affect the cpu feature macros defined in ``cpufeature.h``

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
