# **********************************
# Data center setup parameters
# **********************************

#

#set dc(target_load) 0.30		; # Fixed value or set as parameter: 
set dc(target_load)	0.1; # Targeted data center load in percent 

set dc(scheduler)   "Green";		; # Possible choice: "Green" "RoundRobin" "Random" "HEROS" "RandDENS" "BestDENS" # Selects data center scheduler
$defaultRNG seed    42;

# **********************************
# Setup parameters for servers
# **********************************

#NOTE: All data center configuration is done now in dc.tcl

set serv(eDVFS_enabled)	1 ; #DFVS is supported on the scheduler level. It should futher exploited by applying non-linear power model. Implementation will come soon.
set serv(eDNS_enabled)	1 ;

set vm(static_virtualization) 1; # Configures a single VM on each host if set to 1. (details in dc.tcl)
set vm(eDVFS_enabled) 	      0; # Analogous to serv(eDVFS_enabled). (VM uses the power of the hosting, so its internal scheduling behavior is important.)

# **********************************
# Setup parameters for switches
# **********************************
set switches(eDVFS_enabled)	1
set switches(eDNS_enabled)	0
set switches(eDNS_delay)	0.01	;# time required to power down the switch

# Switch
switch $sim(dc_type) {
  "three-tier high-speed" {

    # Core switch 
    set switches(eCore_Chassis)		15500
    set switches(eCore_LineCard)	12100
    set switches(eCore_Port)		200

    # Aggregation switch
    set switches(eAggr_Chassis)		3100
    set switches(eAggr_LineCard)	2400
    set switches(eAggr_Port)		200
  }
  # three-tier
  default {
	  
    # Core switch
    set switches(eCore_Chassis)		1558
    set switches(eCore_LineCard)	1212
    set switches(eCore_Port)		27

    # Aggregation switch
    set switches(eAggr_Chassis)		1558
    set switches(eAggr_LineCard)	1212
    set switches(eAggr_Port)		27
  }
}

# **********************************
# Task setup parameters
# **********************************

set task(type)		"HPC"				;# can be HPC, balanced, or comm (only HPC support right now)

switch $task(type) {
  default {						;# HPC - computationally intensive
    set task(mips)		30000			;# [MIPS] 10^6 Million instructions
    set task(memory)		256000	;# [Byte] of required used RAM
    set task(storage)		0			;# [Byte] of required disk space
    set task(size)		8500			;# [Byte] 8500 B of task input size
#   set task(duration)		5			;# computing deadline of tasks in seconds, alternatively set as parameter:
    set task(duration)		5	;# computing deadline in seconds
  }
}

set task(outputsize)	250000				;# Size of output on task completion

#set task(outputsize)	5500				;# Size of output on task completion

set task(intercom)	0				;# Size of inter-task communication

# *******************************************************
# Links and servers to monitor time dependant dynamics
# *******************************************************

set mon(link_hosts_C3)	0				;# link number to monitor connecting servers to rack switches
set mon(queue-HC3)	0				;# queue number to monitor connecting servers to rack switches
set mon(queue-C3C2)	0				;# queue number to monitor connecting rack switches to aggregate switches 
set mon(serv)		0				;# server number to monitor for the load


# **********************************
# 	MISC CONTROL VARIABLES:
# **********************************
# **********************************
# Data center setup parameters
# **********************************

set dir(traces)		"/home/greencloud/project"; 		# Traces directory 

# **********************************
# VM control variables
# **********************************
set virt(NVms) 0
set next_migration_id 0

# **********************************
# Power models control variables
# **********************************

set printPModel  0
set next_comp_model_id 0
