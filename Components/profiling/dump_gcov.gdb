define dump_gcov_data
    set $node=gcov_data_head
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
