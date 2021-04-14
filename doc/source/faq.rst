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

Please check whether your driver is installed successfully via replace target `upload` to `run_openocd`
as the board user manual described, especially, for **RV-STAR** and **Nuclei Demo SoC Evaluation** boards,
For windows, you need to download the **HummingBird Debugger Windows Driver** from
https://nucleisys.com/developboard.php, and install it.

If still not working, please check whether your JTAG connection is good.

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

*bfd* is addbreviation for **Binary File Descriptor**.

This is caused by the target core flen is 0, which means it didn't have float point
unit in it, but your program is compiled using flen = 4, single point float unit used,
which is incompatiable, similar cases such as ``bfd requires flen 8, but target has flen 4``

Just change your CORE to proper core settings will solve this issue.

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


How to select correct FDTI debugger?
------------------------------------

From Nuclei SDK release 0.2.9, the openocd configuration file doesn't
contain `ftdi_device_desc`_ line by default, so if there are more than
one FTDI debuggers which has the same VID/PID(0x0403/0x6010) as Nuclei
Debugger Kit use, then you might need to add extra ``ftdi_device_desc``
line in the openocd configuration file to describe the FTDI device description.

* For **Nuclei FPGA Evaluation Board**, you can check the openocd configuration
  file in *SoC/demosoc/Board/nuclei_fpga_eval/openocd_demosoc.cfg*.

* For **Nuclei RVSTAR Board**, you can check the openocd configuration file
  in *SoC/gd32vf103/Board/gd32vf103v_rvstar/openocd_gd32vf103.cfg*.


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


Why ECLIC handler can't be installed using ECLIC_SetVector?
-----------------------------------------------------------

If you are running in ``FlashXIP`` download mode, it is expected,
since the vector table is placed in Flash area which can't be changed
during running time.

You can only this ``ECLIC_SetVector`` API in when your vector table
is placed in RAM which can be changed during running time, so if you want to
write portable application, we recommended you to use exactly the eclic handler
names defined in **startup_<device>.S**.


Access to github.com is slow, any workaround?
---------------------------------------------

Access speed to github.com sometimes is slow and not stable, but if you want to clone source code,
you can also switch to use our mirror site maintained in gitee.com.

This mirror will sync changes from github to gitee every 6 hours, that is 4 times a day.

You just need to replace the github to gitee when you clone any repo in **Nuclei-Software** or **riscv-mcu**.

For example, if you want to clone **nuclei-sdk** using command
``git clone https://github.com/Nuclei-Software/nuclei-sdk``, then
you can achieve it by command ``git clone https://gitee.com/Nuclei-Software/nuclei-sdk``

\`.text' will not fit in region \`ilm' or \`.bss' will not fit in region \`ram'
-------------------------------------------------------------------------------

If you met similar message as below when build an application:

.. code-block:: console

    xxx/bin/ld: cifar10.elf section `.text' will not fit in region `ilm'
    xxx/bin/ld: cifar10.elf section `.bss' will not fit in region `ram'
    xxx/bin/ld: section .stack VMA [000000009000f800,000000009000ffff] overlaps section .bss VMA [00000000900097c0,00000000900144eb]
    xxx/bin/ld: region `ilm' overflowed by 43832 bytes
    xxx/bin/ld: region `ram' overflowed by 0 bytes

It is caused by the program is too big, our default link script is 64K ILM, 64K DLM, 4M SPIFlash for Nuclei Demo SoC.

If your core has bigger ILM or DLM, you can change related linker script file according to your choice.

For example, if you want to change linker script for nuclei_fpga_eval ilm download mode:
``ILM to 512K, DLM to 256K``, then you can change link script file
``SoC/demosoc/Board/nuclei_fpga_eval/Source/GCC/gcc_demosoc_ilm.ld`` as below:

.. code-block:: diff

    diff --git a/SoC/demosoc/Board/nuclei_fpga_eval/Source/GCC/gcc_demosoc_ilm.ld b/SoC/demosoc/Board/nuclei_fpga_eval/Source/GCC/gcc_demosoc_ilm.ld
    index 1ac5b90..08451b3 100644
    --- a/SoC/demosoc/Board/nuclei_fpga_eval/Source/GCC/gcc_demosoc_ilm.ld
    +++ b/SoC/demosoc/Board/nuclei_fpga_eval/Source/GCC/gcc_demosoc_ilm.ld
    @@ -28,8 +28,8 @@ ENTRY( _start )
     MEMORY
     {

    -  ilm (rxai!w) : ORIGIN = 0x80000000, LENGTH = 64K
    -  ram (wxa!ri) : ORIGIN = 0x90000000, LENGTH = 64K
    +  ilm (rxai!w) : ORIGIN = 0x80000000, LENGTH = 512K
    +  ram (wxa!ri) : ORIGIN = 0x90000000, LENGTH = 256K
     }


.. _debugger kit manual: https://www.nucleisys.com/theme/package/Nuclei_FPGA_DebugKit_Intro.pdf
.. _ftdi_device_desc: http://openocd.org/doc/html/Debug-Adapter-Configuration.html
