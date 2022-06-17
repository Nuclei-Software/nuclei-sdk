close_hw_manager -quiet
open_hw_manager -quiet
connect_hw_server -quiet
set hwts [get_hw_targets -quiet]
foreach key $hwts {
    current_hw_target $key
    open_hw_target -quiet
    set hwds [get_hw_devices -quiet]
    puts "CSV,$key,$hwds"
    close_hw_target -quiet
}
disconnect_hw_server -quiet
close_hw_manager -quiet
