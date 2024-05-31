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
  is the perfect replacement of traditional 8051 cores.

* **N200 series:** Designed for ultra-low power consumption and
  embedded scenarios, perfectly replaces the arm Cortex-M series cores.

* **N300 series:** Designed for extreme energy efficiency ratio,
  requiring DSP and FPU features, as IoT and industrial control scenarios.

* **600/900/1000 series:** Fully support Linux for high-performance
  edge computing and smart AIoT.

.. note::

   * **N100 series** is supported by **modified NMSIS** and **Nuclei N100 SDK**
   * **200/300/600/900/1000 series** are supported by **NMSIS** and **Nuclei SDK**

.. _design_nuclei_nmsis:

NMSIS in Nuclei N100 SDK
------------------------

This Nuclei N100 SDK is built based on the modified |NMSIS| framework,
user can access modified NMSIS Core API

These modified NMSIS APIs are mainly responsible for accessing Nuclei RISC-V 100 series Processor
Core.

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

In Nuclei N100 SDK, currently we just required developer to provide the following
common resources:

* A UART used to implement several stub functions for ``printf`` function

  - When using newlib library, please check stub functions list in ``SoC/evalsoc/Common/Stubs/newlib``
  - When using libncrt library, please check stub functions list in ``SoC/evalsoc/Common/Stubs/libncrt``
  - When using iar dlib library, please check stub functions list in ``SoC/evalsoc/Common/Stubs/iardlib``
* Common initialization code defined in **System_<Device>.c/h** in each
  SoC support package in Nuclei N100 SDK.
* Before enter to main function, these resources must be initialized:

  - The UART used to print must be initialized as
    ``115200 bps, 8bit data, none parity check, 1 stop bit``
  - Global interrupt is disabled


.. note::

    * If you want to learn more about SoC, please click :ref:`design_soc`
    * If you want to learn more about Board, please click :ref:`design_board`
    * If you want to learn more about Peripheral, please click :ref:`design_peripheral`


.. _Nuclei Spec: https://doc.nucleisys.com/nuclei_spec/
.. _RISC-V IP Products: https://nucleisys.com/product.php
