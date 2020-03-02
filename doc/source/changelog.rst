.. _changelog:

Changelog
=========

V0.2.0-alpha.xx
---------------

Release tag name is not yet ready, just record changelog for it.

* Build System

  - Add extra linker options ``-u _isatty -u _write -u _sbrk -u _read -u _close -u _fstat -u _lseek``
    in Makefile.conf to make sure if you pass extra ``-flto`` compile option, link phase will not fail

* Documentation

  - Add documentation about how to optimize for code size in application development, using ``demo_eclic``
    as example.

V0.2.0-alpha
------------

This is release ``0.2.0-alpha`` of Nuclei SDK.

* Documentation

  - Initial verison of Nuclei SDK documentation
  - Update Nuclei-SDK README.md

* Application

  - Add ``demo_eclic`` application
  - Add ``demo_dsp`` application
  - ``timer_test`` application renamed to ``demo_timer``

* Build System

  - Add comments for build System
  - Small bug fixes

* **NMSIS**

  - Change ``NMSIS/Include`` to ``NMSIS/Core/Include``
  - Add ``NMSIS/DSP`` and ``NMSIS/NN`` header files
  - Add **NMSIS-DSP** and **NMSIS-NN** pre-built libraries


V0.1.1
------

This is release ``0.1.1`` of Nuclei SDK.

Here are the main features of this release:

* Support Windows and Linux development in command line using Make

* Support development using PlatformIO, see https://github.com/Nuclei-Software/platform-nuclei

* Support Humming Bird FPGA evaluation Board and GD32VF103 boards

  - The **Humming Bird FPGA evaluation Board** is used to run evaluation FPGA bitstream
    of Nuclei N200, N300, N600 and NX600 processor cores
  - The **GD32VF103 boards** are running using a real MCU from Gigadevice which is using
    Nuclei N200 RISC-V processor core

* Support different download modes flashxip, ilm, flash for our FPGA evaluation board


.. _Nuclei-SDK: https://github.com/Nuclei-Software/nuclei-sdk
