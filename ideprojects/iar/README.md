# IAR Embedded Workbench IDE Projects For Nuclei Processors

## Introduction

In this folder, we provided a prebuilt IAR workspace with baremetal and rtos projects.

> You can refer to these demo projects to build your own iar projects, please use **IAR Embedded Workbench for RISC-V 3.30** released at **October 23, 2023**.

> If you want to evaluate Nuclei N300 DSP feature(P-ext 0.5.4), you can change the file `C:\Program Files\IAR Systems\Embedded Workbench 9.2\riscv\config\devices\Nuclei\N300.menu`, change `RV32IMAFDCB_Zkn_Zks_Zcb_Zcmp` to `RV32IMAFDCB_Zkn_Zks_Zcb_Zcmp_Xandesdsp`, `Xandesdsp` is based on P-ext 0.5.0, there are some intruction encoding updates from 0.5.0 to 0.5.4, and we use IAR custom instruction intrinsic to implement the different instructions, please check `NMSIS/Core/core_feature_dsp.h` for details, and in future, we will co-operate with IAR to support full Nuclei DSP feature including(N1/N2/N3 custom enhancement instructions).

These projects demostrated the following features:

- Provide support for Nuclei Evaluation SoC which is a prototype and evaluation SoC mainly
  used to evaluate Nuclei RISC-V processor such as 200/300/600/900 series.
- RISC-V Extension IMAFDCB are supported in this project.
- These projects are based on Nuclei SDK project which intergated NMSIS standard.
- We have implemented CPU core startup, exception handling and linker script and ported NMSIS Core
  for IAR Compiler, please check the commit histories for the changes we have made.
- These projects are using iar linker script icf file provided in this project not the one in IAR workbench.
- Currently both Nuclei RV32 and RV64 processors are supported in this project.
- If you want to **enable P extension**, choose ``Xandesdsp``(based on P-ext 0.5.0) option in IAR project settings: ``the Options->General Options-> ISA Extensions->DSP``, and link with P-ext 0.5.4 optimized library.
- Full Nuclei RISC-V DSP feature is still working in progress together with IAR team.
- DSP/NN libraries are able to be used in this project, you can try with prebuilt libraries, but not able to
  use any dsp intrinsic API, since not yet full compatiable with IAR version.
- For linker file used in IAR, the stack and heap are just following data/bss section, not at the end of ram,
  so you need to take care of the stack and heap size setting in IAR workbench, which means if your application
  code use a lot of heap or stack, the application may crash, you need to increase stack or heap size on demand.

> [!NOTE]
> SMP is now supported in IAR workbench since Nuclei SDK 0.6.0, please refer to `smphello` or `demo_cidu` project.
> `smphello` IAR IDE project by default should run on UX900FD x 4 SMP CORE, and program run on DDR memory.
> `demo_cidu` IAR IDE project by default should run on UX900FD x 2 SMP CORE, and CIDU feature, and program run on DDR memory.
> `freertos_smpdemo` IAR IDE project by default should run on NX900FD x 2 SMP CORE, with ECLIC present, and run on DDR memory to demostrate FreeRTOS with SMP feature, you can configure how smp cores used in this demo, by modify the project properties **configNUMBER_OF_CORES** and **SMP_CPU_CNT** in `C/C++ Compiler and Assembler -> Preprocessor -> Defined symbols(one per line)` and **SMP_CPU_CNT** in `Linker -> Config -> Configuration file symbol definitions:(one per line)`.

- SMP application is now supported in IAR project see `smphello`, to support SMP, you should use different startup asm code
  and different linker file provided in this repo, see `SoC/evalsoc/Common/Source/IAR/startup.S`, and for the linker file,
  you can refer to `application/baremetal/smphello/iar_evalsoc_smp.icf` which is base on the `iar_evalsoc_ddr.icf` mainly
  require linker define `SMP_CPU_CNT` to control how many cpu cores are used, and prepare correct `CSTACK` for these smp cpu,
  and you should also define a macro in `asm/c/cpp/linker` options called `SMP_CPU_CNT` to define how many cores you want to support, such as `SMP_CPU_CNT=4`.
- `demo_cidu` example is also added as a smp application, which require CIDU feature enabled, and a seperated linker file
  `application/baremetal/demo_cidu/iar_evalsoc_smp.icf` is used which setup CSTACK for two cpu, and also set extra macros
  `SMP_CPU_CNT=2` and `__CIDU_PRESENT=1`, linker config define must be defined `SMP_CPU_CNT=2`.

## Quick Start

You can directly try with this iar workspace by click this `nucleisdk.eww` after you have installed [IAR Workbench](https://www.iar.com/riscv).
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

## FAQ

* **demo_dsp** link failed with `unsupported section type 0x70000003 found in riscv_conv_fast_opt_q15.c.o(libnmsis_dsp_rv32imafdc.a)`

Since the prebuilt library is built with gcc, so it may not work with IAR compiler. You can try to follow guide in https://github.com/Nuclei-Software/NMSIS/tree/master/NMSIS/ideprojects/iar to rebuild this ``libnmsis_dsp_xxxx.a`` library, and link to the version built with IAR compiler.

* Some applications are using a different linker script than the one in `SoC/evalsoc/Board/nuclei_fpga_eval/Source/IAR` folder due to its project limitation, please take care.

* Some smp applications are using customized ``startup.S`` not the IAR compiler provided one, please take care.
