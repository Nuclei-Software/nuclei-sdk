.. _faq:

FAQ
===

Why I can't download application?
---------------------------------

* **Case 1: Remote communication error.  Target disconnected.: Success.**

.. code-block:: console

    Nuclei OpenOCD, 64-bit Open On-Chip Debugger 0.10.0+dev-00014-g0eae03214 (2019-12-12-07:43)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    Remote communication error.  Target disconnected.: Success.
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.
    You can't do that when your target is ``exec'
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.

Please check whether your driver is installed successfully via replace target ``upload`` to ``run_openocd``
as the board user manual described, especially, for **RV-STAR** and **Nuclei Eval SoC Evaluation** boards,
For windows, you need to download the **HummingBird Debugger Windows Driver** from
https://nucleisys.com/developboard.php, and install it.

If still not working, please check whether your JTAG connection is good or your CPU core is OK.

.. note::

    The USB driver might lost when you re-plug the USB port, you might need to reinstall the driver.

* **Case 2: bfd requires flen 4, but target has flen 0**

.. code-block:: console

    bfd requires flen 4, but target has flen 0
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.
    You can't do that when your target is `exec'
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.

*bfd* is abbreviation for **Binary File Descriptor**.

This is caused by the target core flen is 0, which means it didn't have float point
unit in it, but your program is compiled using flen = 4, single point float unit used,
which is incompatible, similar cases such as ``bfd requires flen 8, but target has flen 4``

Just change your CORE to proper core settings and will solve this issue.

For example, if you compile your core with ``CORE=n307``,
just change it to ``CORE=n305``.

* **Case 3: bfd requires xlen 8, but target has xlen 4**

.. code-block:: console

    bfd requires xlen 8, but target has xlen 4
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.
    You can't do that when your target is ``exec'
    "monitor" command not supported by this target.
    "monitor" command not supported by this target.

This issue is caused by the program is a riscv 64 program,
but the core is a riscv 32 core, so just change your program
to be compiled using a riscv 32 compile option.

For example, if you compile your core with ``CORE=ux600``,
just change it to ``CORE=n305``.


How to select correct FTDI debugger?
------------------------------------

From Nuclei N100 SDK release 0.2.9, the openocd configuration file doesn't
contain `ftdi_device_desc`_ line by default, so if there are more than
one FTDI debuggers which has the same VID/PID(0x0403/0x6010) as Nuclei
Debugger Kit use, then you might need to add extra ``ftdi device_desc``
line in the openocd configuration file to describe the FTDI device description.

Or you can add extra ``adapter serial your_serial_no`` for your debugger, you can check
its serial number via windows FT_PROG tool.

**NOTE**: for windows, you need to add an extra ``A`` to the serial number, eg. your serial number is
``FT6S9RD6``, then this extra openocd config line should be ``adapter serial "FT6S9RD6A"`` for windows.

* For **Nuclei FPGA Evaluation Board**, you can check the openocd configuration
  file in *SoC/evalsoc/Board/nuclei_fpga_eval/openocd_evalsoc.cfg*.

* For **Nuclei RVSTAR Board**, you can check the openocd configuration file
  in *SoC/gd32vf103/Board/gd32vf103v_rvstar/openocd_gd32vf103.cfg*.

For more details, please check `Debug with multiple FTDI devices <https://doc.nucleisys.com/nuclei_studio_supply/27-debug_with_multiple_ftdi_devices/>`_

Why I can't download application in Linux?
------------------------------------------

Please check that whether you have followed the `debugger kit manual`_
to setup the USB JTAG drivers correctly.
The windows steps and linux steps are different, please take care.


Why the provided application is not running correctly in my Nuclei FPGA Evaluation Board?
-----------------------------------------------------------------------------------------

Please check the following items:

1. Did you program the correct Nuclei Evaluation FPGA bitstream?
2. Did you re-power the board, when you just programmed the board with FPGA bitstream?
3. Did you choose the right **CORE** as the Nuclei Evaluation FPGA bitstream present?
4. If your application is RTOS demos, did you run in ``flashxip`` mode, if yes, it is expected
   due to flash speed is really slow, you'd better try ``ilm`` or ``flash`` mode.
5. If still not working, you might need to check whether the FPGA bitstream is correct or not?


Access to github.com is slow, any workaround?
---------------------------------------------

Access speed to github.com sometimes is slow and not stable, but if you want to clone source code,
you can also switch to use our mirror site maintained in gitee.com.

This mirror will sync changes from github to gitee every 6 hours, that is 4 times a day.

You just need to replace the github to gitee when you clone any repo in **Nuclei-Software** or **riscv-mcu**.

For example, if you want to clone **nuclei-n100-sdk** using command
``git clone -b develop_n100 https://github.com/Nuclei-Software/nuclei-sdk``, then
you can achieve it by command ``git clone -b develop_n100 https://gitee.com/Nuclei-Software/nuclei-sdk``

\`.text' will not fit in region \`ilm' or \`.bss' will not fit in region \`ram'
-------------------------------------------------------------------------------

If you met similar message as below when build an application:

.. code-block:: console

    xxx/bin/ld: cifar10.elf section `.text' will not fit in region `ilm'
    xxx/bin/ld: cifar10.elf section `.bss' will not fit in region `ram'
    xxx/bin/ld: section .stack VMA [000000009000f800,000000009000ffff] overlaps section .bss VMA [00000000900097c0,00000000900144eb]
    xxx/bin/ld: region `ilm' overflowed by 43832 bytes
    xxx/bin/ld: region `ram' overflowed by 0 bytes

It is caused by the program is too big, our default link script is 64K ILM, 64K DLM, 4M SPIFlash for Nuclei Demo/Eval SoC.

If your core has bigger ILM or DLM, you can change related linker script file according to your choice.

For example, if you want to change linker script for evalsoc on nuclei_fpga_eval ilm download mode:
``ILM to 512K, DLM to 256K``, then you can change link script file
``SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/gcc_evalsoc_ilm.ld`` as below:

.. code-block:: diff

    diff --git a/SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/gcc_evalsoc_ilm.ld b/SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/gcc_evalsoc_ilm.ld
    index 1ac5b90..08451b3 100644
    --- a/SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/gcc_evalsoc_ilm.ld
    +++ b/SoC/evalsoc/Board/nuclei_fpga_eval/Source/GCC/gcc_evalsoc_ilm.ld
    @@ -28,8 +28,8 @@ ENTRY( _start )
     MEMORY
     {

    -  ilm (rxa!w) : ORIGIN = 0x80000000, LENGTH = 64K
    -  ram (wxa!r) : ORIGIN = 0x90000000, LENGTH = 64K
    +  ilm (rxa!w) : ORIGIN = 0x80000000, LENGTH = 512K
    +  ram (wxa!r) : ORIGIN = 0x90000000, LENGTH = 256K
     }


undefined reference to __errno when using libncrt library
---------------------------------------------------------

When you are using libncrt library, and linked with ``-lm``, you may face below issues

.. code-block:: console

   /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/bin/ld: /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/lib/rv32imafdc/ilp32d/libm.a(libm_a-w_exp.o): in function `.L1':
   w_exp.c:(.text.exp+0x4a): undefined reference to `__errno'
   /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/bin/ld: /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/lib/rv32imafdc/ilp32d/libm.a(libm_a-w_exp.o): in function `.L0 ':
   w_exp.c:(.text.exp+0x6e): undefined reference to `__errno'
   /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/bin/ld: /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/lib/rv32imafdc/ilp32d/libm.a(libm_a-w_log.o): in function `log':
   w_log.c:(.text.log+0x28): undefined reference to `__errno'
   /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/bin/ld: w_log.c:(.text.log+0x46): undefined reference to `__errno'
   /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/bin/ld: /home/share/devtools/toolchain/nuclei_gnu/linux64/newlibc/2023.10.14/gcc/bin/../lib/gcc/riscv64-unknown-elf/13.1.1/../../../../riscv64-unknown-elf/lib/rv32imafdc/ilp32d/libm.a(libm_a-math_err.o): in function `with_errno':
   math_err.c:(.text.with_errno+0x12): undefined reference to `__errno'
   collect2: error: ld returned 1 exit status

You can fix it by not link ``-lm`` library, since libncrt library already provided math library feature, so
no need to link this math library.

undefined reference to fclose/sprintf similar API provided in system libraries
------------------------------------------------------------------------------

We no longer use ``--specs=`` option to select library we want to use, and we also passed
``-nodefaultlibs`` options to not use standard system libraries, this changes are made to support both gcc and clang
toolchain, so in Nuclei N100 SDK build system, we control the needed system libraries to be linked as required by ``STDCLIB`` make variable, for details, please check ``Build/toolchain/*.mk`` makefiles, and also we use linker's group
libraries feature ``--start-group archives --end-group`` to repeatly search undefined reference in the group libraries,
but this feature is not enabled in Eclipse CDT based IDE like Nuclei Studio, which undefined reference is searched in the order of library specified on the command line, so you may meet issue like undefined fclose reference even you linked newlib nano c library ``-lc_nano`` if the library order is not good, so to fix this issue, you may need to place
the library in a good order and need to repeatly link it, such as ``-lgcc -lc_nano -lm -lsemihost -lgcov -lgcc -lc_nano``, and also we have opened an issue to track it, see https://github.com/eclipse-embed-cdt/eclipse-plugins/issues/592

fatal error: rvintrin.h: No such file or directory
--------------------------------------------------

If you are using Nuclei Toolchain 2023.10, ``rvintrin.h`` no longer exist for B extension, please
don't include this header file. If you want to use an intrinsic API for B extension, you need to write
using c asm intrinsic.

riscv-nuclei-elf-gcc: not found when using Nuclei Studio 2023.10
----------------------------------------------------------------

``riscv-nuclei-elf-gcc``(gcc10) has changed to ``riscv64-unknown-elf-gcc``(gcc13) since Nuclei Studio 2023.10 or
Nuclei RISC-V Toolchain 2023.10, so if you are using older toolchain created npk package or ide project, you
may face this build fail issue, you can follow the user guide of Nuclei Studio 2023.10 to fix this issue, see chapter 8.

.. _debugger kit manual: https://nucleisys.com/developboard.php#ddr200t
.. _ftdi_device_desc: http://openocd.org/doc/html/Debug-Adapter-Configuration.html
