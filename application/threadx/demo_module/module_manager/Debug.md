# Debug in Qemu

> Just for personal usage, maybe not work for you. Take care!!!

Here demonstrates how to debug module manager and module sample code built for nx900fd(rv64) in Qemu.

Assume you have setup build environment for nuclei sdk, see https://doc.nucleisys.com/nuclei_sdk/quickstart.html#setup-tools-and-environment

~~~shell
# assume riscv64-unkown-elf-gcc and qemu-system-riscv64 environment are setup before
cd application/threadx/demo_module/module_manager
# Clean module manager and module sample object files
make CORE=nx900fd clean cleanmodule
# Build module manager and module sample code
make CORE=nx900fd module all
~~~

Now you can start qemu and gdb port and wait for gdb connection.

~~~shell
make CORE=nx900fd  run_qemu_debug
"Run program threadx_module_manager.elf on qemu-system-riscv64, and waiting for gdb connection on port 3333"
"Then you can open a new terminal in the same directory and run make target: run_gdb to connect to this gdb connection"
qemu-system-riscv64 -M nuclei_evalsoc,download=sram -cpu nuclei-nx900fd,ext= -device loader,file=../module_sample/threadx_module_sample.bin,addr=0xA0400000 -smp 1 -icount shift=0 -nodefaults -nographic -serial stdio -kernel threadx_module_manager.elf -S -gdb tcp::3333
GDB Server listening on: 'tcp::3333'...
~~~

Use gdb/Nuclei Studio IDE Debug Launch(nsdk_threadx_module_manager.launch) to connect to this gdb port:

~~~shell
# in Nuclei Studio gdb now
b sample_threadx_module_manager.c:92
Breakpoint 6 at 0xa000ab6e: file sample_threadx_module_manager.c, line 92.

b sample_threadx_module_manager.c:98
Breakpoint 7 at 0xa000ab80: file sample_threadx_module_manager.c, line 98.
c
c
Continuing.

Breakpoint 6, module_manager_entry (thread_input=<optimized out>) at sample_threadx_module_manager.c:92
92	    txm_module_manager_start(&my_module);
p /x my_module.txm_module_instance_code_start
$2 = 0xa01039b8
p /x my_module.txm_module_instance_module_data_base_address
$3 = 0xa0105df8
del
Delete all breakpoints? (y or n) [answered Y; input not from terminal]
file
A program is being debugged already.
Are you sure you want to change the file? (y or n) [answered Y; input not from terminal]
No executable file now.
Discard symbol table from `C:\Work\Code\nuclei-sdk\application\threadx\demo_module\module_manager\threadx_module_manager.elf'? (y or n) [answered Y; input not from terminal]
No symbol file now.
b *0xa000ab80
Breakpoint 8 at 0xa000ab80
add-symbol-file application/threadx/demo_module/module_sample/threadx_module_sample.elf -s .text 0xa0103ab8 -s .bss 0xa0105df8
add symbol table from file "application/threadx/demo_module/module_sample/threadx_module_sample.elf" at
	.text_addr = 0xa0103ab8
	.bss_addr = 0xa0105df8
(y or n) [answered Y; input not from terminal]
Reading symbols from application/threadx/demo_module/module_sample/threadx_module_sample.elf...
b _txm_module_thread_shell_entry
Breakpoint 10 at 0xa0104a16: file ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c, line 143.
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa01164d0, thread_info=0xa0105990 <demo_module_pool_space+688>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa0116678, thread_info=0xa0105d90 <demo_module_pool_space+1712>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa0112438, thread_info=0xa0106144 <demo_module_pool_space+2660>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa0112d70, thread_info=0xa0106f54 <demo_module_pool_space+6260>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa01129c0, thread_info=0xa01069b4 <demo_module_pool_space+4820>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa0112b98, thread_info=0xa0106c84 <demo_module_pool_space+5540>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa0112f48, thread_info=0xa0107224 <demo_module_pool_space+6980>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa0113120, thread_info=0xa01074f4 <demo_module_pool_space+7700>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa0112610, thread_info=0xa0106414 <demo_module_pool_space+3380>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 10, _txm_module_thread_shell_entry (thread_ptr=0xa01127e8, thread_info=0xa01066e4 <demo_module_pool_space+4100>) at ../../../../OS/ThreadX/ports/nuclei/module_lib/src/txm_module_thread_shell_entry.c:143
143	{
c
c
Continuing.

Breakpoint 8, 0x00000000a000ab80 in ?? ()
del
Delete all breakpoints? (y or n) [answered Y; input not from terminal]
file
A program is being debugged already.
Are you sure you want to change the file? (y or n) [answered Y; input not from terminal]
No executable file now.
Discard symbol table? (y or n) [answered Y; input not from terminal]
No symbol file now.
add-symbol-file application/threadx/demo_module/module_manager/threadx_module_manager.elf
add symbol table from file "application/threadx/demo_module/module_manager/threadx_module_manager.elf"
(y or n) [answered Y; input not from terminal]
Reading symbols from application/threadx/demo_module/module_manager/threadx_module_manager.elf...
si
si
0x00000000a000ab84	98	    txm_module_manager_stop(&my_module);
n
n
101	    txm_module_manager_unload(&my_module);
n
n
105	    txm_module_manager_in_place_load(&my_module, "my module", (VOID *) 0xA0400000);
n
n
108	    txm_module_manager_start(&my_module);
p /x my_module.txm_module_instance_code_start
$4 = 0xa0400000
p /x my_module.txm_module_instance_module_data_base_address
$5 = 0xa01041b8
del
file
A program is being debugged already.
Are you sure you want to change the file? (y or n) [answered Y; input not from terminal]
No executable file now.
Discard symbol table? (y or n) [answered Y; input not from terminal]
No symbol file now.
add-symbol-file application/threadx/demo_module/module_sample/threadx_module_sample.elf -s .text 0xa0400100 -s .bss 0xa01041b8
add symbol table from file "application/threadx/demo_module/module_sample/threadx_module_sample.elf" at
	.text_addr = 0xa0400100
	.bss_addr = 0xa01041b8
(y or n) [answered Y; input not from terminal]
Reading symbols from application/threadx/demo_module/module_sample/threadx_module_sample.elf...
b thread_6_and_7_entry
Breakpoint 11 at 0xa04012da: file sample_threadx_module.c, line 378.
c
c
Continuing.

Breakpoint 11, thread_6_and_7_entry (thread_input=6) at sample_threadx_module.c:378
378	{
n
n
389	        if (thread_input == 6)
n
n
390	            thread_6_counter++;
n
n
395	        status =  tx_mutex_get(mutex_0, TX_WAIT_FOREVER);
n
n
398	        if (status != TX_SUCCESS)
n
n
404	        status =  tx_mutex_get(mutex_0, TX_WAIT_FOREVER);
n
n
407	        if (status != TX_SUCCESS)
n
n
411	        tx_thread_sleep(2);
n
n

Breakpoint 11, thread_6_and_7_entry (thread_input=7) at sample_threadx_module.c:378
378	{
n
n
389	        if (thread_input == 6)
n
n
392	            thread_7_counter++;
n
n
395	        status =  tx_mutex_get(mutex_0, TX_WAIT_FOREVER);
~~~

# Run in hardware

The example build process are the same as run in qemu, but when you want to run in hardware,
you need to make sure you have programmed nx900fd(rv64) bitstream in our fpga evaluation board,
and connect it to your pc. 

The module manager application is downloaded to `0xA0000000` which is known as the threadx application,
The module sample application is downloaded to `0xA0400000` which is known as the threadx module.

Here we use `gdb` to restore the threadx module binary to `0xA0400000`.

~~~shell
# connect to hardware using gdb + openocd and try to debug it
make CORE=nx900fd  debug
(gdb) monitor reset halt
JTAG tap: riscv.cpu tap/device found: 0x10900a6d (mfg: 0x536 (Nuclei System Technology Co Ltd), part: 0x0900, ver: 0x1)
(gdb) load
Loading section .init, size 0x330 lma 0xa0000000
Loading section .text, size 0xbb20 lma 0xa0000340
Loading section .data, size 0x658 lma 0xa0100000
Start address 0x00000000a0000200, load size 50344
Transfer rate: 72 KB/sec, 10068 bytes/write.
(gdb) restore ../module_sample/threadx_module_sample.bin binary 0xA0400000
Restoring binary file ../module_sample/threadx_module_sample.bin into memory (0xa0400000 to 0xa0401c28)
(gdb) b main 
Breakpoint 1 at 0xa0000344
(gdb) c
Continuing.

Breakpoint 1, 0x00000000a0000344 in main ()
(gdb) si
[riscv.cpu] Found 2 triggers
_tx_initialize_kernel_enter () at ../../../../OS/ThreadX/common/src/tx_initialize_kernel_enter.c:105
105         if (_tx_thread_system_state != TX_INITIALIZE_ALMOST_DONE)
(gdb) b module_manager_entry
Breakpoint 2 at 0xa000ab06: file sample_threadx_module_manager.c, line 78.
(gdb) c
Continuing.

Breakpoint 2, module_manager_entry (thread_input=0) at sample_threadx_module_manager.c:78
78          txm_module_manager_initialize((VOID *) module_ram, sizeof(module_ram));
(gdb) n
80          txm_module_manager_object_pool_create(object_memory, sizeof(object_memory));
(gdb) n
83          txm_module_manager_memory_fault_notify(module_fault_handler);
(gdb) n
86          txm_module_manager_memory_load(&my_module, "my module", (VOID *) 0xA0400000);
(gdb) n
89          txm_module_manager_external_memory_enable(&my_module, (void *) 0xA0500000, 128, TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_WRITE);
(gdb) n
92          txm_module_manager_start(&my_module);
(gdb) n
95          tx_thread_sleep(20);
(gdb) n
98          txm_module_manager_stop(&my_module);
(gdb) n
101         txm_module_manager_unload(&my_module);
(gdb) n
105         txm_module_manager_in_place_load(&my_module, "my module", (VOID *) 0xA0400000);
(gdb) n
108         txm_module_manager_start(&my_module);
(gdb) n
111         thread_6_counter = (ULONG *) ((ULONG)my_module.txm_module_instance_module_data_base_address + 6 * sizeof(unsigned long));
(gdb) n
115             printf("Hello world from the module manager demo %d, thread_6_counter %d!\n", i, *thread_6_counter);
(gdb) n
117             tx_thread_sleep(10);
(gdb) n
113         while(1)
(gdb) n
115             printf("Hello world from the module manager demo %d, thread_6_counter %d!\n", i, *thread_6_counter);
(gdb) n
117             tx_thread_sleep(10);
(gdb) n
113         while(1)
(gdb) n
115             printf("Hello world from the module manager demo %d, thread_6_counter %d!\n", i, *thread_6_counter);
(gdb) n
117             tx_thread_sleep(10);
(gdb) n
113         while(1)
(gdb) n
115             printf("Hello world from the module manager demo %d, thread_6_counter %d!\n", i, *thread_6_counter);
(gdb) n
117             tx_thread_sleep(10);
(gdb) n
113         while(1)
(gdb) n
115             printf("Hello world from the module manager demo %d, thread_6_counter %d!\n", i, *thread_6_counter);
(gdb) n
117             tx_thread_sleep(10);
(gdb) c
Continuing.

Program received signal SIGINT, Interrupt.
0x00000000a000232a in _tx_queue_receive (queue_ptr=queue_ptr@entry=0xa0116820 <my_module+1048>, destination_ptr=0xa0104090 <module_ram+1768>, 
    wait_option=4294967295) at ../../../../OS/ThreadX/common/src/tx_queue_receive.c:402
402                 thread_ptr -> tx_thread_suspend_cleanup =  &(_tx_queue_cleanup);
~~~


While in the uart terminal side, you will be able to see output like this

~~~shell
Nuclei SDK Build Time: Nov 22 2024, 11:54:32
Download Mode: SRAM
CPU Frequency 50000363 Hz
CPU HartID: 0
Hello world from the module manager demo 0, thread_6_counter 0!
Hello world from the module manager demo 1, thread_6_counter 3!
Hello world from the module manager demo 2, thread_6_counter 6!
Hello world from the module manager demo 3, thread_6_counter 8!
Hello world from the module manager demo 4, thread_6_counter 11!
Hello world from the module manager demo 5, thread_6_counter 13!
Hello world from the module manager demo 6, thread_6_counter 16!
Hello world from the module manager demo 7, thread_6_counter 18!
Hello world from the module manager demo 8, thread_6_counter 21!
Hello world from the module manager demo 9, thread_6_counter 23!
Hello world from the module manager demo 10, thread_6_counter 26!
Hello world from the module manager demo 11, thread_6_counter 28!
Hello world from the module manager demo 12, thread_6_counter 31!
Hello world from the module manager demo 13, thread_6_counter 33!
Hello world from the module manager demo 14, thread_6_counter 36!
Hello world from the module manager demo 15, thread_6_counter 38!
Hello world from the module manager demo 16, thread_6_counter 41!
Hello world from the module manager demo 17, thread_6_counter 43!
Hello world from the module manager demo 18, thread_6_counter 46!
Hello world from the module manager demo 19, thread_6_counter 48!
~~~
