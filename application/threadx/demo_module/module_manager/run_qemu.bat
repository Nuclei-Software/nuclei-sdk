qemu-system-riscv32 -M nuclei_evalsoc,download=ilm -cpu nuclei-n300fd,ext= -smp 1 -icount shift=0 -nodefaults -nographic -serial stdio -device loader,file=../module_sample/threadx_module_sample.bin,addr=0xA0000000 -kernel threadx_module_manager.elf -S -s

REM add-symbol-file application/threadx/demo_module/module_sample/threadx_module_sample.elf  -s .bss  0x90004800
REM remove-symbol-file -a 0xa0000080

REM qemu-system-riscv64 -M nuclei_evalsoc,download=ilm -cpu nuclei-nx900fd,ext= -smp 1 -icount shift=0 -m 2g -nodefaults -nographic -serial stdio -device loader,file=../module_sample/threadx_module_sample.bin,addr=0xA0000000 -kernel threadx_module_manager.elf

REM qemu-system-riscv32 -M nuclei_evalsoc,download=ilm -cpu nuclei-n300fd,ext= -smp 1 -icount shift=0 -m 2g -nodefaults -nographic -serial stdio -device loader,file=../module_sample/threadx_module_sample.bin,addr=0xA0000000 -kernel threadx_module_manager.elf

REM restore ../module_sample/threadx_module_sample.bin binary 0xa0000000

REM make RISCV_ARCH=rv64imac RISCV_ABI=lp64 bin
