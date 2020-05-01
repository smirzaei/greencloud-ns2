# -------------------------------------------------------------
# ------------- Main GreenCloud simulation script -------------
# -------------------------------------------------------------

# Type of DC architecture
set sim(dc_type) [lindex $argv 6];			;# can be "three-tier", "three-tier high-speed", "three-tier debug", "three-tier heterogenous debug" and "three-tier heterogenous"
puts $sim(dc_type) 					;# in case of heterogenous topologies make sure that VMs are not larger than hosts. (You can also try to turn off virtualization in setup_params.tcl: set vm(static_virtualization) 0;)

# Set the time of simulation end
set sim(post_time) 0.6
set sim(end_time) [ expr 60 + [lindex $argv 1] + $sim(post_time)] 	;# simualtion length set to 60 s + deadline of tasks

# Start collecting statistics
set sim(start_time) 0.1

set sim(tot_time) [expr $sim(end_time) - $sim(start_time)]

set sim(linkload_stats) "enabled"

# Set the interval time (in seconds) to make graphs and to create flowmonitor file
set sim(interval) 0.1

# Setting up main simulation parameters
source "setup_params.tcl"				

# Get new instance of simulator
set ns [new Simulator]

# Tracing general files (*.nam & *.tr)
set nf [open "$dir(traces)/main.nam" w]
$ns namtrace-all $nf

set trace [open "$dir(traces)/main.tr" w]
$ns trace-all $trace

# Building data center topology
source "topology.tcl"

# Output simulation parameters
proc setup {} {
  global serv top switches sim psrv pswitches

  puts "  " 
  puts "*********************"
  puts "SIMULATION PARAMETERS"
  puts "*********************"
  puts "  "
  puts "Simulation time: $sim(tot_time) seconds"
  puts "  "

  set psrv ""
  if {$serv(eDVFS_enabled) == 1} {append psrv "DVFS "}
  if {$serv(eDNS_enabled) == 1} {append psrv "DNS"}
  if {[ string length $psrv ] == 0} { append psrv "No" }
  puts "Power management of computing servers: $psrv"

  set pswitches ""
  if {$switches(eDVFS_enabled) == 1} {append pswitches "DVFS "}
  if {$switches(eDNS_enabled) == 1} {append pswitches "DNS"}
  if {[ string length $pswitches ] == 0} { append pswitches "No" }
  puts "Power management of network switches: $pswitches"	
	
  puts "  "
}


source "dc.tcl"			;# Building data center components
# source "user.tcl"		;# Defining user behavior
# source "record.tcl"		;# Record procedure - time-depenent statistics
source "finish2.tcl"		;# Finish procedure - calculating stats, building graphs

# Simulation progress bar
set progress_interval [expr $sim(end_time)/10]

Simulator instproc progress { } {
    global progress_interval
    puts [format "Progress to %6.0f %%" [expr [$self now]/$progress_interval*10]]
    if { ![info exists progress_interval] } {
	set progress_interval [$self now]
    }
    $self at [expr [$self now] + $progress_interval] "$self progress"
}

$ns at $sim(start_time) "$ns progress"

# $ns at $sim(start_time) "record_graphs"

$ns at 0.05 "setup"

### MY CODE BEINGS

#set n1 [$ns node]
#set n2 [$ns node]
#
#$ns duplex-link $n1 $n2 1Mb 10ms DropTail
#
#set udp0 [new Agent/UDP]
#$ns attach-agent $n1 $udp0
#
#set cbr0 [new Application/Traffic/CBR]
#$cbr0 set packetSize_ 500
#$cbr0 set interval_ 0.005
#$cbr0 attach-agent $udp0
#
#set null0 [new Agent/Null]
#$ns attach-agent $n2 $null0
#
#$ns connect $udp0 $null0
#
#$ns at 1.0 "$cbr0 start"
#
#$ns at 5.0 "$cbr0 stop"


### MY CHANGES ENDS


# Scheduler start and stop for cloudusers
#for {set i 0} {$i < $top(NCloudUsers) } {incr i} {
#	$ns at 0.1 "$clouduser_($i) start"
#	$ns at [ expr $sim(end_time) - [lindex $argv 1] -  $sim(post_time)] "$clouduser_($i) stop"
#}

$ns at [expr $sim(end_time) + 0.1] "finish"

# Start the simulation
$ns run 
