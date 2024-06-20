# Please call gcov_collect(0); in your c code after the code needs coverage
# call this script in Nuclei Studio IDE Debugger Console like this below
# source nuclei_sdk/Components/profiling/dump_gcov.gdb
define dump_gcov_data
    set $node=gcov_data_head
    if $node == 0
        printf "WARNING: No gcov data found, did you call gcov_collect(0) in your c code after the code you want to coverage!"
    end
    while ($node != 0)
		set $filename=$node->filename
		set $bufstart=$node->buffer
		set $bufend=$node->buffer + $node->size
		printf "dump binary memory %s 0x%lx 0x%lx\n", $filename, $bufstart, $bufend
        eval "dump binary memory %s 0x%lx 0x%lx\n", $filename, $bufstart, $bufend
		set $node=$node->next
    end
end
dump_gcov_data
