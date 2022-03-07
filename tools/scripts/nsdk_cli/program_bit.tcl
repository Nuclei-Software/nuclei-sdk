set bitfile [lindex $argv 0]
set hwtarget [lindex $argv 1]
if { [file exists $bitfile] != 1 } {
    puts "No bitfile $bitfile"
    exit 1
} else {
    puts "Using bitfile $bitfile"
}
open_hw_manager
connect_hw_server
set target [lindex [get_hw_targets -quiet -nocase $hwtarget] 0]
if { $target eq "" } {
    puts "Can't found hardware target $hwtarget"
    close_hw_manager
    exit 2
}
current_hw_target $target
open_hw_target
set first_hw_device [lindex [get_hw_devices -quiet] 0]
if { $first_hw_device eq "" } {
    puts "No hardware device found!"
    close_hw_manager
    exit 3
}
current_hw_device $first_hw_device
set_property PROGRAM.FILE $bitfile $first_hw_device

if { [program_hw_devices $first_hw_device] != 1 } {
    puts "Program bitfile $bitfile failed"
    close_hw_manager
    exit 4
}

exit 0