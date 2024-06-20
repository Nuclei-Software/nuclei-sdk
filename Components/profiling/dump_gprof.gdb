# Please call gprof_collect(0); in your c code after the code needs profiling
# call this script in Nuclei Studio IDE Debugger Console like this below
# source nuclei_sdk/Components/profiling/dump_gprof.gdb
if gprof_data.buf != 0
	printf "dump binary memory gmon.out 0x%lx 0x%lx\n", gprof_data.buf, gprof_data.buf + gprof_data.size
	dump binary memory gmon.out gprof_data.buf gprof_data.buf + gprof_data.size
else
    printf "WARNING: No gprof data found, did you call gprof_collect(0) in your c code after the code you want to profiling"
end
