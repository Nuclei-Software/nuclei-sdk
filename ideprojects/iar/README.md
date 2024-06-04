# IAR Embedded Workbench IDE Projects For Nuclei 100 Series Processors

In this folder, we provided a prebuilt IAR workspace with baremetal and rtos projects.

> You can refer to these demo projects to build your own iar projects, please use **IAR Embedded Workbench for RISC-V 3.30** released at **October 23, 2023**.

These projects demostrated the following features:

- Provide support for Nuclei Evaluation SoC which is a prototype and evaluation SoC mainly
  used to evaluate Nuclei 100 series RISC-V Processor.
- RISC-V Extension IMAC are supported in this project.
- These projects are based on Nuclei SDK project which intergated modified NMSIS standard.
- We have implemented CPU core startup, exception handling and linker script and ported NMSIS Core
  for IAR Compiler, please check the commit histories for the changes we have made.
- These projects are using iar linker script icf file provided in this project not the one in IAR workbench.
- Currently Nuclei 100 series Processors are supported in this project.
- For linker file used in IAR, the stack and heap are just following data/bss section, not at the end of ram,
  so you need to take care of the stack and heap size setting in IAR workbench, which means if your application
  code use a lot of heap or stack, the application may crash, you need to increase stack or heap size on demand.
- **N100** not yet list in the IAR project properties menu, so we choose **N200** now as a workaround, but it will be fixed in the future.

You can directly try with this iar workspace by click this `nuclei_n100_sdk.eww` after you have installed [IAR Workbench](https://www.iar.com/riscv).
![IAR Projects for Nuclei](asserts/nsdk_iar_projects.png)

If you want to debug using I-jet, you need to change Debugger Driver to I-jet in iar project options.

For Nuclei FPGA Evaluation Board, you need to connect I-jet JTAG pins to our FPGA onboard JTAG pins.

- VTref       <->   3.3V
- SWDIO/TMS   <->   TMS
- SWCLK/TCK   <->   TCK
- SWO/TDO     <->   TDO
- TDI         <->   TDI
- GND         <->   GND

If you want to see UART output, you need to connect FPGA UART TX/RX to your usb uart debug cable.

You can also try with C-SPY emulated IO by define `DEBUG_IAR_BREAKPOINT` in `SoC/evalsoc/Common/Source/Stubs/iardlib/stubs.c` to get print output.

For more details about how to use IAR workbench, please refer to its user guides.
