qemu-system-riscv32 -M nuclei_evalsoc,download=ilm -cpu nuclei-n300fd,ext= -smp 1 -icount shift=0 -nodefaults -nographic -serial stdio -device loader,file=../module_sample/threadx_module_sample.bin,addr=0xA0000000 -kernel threadx_module_manager.elf -S -s

REM  add-symbol-file application/threadx/demo_module/module_sample/threadx_module_sample.elf
