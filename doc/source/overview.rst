.. _overview:

Overview
========

.. _overview_intro:

Introduction
------------

The **Nuclei Software Development Kit (SDK)** is an open-source embedded software platform to
speed up the software development of SoCs based on Nuclei N/NX Processors(except N100 series).

This Nuclei SDK is built based on the |NMSIS|, user can access all the APIs provided
by |NMSIS| and also the APIs that provided by Nuclei SDK which mainly for on-board
peripherals access such as GPIO, UART, SPI and I2C, etc.

Nuclei SDK provides a good start base for embedded developers which will help them simplify
software development and improve time-to-market through well-designed software framework.


.. _overview_design_arch:

Design and Architecture
-----------------------

The Nuclei SDK general design and architecture are shown in the block diagram as below.

.. _figure_overview_1:

.. figure:: /asserts/images/nuclei_sdk_diagram.png
   :width: 80 %
   :align: center
   :alt: Nuclei SDK Design and Architecture Diagram

   Nuclei SDK Design and Architecture Diagram

As :ref:`figure_overview_1` shown, The Nuclei SDK provides the following features:

* Nuclei Core API service is built on top of |NMSIS|, so silicon vendors of Nuclei N/NX processors can easily
  port their SoCs to Nuclei SDK, and quickly evaluate software on their SoC.
* Mainly support two Nuclei Processor based SoCs, :ref:`design_soc_hbird` and :ref:`design_soc_gd32vf103`
* Provided realtime operation system service via :ref:`design_rtos_freertos` and :ref:`design_rtos_ucosii`
* Provided bare-metal service for embedded system software beginners and resource-limited use-cases.
* Currently Nuclei SDK didn't define any common device APIs to access GPIO/I2C/SPI/UART devices, it still
  relied on the device/peripheral APIs from firmware libraries from various silicon vendors, such as current
  supported :ref:`design_soc_gd32vf103`.
* Applications are logically seperated into three parts:

  - **General applications for all Nuclei Processors**: In the Nuclei SDK software code, the applications provided
    are all general applications which can run on all Nuclei Processors, with basic UART service to provide ``printf`` function.
  - **HummingBird SoC applications**: These applications are not included in the Nuclei SDK software code, it is
    maintained seperately, it will use resource from HummingBird SoC and its evaluation boards to develop applications, which will
    not be compatiable with different boards.
  - **GD32VF103 SoC applications**: These applications are not included in the Nuclei SDK software code, it is
    maintained seperately, it will use resource from GD32VF103 SoC and its evaluation boards to develop applications, which will
    not be compatiable with different boards.

.. _overview_getstarted:

Get Started
-----------


.. _overview_contribute:

Contributing
------------


.. _overview_copyright:

Copyright
---------


.. _overview_license:

License
-------
