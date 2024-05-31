.. _overview:

Overview
========

.. _overview_intro:

Introduction
------------

.. note::

    This is Nuclei N100 SDK which is modified based on **Nuclei SDK 0.5.0** to support Nuclei 100 series CPU.

    If you are looking for Nuclei SDK for Nuclei 200/300/600/900/1000 CPU, please refer to https://doc.nucleisys.com/nuclei_sdk

The **Nuclei N100 Software Development Kit (SDK)** is an open-source software platform to
speed up the software development of SoCs based on Nuclei Processor 100 series Cores.

This Nuclei N100 SDK is built based on the modified |NMSIS| for N100 and also the APIs that provided by Nuclei N100 SDK which mainly for on-board
peripherals access such as UART etc.

Nuclei N100 SDK provides a good start base for embedded developers which will help them simplify
software development and improve time-to-market through well-designed software framework.

.. note::

    To get a pdf version of this documentation, please click `Nuclei N100 SDK Document`_

.. _overview_design_arch:

Design and Architecture
-----------------------

The Nuclei N100 SDK general design and architecture are shown in the block diagram as below.

.. _figure_overview_1:

.. figure:: /asserts/images/nuclei_sdk_diagram.png
   :width: 80 %
   :align: center
   :alt: Nuclei N100 SDK Design and Architecture Diagram

   Nuclei N100 SDK Design and Architecture Diagram

As :ref:`figure_overview_1` shown, The Nuclei N100 SDK provides the following features:

* Nuclei Core API service is built on top of |NMSIS|, so silicon vendors of Nuclei processors can easily
  port their SoCs to Nuclei N100 SDK, and quickly evaluate software on their SoC.
* Mainly support :ref:`design_soc_evalsoc`
* Provided realtime operation system service via :ref:`design_rtos_freertos`, :ref:`design_rtos_ucosii` and
  :ref:`design_rtos_rtthread`
* Provided bare-metal service for embedded system software beginners and resource-limited use-cases.
* Applications are logically separated into three parts:

  - **General applications for Nuclei 100 series Processors**: In the Nuclei N100 SDK software code, the applications provided
    are all general applications which can run on all Nuclei Processors, with basic UART service to provide ``printf`` function.
  - **Nuclei Eval SoC applications**: These applications are not included in the Nuclei N100 SDK software code, and it is
    *maintained separately*, which will use resource from Nuclei Demo SoC and its evaluation boards to develop applications, which will
    not be compatible with different boards.

.. _overview_getstarted:

Get Started
-----------

Please refer to :ref:`quickstart` to get started to take a try with Nuclei N100 SDK.

.. _overview_contribute:

Contributing
------------

Contributing to Nuclei N100 SDK is welcomed, if you have any issue or pull request
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

Nuclei N100 SDK is an opensource project licensed by :ref:`Apache License 2.0 <license>`.

.. _Nuclei N100 SDK Document: https://doc.nucleisys.com/nuclei_n100_sdk/nuclei_n100sdk.pdf
