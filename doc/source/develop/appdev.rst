.. _develop_appdev:

Application Development
=======================

.. _develop_appdev_overview:

Overview
--------

Here will describe how to develop an Nuclei SDK application.

To develop a Nuclei SDK application from scratch, you can do the following steps:

1. Create a directory to place your application code.
2. Create **Makefile** in the new created directory, the minimal **Makefile** should look like this

   .. code-block:: makefile
      :linenos:

       TARGET = your_target_name

       NUCLEI_SDK_ROOT = path/to/your_nuclei_sdk_root

       SRCDIRS = .

       INCDIRS = .

       include $(NUCLEI_SDK_ROOT)/Build/Makefile.base

3. Copy or create your application code in new created directory.

   .. note::

      * If you just want to SoC related resource, you can include header file ``nuclei_sdk_soc.h`` in your application code.
      * If you just want to SoC and Board related resource, you can include header file ``nuclei_sdk_hal.h`` in your application code.
      * For simplity, we recomment you to use ``nuclei_sdk_hal.h`` header file

4. Follow :ref:`develop_buildsystem` to change your application Makefile.

.. note::

   * Starting from version 0.9.0, you can use :ref:`develop_buildsystem_var_appdirs` to apply compilation flags specifically to your application source code.

.. _develop_appdev_addsrc:

Add Extra Source Code
---------------------

If you want to add extra source code, you can use these makefile variables:

To add all the source code in directories, recursive search is not supported.
  * :ref:`develop_buildsystem_var_srcdirs`: Add C/CPP/ASM source code located
    in the directories defined by this variable.

  * :ref:`develop_buildsystem_var_c_srcdirs`: Add C only source code located
    in the directories defined by this variable.

  * :ref:`develop_buildsystem_var_cxx_srcdirs`: Add CPP only source code located
    in the directories defined by this variable.

  * :ref:`develop_buildsystem_var_asm_srcdirs`: Add ASM only source code located
    in the directories defined by this variable.


To add only selected c/cxx/asm source files
  * :ref:`develop_buildsystem_var_c_srcs`: Add C only source code files defined by this variable.
  * :ref:`develop_buildsystem_var_cxx_srcs`: Add CPP only source code files defined by this variable.
  * :ref:`develop_buildsystem_var_asm_srcs`: Add ASM only source code files defined by this variable.


To exclude some source files
  * :ref:`develop_buildsystem_var_exclude_srcs`: Exclude source files defined by this variable.

.. _develop_appdev_addinc:

Add Extra Include Directory
---------------------------

If you want to add extra include directories, you can use these makefile variables:

* :ref:`develop_buildsystem_var_incdirs`: Include the directories defined by
  this variable for C/ASM/CPP code during compiling.
* :ref:`develop_buildsystem_var_c_incdirs`: Include the directories defined by
  this variable for C only code during compiling.
* :ref:`develop_buildsystem_var_cxx_incdirs`: Include the directories defined by
  this variable for CPP only code during compiling.
* :ref:`develop_buildsystem_var_asm_incdirs`: Include the directories defined by
  this variable for ASM only code during compiling.

.. _develop_appdev_addoptions:

Add Extra Build Options
-----------------------

If you want to add extra build options, you can use these makefile variables:

* :ref:`develop_buildsystem_var_common_flags`: This will add compiling flags
  for C/CPP/ASM source code.
* :ref:`develop_buildsystem_var_cflags`: This will add compiling flags
  for C source code.
* :ref:`develop_buildsystem_var_cxxflags`: This will add compiling flags
  for CPP source code.
* :ref:`develop_buildsystem_var_asmflags`: This will add compiling flags
  for ASM source code.
* :ref:`develop_buildsystem_var_ldflags`: This will add linker flags when linking.
* :ref:`develop_buildsystem_var_ldlibs`: This will add extra libraries need to be linked.
* :ref:`develop_buildsystem_var_libdirs`: This will add extra library directories to be searched by linker.

.. _develop_appdev_optimize_for_codesize:

Optimize For Code Size
----------------------

If you want to optimize your application for code size, you set ``COMMON_FLAGS``
in your application Makefile like this:

.. code-block:: makefile

    COMMON_FLAGS := -Os

If you want to optimize code size even more, you use this link time optimization(LTO) as below:

.. code-block:: makefile

    COMMON_FLAGS := -Os -flto

see :ref:`design_app_demo_eclic` for example usage of optimize for code size.

For more details about gcc optimization, please refer to `Options That Control Optimization in GCC`_.

.. _develop_appdev_linkscript:

Change Link Script
------------------

If you want to change the default link script defined by your make configuration(SOC, BOARD, DOWNLOAD).
You can use :ref:`develop_buildsystem_var_linker_script` variable to set your linker script.

The default linker script used for different boards can be found in :ref:`design_board`.

.. _develop_appdev_setdefaultmake:

Set Default Make Options
------------------------

Set Default Global Make Options For Nuclei SDK
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you want to change the global Make options for the Nuclei SDK,
you can add the :ref:`develop_buildsystem_makefile_global`.


Set Local Make Options For Your Application
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you want to change the application level Make options,
you can add the :ref:`develop_buildsystem_makefile_local`.


.. _develop_appdev_appdirs_usage:

Application-Specific Compilation Flags with APPDIRS
---------------------------------------------------

Starting from Nuclei SDK version 0.9.0, you can use the :ref:`develop_buildsystem_var_appdirs` variable to apply compilation flags specifically to your application source code, without affecting the SDK library code. This is particularly useful for features like profiling, code coverage, or other application-specific optimizations.

The **APPDIRS** variable allows you to specify directories that need application-specific compilation flags. It can support multiple directories separated by space, e.g. ``APPDIRS = . src``.

If **APPDIRS** is not defined or empty, the **APP_XXXFLAGS** and **APP_COMMON_FLAGS** will be added to global flags and applied to all source files in the project.

If **APPDIRS** is defined, the **APP_XXXFLAGS** and **APP_COMMON_FLAGS** will be applied only to source code located in the directories specified by **APPDIRS**.

The following flags can be used with **APPDIRS**:

* :ref:`develop_buildsystem_var_app_common_flags`: Common flags for C/C++/ASM compilation
* :ref:`develop_buildsystem_var_app_cflags`: Flags specific to C compilation
* :ref:`develop_buildsystem_var_app_cxxflags`: Flags specific to C++ compilation
* :ref:`develop_buildsystem_var_app_asmflags`: Flags specific to ASM compilation

For example, if you want to append specific profiling or coverage flags only to your application source code while keeping the SDK source code compiled with default flags, you can set ``APPDIRS`` to point to your application source directories.

A practical example of this feature can be found in the ``application/baremetal/demo_profiling`` application, which uses ``APPDIRS`` to enable profiling instrumentation only for the application code:

.. code-block:: makefile

    # Application-specific compilation options that can be used for
    # profiling, coverage, and other app-specific settings
    # -pg enables profiling instrumentation
    APP_COMMON_FLAGS := -pg

    # App-specific C compilation flags
    APP_CFLAGS :=
    # App-specific C++ compilation flags
    APP_CXXFLAGS :=
    # App-specific Assembly compilation flags
    APP_ASMFLAGS :=

    # Specify directories that need app-specific compilation flags
    # If left empty, APP_XXXFLAGS will be applied to all source files
    APPDIRS := . src

This approach allows for fine-grained control of compilation flags for specific parts of an application while maintaining clean separation from SDK source code.

.. _Options That Control Optimization in GCC: https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/Optimize-Options.html#Optimize-Options
