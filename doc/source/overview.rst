.. _overview:

Overview
========

.. _overview_intro:

Introduction
------------

The **Nuclei Software Development Kit (SDK)** is an open-source software platform to
speed up the software development of SoCs based on Nuclei Processor Cores.

This Nuclei SDK is built based on the |NMSIS|, user can access all the APIs provided
by |NMSIS| and also the APIs that provided by Nuclei SDK which mainly for on-board
peripherals access such as GPIO, UART, SPI and I2C, etc.

Nuclei SDK provides a good start base for embedded developers which will help them simplify
software development and improve time-to-market through well-designed software framework.

.. note::

    To get a pdf version of this documentation, please click `Nuclei SDK Document`_

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

* Nuclei Core API service is built on top of |NMSIS|, so silicon vendors of Nuclei processors can easily
  port their SoCs to Nuclei SDK, and quickly evaluate software on their SoC.
* **NMSIS-NN** and **NMSIS-DSP** library can be also used in Nuclei SDK, and the prebuilt libraries are
  included in **NMSIS/Library** folder of Nuclei SDK.
* Mainly support two Nuclei Processor based SoCs, :ref:`design_soc_demosoc` and :ref:`design_soc_gd32vf103`
* Provided realtime operation system service via :ref:`design_rtos_freertos`, :ref:`design_rtos_ucosii` and
  :ref:`design_rtos_rtthread`
* Provided bare-metal service for embedded system software beginners and resource-limited use-cases.
* Currently Nuclei SDK doesn't define any common device APIs to access GPIO/I2C/SPI/UART devices, which still
  relies on the device/peripheral APIs from firmware libraries provided by various silicon vendors, such as current
  supported :ref:`design_soc_gd32vf103`.
* Applications are logically separated into three parts:

  - **General applications for all Nuclei Processors**: In the Nuclei SDK software code, the applications provided
    are all general applications which can run on all Nuclei Processors, with basic UART service to provide ``printf`` function.
  - **Nuclei Demo SoC applications**: These applications are not included in the Nuclei SDK software code, and it is
    *maintained separately*, which will use resource from Nuclei Demo SoC and its evaluation boards to develop applications, which will
    not be compatible with different boards.
  - **GD32VF103 SoC applications**: These applications are not included in the Nuclei SDK software code, and it is
    *maintained separately*, which will use resource from GD32VF103 SoC and its evaluation boards to develop applications, which will
    not be compatible with different boards.

.. _overview_getstarted:

Get Started
-----------

Please refer to :ref:`quickstart` to get started to take a try with Nuclei SDK.

.. _overview_contribute:

Contributing
------------

Contributing to Nuclei SDK is welcomed, if you have any issue or pull request
want to open, you can take a look at :ref:`contribute` section.

.. _overview_copyright:

Copyright
---------

Copyright (c) 2019 - Present, Nuclei System Technology. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the Nuclei System Technology., nor the names of its contributors
   may be used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. NY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

.. _overview_license:

License
-------

Nuclei SDK is an opensource project licensed by :ref:`Apache License 2.0 <license>`.

.. _Nuclei SDK Document: https://doc.nucleisys.com/nuclei_sdk/nucleisdk.pdf
