.. _design_nuclei:

Nuclei Processor
================

Nuclei processor core are following and compatible to RISC-V standard architecture,
but there might be some additions and enhancements to the original standard spec.

Click `Nuclei Spec`_ to learn more about Nuclei RISC-V Instruction Set Architecture.

.. _design_nuclei_intro:

Introduction
------------

Nuclei provides the following `RISC-V IP Products`_ for AIoT:

* **N100 series:** Designed for mixed digital and analog, IoT or
  other extremely low-power and small area scenarios, which
  is the perfect replacement of traditional 8051 cores, you need
  to use it with `Nuclei N100 SDK`_ .

* **N200 series:** Designed for ultra-low power consumption and
  embedded scenarios, perfectly replaces the arm Cortex-M series cores.

* **N300 series:** Designed for extreme energy efficiency ratio,
  requiring DSP and FPU features, as IoT and industrial control scenarios.

* **600 series and 900 series:** Fully support Linux for high-performance
  edge computing and smart AIoT.

* **1000 series:** The UX1000 Series have three different variants: UX1030, UX1040 and UX1060.
  UX1030 is a 3-wide processor with good performance and smaller power & area;
  UX1040 is a 4-wide processor with better performance and balanced power & area;
  UX1060 is a 6-wide processor with even higher performance targeting high-end applications.

.. note::

   * **N100 series** is not supported by **NMSIS** and **Nuclei SDK**

.. _design_nuclei_nmsis:

NMSIS in Nuclei SDK
-------------------

This Nuclei SDK is built based on the |NMSIS| framework,
user can access `NMSIS Core API`_, `NMSIS DSP API`_ and `NMSIS NN API`_
provided by |NMSIS|.

These NMSIS APIs are mainly responsible for accessing Nuclei RISC-V Processor
Core.

The prebuilt NMSIS-DSP and NMSIS-NN libraries are also provided in Nuclei SDK,
see ``NMSIS/Library/`` folder.

.. note::

    * To support RT-Thread in Nuclei-SDK, we have to modify the **startup_<device>.S**,
      to use macro ``RTOS_RTTHREAD`` defined when using RT-Thread as below:

      .. code-block:: c

        #ifdef RTOS_RTTHREAD
            // Call entry function when using RT-Thread
            call entry
        #else
            call main
        #endif

    * In order to support RT-Thread initialization macros ``INIT_XXX_EXPORT``, we also need
      to modify the link script files, add lines after `` *(.rodata .rodata.*)`` as below:

      .. code-block::

        . = ALIGN(4);
        *(.rdata)
        *(.rodata .rodata.*)
        /* RT-Thread added lines begin */
        /* section information for initial. */
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        /* RT-Thread added lines end */
        *(.gnu.linkonce.r.*)

.. _design_nuclei_soc:

SoC Resource
------------

Regarding the SoC Resource exclude the Nuclei RISC-V Processor Core,
it mainly consists of different peripherals such UART, GPIO, I2C, SPI,
CAN, PWM, DMA, USB and etc.

The APIs to access to the SoC resources are usually defined by the SoC
Firmware Library Package provided by SoC Vendor.

In Nuclei SDK, currently we just required developer to provide the following
common resources:

* A UART used to implement several stub functions for ``printf`` function

  - When using newlib library, please check stub functions list in ``SoC/evalsoc/Common/Stubs/newlib``
  - When using libncrt library, please check stub functions list in ``SoC/evalsoc/Common/Stubs/libncrt``
  - When using iar dlib library, please check stub functions list in ``SoC/evalsoc/Common/Stubs/iardlib``
* Common initialization code defined in **System_<Device>.c/h** in each
  SoC support package in Nuclei SDK.
* Before enter to main function, these resources must be initialized:

  - The UART used to print must be initialized as
    ``115200 bps, 8bit data, none parity check, 1 stop bit``
  - ECLIC MTH set to 0 using ``ECLIC_SetMth``, means don't mask any interrupt
  - ECLIC NLBits set to ``__ECLIC_INTCTLBITS``, means all the nlbits are for level
  - Global interrupt is disabled


.. note::

    * If you want to learn more about SoC, please click :ref:`design_soc`
    * If you want to learn more about Board, please click :ref:`design_board`
    * If you want to learn more about Peripheral, please click :ref:`design_peripheral`


.. _Nuclei Spec: https://doc.nucleisys.com/nuclei_spec/
.. _RISC-V IP Products: https://nucleisys.com/product.php
.. _NMSIS Core API: https://doc.nucleisys.com/nmsis/core/api/index.html
.. _NMSIS DSP API: https://doc.nucleisys.com/nmsis/dsp/api/index.html
.. _NMSIS NN API: https://doc.nucleisys.com/nmsis/nn/api/index.html
.. _Nuclei N100 SDK: https://doc.nucleisys.com/nuclei_n100_sdk
