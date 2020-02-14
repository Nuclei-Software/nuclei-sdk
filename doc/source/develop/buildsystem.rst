.. _develop_buildsystem:

Build System based on Makefile
==============================

Nuclei SDK's build system is based on Makefile, user can build,
run ordebug application in Windows and Linux.

.. _develop_buildsystem_structure:

Makefile Structure
------------------

Nuclei SDK's Makefiles mainly placed in **<NUCLEI_SDK_ROOT>/Build** directory.

The directory content list as below:

.. code-block:: text

    gmsl/
    Makefile.base
    Makefile.conf
    Makefile.core
    Makefile.files
    Makefile.files.gd32vf103
    Makefile.files.hbird
    Makefile.global  -> Created by user
    Makefile.misc
    Makefile.rtos
    Makefile.rtos.FreeRTOS
    Makefile.rtos.UCOSII
    Makefile.rules
    Makefile.soc
    Makefile.soc.gd32vf103
    Makefile.soc.hbird

The file or directory is used explained as below:

.. _develop_buildsystem_makefile_base:

Makefile.base
~~~~~~~~~~~~~

This **Makefile.base** file is used as Nuclei SDK build system entry file,
application's Makefile need to include this file to use all the features of
Nuclei SDK build system.

It will expose Make variables or options such as **BOARD** or **SOC** passed
by ``make`` command, click :ref:`develop_buildsystem_exposed_make_vars`
to learn more.

This file will include optional :ref:`develop_buildsystem_makefile_global`
and :ref:`develop_buildsystem_makefile_local` which allow user to set custom
global Makefile configurations and local application Makefile configurations.

This file will include the following makefiles:

* :ref:`develop_buildsystem_gmsl`: additional library functions provided via gmsl
* :ref:`develop_buildsystem_makefile_misc`: misc functions and OS check helpers
* :ref:`develop_buildsystem_makefile_conf`: main Makefile configuration entry
* :ref:`develop_buildsystem_makefile_rules`: make rules of this build system

.. _develop_buildsystem_gmsl:

gmsl
~~~~

The **gmsl** directory consist of the `GNU Make Standard Library (GMSL)`_,
which is an a library of functions to be used with GNU Make's $(call) that
provides functionality not available in standard GNU Make.

We use this **gmsl** tool to make sure we help us achieve some linux command
which is only supported in Linux.

.. _develop_buildsystem_makefile_misc:

Makefile.misc
~~~~~~~~~~~~~

This **Makefile.misc** file mainly provide these functions:

* Define **get_csrcs**, **get_asmsrcs**, **get_cxxsrcs** and **check_item_exist** make functions

  - **get_csrcs**: Function to get ``*.c or *.C`` source files from a list of directories, no ability to
    do recursive match. e.g. ``$(call get_csrcs, csrc csrc/abc)`` will return c source files in
    ``csrc`` and ``csrc/abc`` directories.
  - **get_asmsrcs**: Function to get ``*.s or *.S`` source files from a list of directories, no ability to
    do recursive match. e.g. ``$(call get_asmsrcs, asmsrc asmsrc/abc)`` will return asm source files in
    ``asmsrc`` and ``asmsrc/abc`` directories.
  - **get_cxxsrcs**: Function to get ``*.cpp or *.CPP`` source files from a list of directories, no ability
    to do recursive match. e.g. ``$(call get_cxxsrcs, cppsrc cppsrc/abc)`` will return cpp source files in
    ``cppsrc`` and ``cppsrc/abc`` directories.
  - **check_item_exist**: Function to check if item existed in a set of items. e.g.
    ``$(call check_item_exist, flash, flash ilm flashxip)`` will check ``flash`` whether existed in
    ``flash ilm flashxip``, if existed, return ``flash``, otherwise return empty.

* Check and define OS related functions, and also a set of trace print functions.

.. _develop_buildsystem_makefile_conf:

Makefile.conf
~~~~~~~~~~~~~

This **Makefile.conf** file will define the following items:

* Toolchain related variables used during compiling
* Debug related variables
* Include :ref:`develop_buildsystem_makefile_files` and :ref:`develop_buildsystem_makefile_rtos`
* Collect all the C/C++/ASM compiling and link options

.. _develop_buildsystem_makefile_rules:

Makefile.rules
~~~~~~~~~~~~~~

This **Makefile.rules** file will do the following things:

* Collect all the sources during compiling
* Define all the rules used for building, uploading and debugging
* Print help message for build system


.. _develop_buildsystem_makefile_files:

Makefile.files
~~~~~~~~~~~~~~

This **Makefile.files** file will do the following things:

* Define common C/C++/ASM source and include directories
* Define common C/C++/ASM macros
* Include **Makefile.files.<SOC>** which will include all the source
  code related to the **SOC** and **BOARD**

  - **Makefile.files.gd32vf103**: Used to include source code for
    :ref:`design_soc_gd32vf103`
  - **Makefile.files.hbird**: Used to include source code for
    :ref:`design_soc_hbird`


.. _develop_buildsystem_makefile_soc:

Makefile.soc
~~~~~~~~~~~~

.. _develop_buildsystem_makefile_rtos:

Makefile.rtos
~~~~~~~~~~~~~



.. _develop_buildsystem_makefile_global:

Makefile.global
~~~~~~~~~~~~~~~

This **Makefile.global** file is an optional file, and will not be tracked by git,
user can create own **Makefile.global** in **<NUCLEI_SDK_ROOT>/Build** directory.

In this file, user can define custom **SOC**, **BOARD**, **DOWNLOAD** options to
overwrite the default configuration.

For example, if you will use only the :ref:`design_board_gd32vf103v_rvstar`, you can
create the **<NUCLEI_SDK_ROOT>/Build/Makefile.global** as below:

.. code-block:: Makefile

    SOC ?= gd32vf103
    BOARD ?= gd32vf103v_rvstar
    DOWNLOAD ?= flashxip

.. note::

    * If you add above file, then you can build, run, debug application without passing
      **SOC**, **BOARD** and **DOWNLOAD** variables using make command for
      :ref:`design_board_gd32vf103v_rvstar` board, e.g.

      - Build and run application for :ref:`design_board_gd32vf103v_rvstar`: ``make run``
      - Debug application for :ref:`design_board_gd32vf103v_rvstar`: ``make debug``

    * The :ref:`design_board_gd32vf103v_rvstar` only support ``FlashXIP`` download mode.
    * If you create the **Makefile.global** like above sample code, you will also be able
      to use Nuclei SDK build system as usually, it will only change the default **SOC**,
      **BOARD** and **DOWNLOAD**, but you can still override the default variable using
      make command, such as ``make SOC=hbird BOARD=hbird_eval DOWNLOAD=ilm``

.. _develop_buildsystem_makefile_local:

Makefile.local
~~~~~~~~~~~~~~

As the :ref:`develop_buildsystem_makefile_global` is used to override the default Makefile
configurations, and the **Makefile.local** is used to override application level Makefile
configurations, and also this file will not be tracked by git.

User can create ``Makefile.local`` file in any of the application folder, placed together with
the application Makefile, for example, you can create ``Makefile.local`` in ``application/baremetal/helloworld``
to override default make configuration for this **helloworld** application.

If you want to change the default board for **helloworld** to use :ref:`design_board_gd32vf103v_rvstar`,
you can create ``application/baremetal/helloworld/Makefile.local`` as below:

.. code-block:: Makefile

    SOC ?= gd32vf103
    BOARD ?= gd32vf103v_rvstar
    DOWNLOAD ?= flashxip

.. note::

    * This local make configuration will override global and default make configuration.
    * If you just want to change only some applications' makefile configuration, you can
      add and update ``Makefile.local`` for those applications.

.. _develop_buildsystem_exposed_make_vars:

Makefile variables passed by make command
-----------------------------------------

In Nuclei SDK build system, we exposed the following Makefile variables
which can be passed via make command.

.. _develop_buildsystem_var_soc:

SOC
~~~

**SOC** variable is used to declare which SoC is used in application during compiling.

You can easily find the supported SoCs in the **<NUCLEI_SDK_ROOT>/SoC** directory.

Currently we support the following SoCs, see :ref:`table_dev_buildsystem_1`.

.. _table_dev_buildsystem_1:

.. list-table:: Supported SoCs
   :widths: 10, 60
   :header-rows: 1
   :align: center

   * - SOC
     - Reference
   * - gd32vf103
     - :ref:`design_soc_gd32vf103`
   * - hbird
     - :ref:`design_soc_hbird`

BOARD
~~~~~

**Board** variable is used to declare which Board is used in application during compiling.

The **BOARD** variable should match the supported boards of chosen **SOC**.
You can easily find the supported Boards in the **<NUCLEI_SDK_ROOT>/<SOC>/Board/** directory.

* :ref:`table_dev_buildsystem_2`
* :ref:`table_dev_buildsystem_3`


Currently we support the following SoCs.

.. _table_dev_buildsystem_2:

.. list-table:: Supported Boards when SOC=gd32vf103
   :widths: 20, 60
   :header-rows: 1
   :align: center

   * - Board
     - Reference
   * - gd32vf103v_rvstar
     - :ref:`design_board_gd32vf103v_rvstar`
   * - gd32vf103v_eval
     - :ref:`design_board_gd32vf103v_eval`

.. _table_dev_buildsystem_3:

.. list-table:: Supported Boards when SOC=hbird
   :widths: 10 60
   :header-rows: 1
   :align: center

   * - Board
     - Reference
   * - hbird_eval
     - :ref:`design_board_hbird_eval`



.. _develop_buildsystem_app_make_vars:

Makefile variables used only in Application Makefile
----------------------------------------------------


.. _GNU Make Standard Library (GMSL): http://sourceforge.net/projects/gmsl/
