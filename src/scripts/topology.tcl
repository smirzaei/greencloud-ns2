# ---------------------------------------------------------
# ------------- Creating data center topology -------------
# ---------------------------------------------------------

# SWITCHES

switch $sim(dc_type) {
  "three-tier high-speed" {
    set top(NCore)		2			;# Number of L3 Switches in the CORE network
    set top(NAggr)		[expr 2*$top(NCore)]	;# Number of Switches in AGGREGATION network 
    set top(NAccess)		256			;# Number switches in ACCESS network per pod
    set top(NRackHosts)		3			;# Number of Hosts on a rack
  }
  "three-tier debug" {
    set top(NCore)		1			;# Number of L3 Switches in the CORE network
    set top(NAggr)		[expr 2*$top(NCore)]	;# Number of Switches in AGGREGATION network 
    set top(NAccess)		3			;# Number switches in ACCESS network per pod
    set top(NRackHosts)		48			;# Number of Hosts on a rack
  }
  "three-tier heterogenous debug" {
    set top(NCore)		1			;# Number of L3 Switches in the CORE network
    set top(NAggr)		[expr 2*$top(NCore)]	;# Number of Switches in AGGREGATION network 
    set top(NAccess)		3			;# Number switches in ACCESS network per pod
    set top(NRackHosts)		48			;# Number of Hosts on a rack
  }
  # three-tier
  default {
    set top(NCore)		4			;# Number of L3 Switches in the CORE network
    set top(NAggr)		[expr 2*$top(NCore)]	;# Number of Switches in AGGREGATION network 
    set top(NAccess)		8			;# Number switches in ACCESS network per pod
    set top(NRackHosts)		2			;# Number of Hosts on a rack
  }
}

# Number of racks
set top(NRacks) [expr $top(NAccess)*$top(NCore)]

# Number of servers
set top(NServers) [expr $top(NRacks)*$top(NRackHosts)]




# Set the propagation time (in seconds) on a link
set top(ptime_C1C2) 0.0033ms	
set top(ptime_C2C3) 0.0033ms
set top(ptime_C3H) 0.0033ms

switch $sim(dc_type) {
  "three-tier high-speed" {
    # Set the bandwidth on a link
    set top(bw_C1C2)	100000000000 	;#100Gb	
    set top(bw_C2C3)	10000000000 	;#10Gb	
    set top(bw_C3H)	1000000000 	;#1Gb
  }
  # three-tier
  default {
    # Set the bandwidth on a link
    set top(bw_C1C2)	10000000000	;#10Gb	
    set top(bw_C2C3)	10000000000	;#10Gb
    set top(bw_C3H)	1000000000	;#1Gb
  }
}

# Set topology size
set x_topology	1000
set y_topology	1000

# -----------------------------------------------------------------------------------------------
# --------------------------Start building topology here ----------------------------------------
# -----------------------------------------------------------------------------------------------

Queue/DropTail set limit_ 2147483647

# -----------------------------------------------------------------------------------------------
# -----------------------------------------Data Center 1 ----------------------------------------
# -----------------------------------------------------------------------------------------------

puts ""
puts ""
puts "*****************"
puts "BUILDING TOPOLOGY 1"
puts "*****************"
puts ""
puts "Data center architecture: $sim(dc_type)"
puts "Creating switches CORE($top(NCore)) AGGREGATION ($top(NAggr)) ACCESS($top(NRacks))..."

# Creating Data Center
set DCenter [new DataCenter]

set topoDC [new Topography]
$topoDC load_flatgrid $x_topology $y_topology

# CORE Swithces (C1)
set posX_shift [expr $x_topology/[expr $top(NCore) + 1]]
for {set i 0} {$i < $top(NCore) } {incr i} {
     set switch_C1_($i) [$ns node]

     # Switch position
     $switch_C1_($i) set X_ [expr $posX_shift + $i*$posX_shift]
     $switch_C1_($i) set Y_ 400.0	
     $switch_C1_($i) set Z_ 0.0

     # Switch energy model
     set energyModel_C1_($i) [new SwitchEnergyModel]
     $energyModel_C1_($i) set eChassis_ $switches(eCore_Chassis)
     $energyModel_C1_($i) set eLineCard_ $switches(eCore_LineCard)
     $energyModel_C1_($i) set ePort_ $switches(eCore_Port)
     $energyModel_C1_($i) set eSimEnd_ $sim(end_time)

     $energyModel_C1_($i) set eDVFS_enabled_ $switches(eDVFS_enabled)
     $energyModel_C1_($i) set eDNS_enabled_ $switches(eDNS_enabled)
     $energyModel_C1_($i) set eDNS_delay_ $switches(eDNS_delay)
     
     $ns at $sim(start_time) "$energyModel_C1_($i) start"
     $ns at $sim(end_time) "$energyModel_C1_($i) stop"

     set classifier [$switch_C1_($i) set classifier_]
     $classifier attach-energymodel $energyModel_C1_($i)
}

# AGGREGATION Switches (C2)
set posX_shift [expr $x_topology/[expr $top(NAggr) + 1]]
for {set j 0} {$j < $top(NAggr)} {incr j} {
     set switch_C2_($j) [$ns node]

     # Switch position
     $switch_C2_($j) set X_ [expr $posX_shift + $j*$posX_shift]
     $switch_C2_($j) set Y_ 300.0	
     $switch_C2_($j) set Z_ 0.0

     # Switch energy model
     set energyModel_C2_($j) [new SwitchEnergyModel]
     $energyModel_C2_($j) set eChassis_ $switches(eAggr_Chassis)
     $energyModel_C2_($j) set eLineCard_ $switches(eAggr_LineCard)
     $energyModel_C2_($j) set ePort_ $switches(eAggr_Port)
     $energyModel_C2_($j) set eSimEnd_ $sim(end_time)

     $energyModel_C2_($j) set eDVFS_enabled_ $switches(eDVFS_enabled)
     $energyModel_C2_($j) set eDNS_enabled_ $switches(eDNS_enabled)
     $energyModel_C2_($j) set eDNS_delay_ $switches(eDNS_delay)
     
     $ns at $sim(start_time) "$energyModel_C2_($j) start"
     $ns at $sim(end_time) "$energyModel_C2_($j) stop"

     set classifier_C2_ [$switch_C2_($j) set classifier_]
     $classifier attach-energymodel $energyModel_C2_($j)		
}

  # Three-tier and three-tier high-speed
  # Interconnection of AGGREGATION (C2) and CORE (C1)
for {set i 0} {$i < $top(NCore)} {incr i} {
	  for {set j 0} {$j < [expr 2*$top(NCore)] } {incr j} {
	      $ns duplex-link $switch_C1_($i) $switch_C2_($j) $top(bw_C1C2) $top(ptime_C1C2) DropTail
		  
		  #Attach queue monitor
	      if { $sim(linkload_stats) == "enabled" } {
		  set qmon_C1_C2_($i-$j) [$ns monitor-queue $switch_C1_($i) $switch_C2_($j) ""]
		  set qmon_C2_C1_($j-$i) [$ns monitor-queue $switch_C2_($j) $switch_C1_($i) ""]
	      }  
	  }
 }

# ACCESS Switches (C3)
set posX_shift [expr $x_topology/[expr $top(NRacks) + 1]]
for {set k 0} {$k < [expr $top(NRacks)] } {incr k} {
     set switch_C3_($k) [$ns node ]
     
     # Switch position
     $switch_C3_($k) set X_ [expr $posX_shift + $k*$posX_shift]
     $switch_C3_($k) set Y_ 200.0	
     $switch_C3_($k) set Z_ 0.0
	
     # Switch energy model
     set energyModel_C3_($k) [new SwitchEnergyModel]
     $energyModel_C3_($k) set eChassis_ 146
     $energyModel_C3_($k) set eLineCard_ 0
     $energyModel_C3_($k) set ePort_ 0.42
     $energyModel_C3_($k) set eSimEnd_ $sim(end_time)

     $energyModel_C3_($k) set eDVFS_enabled_ $switches(eDVFS_enabled)
     $energyModel_C3_($k) set eDNS_enabled_ $switches(eDNS_enabled)
     $energyModel_C3_($k) set eDNS_delay_ $switches(eDNS_delay)
     
     $ns at $sim(start_time) "$energyModel_C3_($k) start"
     $ns at $sim(end_time) "$energyModel_C3_($k) stop"

     set classifier [$switch_C3_($k) set classifier_]
     $classifier attach-energymodel $energyModel_C3_($k)

     # Setup rack objects
     set racks_($k) [new DcRack]
     $racks_($k) set rack_id_ $k
     $racks_($k) set stat_interval $sim(interval)
     $racks_($k) set uplink_B [expr $top(bw_C2C3)]

}

set l 0
# Interconnection of ACCESS (C3) and AGGREGATION (C2)
for {set i 0} {$i < [expr 2*$top(NCore)] } { set i [expr $i + 2] } {
	for {set j [expr $l*$top(NAccess)]} {$j < [expr ($l+1)*$top(NAccess)] } {incr j} {
	     $ns duplex-link $switch_C2_($i) $switch_C3_($j) $top(bw_C2C3) $top(ptime_C2C3) DropTail
	     if { $sim(linkload_stats) == "enabled" } {
		set qmon_C2_C3_($i-$j) [$ns monitor-queue $switch_C2_($i) $switch_C3_($j) ""]
		set qmon_C3_C2_($j-$i) [$ns monitor-queue $switch_C3_($j) $switch_C2_($i) ""]
		
		# Attaching qmon ($j-$i) to rack $j
		$racks_($j) add-uplink-qmon $qmon_C3_C2_($j-$i)

	     }  
	     incr i 	
	     $ns duplex-link $switch_C2_($i) $switch_C3_($j) $top(bw_C2C3) $top(ptime_C2C3) DropTail
	     if { $sim(linkload_stats) == "enabled" } {
		set qmon_C2_C3_($i-$j) [$ns monitor-queue $switch_C2_($i) $switch_C3_($j) ""]
		set qmon_C3_C2_($j-$i) [$ns monitor-queue $switch_C3_($j) $switch_C2_($i) ""]
		
		# Attaching qmon ($j-$i) to rack $j
		$racks_($j) add-uplink-qmon $qmon_C3_C2_($j-$i)
	     }  	     
	     set i [expr $i - 1]	
	}
	set $j [expr $j +$top(NAccess)]
	if {$l< $top(NCore)} {
 		incr l
	}
}

for {set k 0} {$k < [expr $top(NRacks)] } {incr k} {

	$racks_($k) start
}

# Hosts (H) attached to ACCESS switches
puts "Creating $top(NServers) servers..."
set posX_shift [expr $x_topology/$top(NServers) + 1]
for {set k 0} {$k < $top(NServers)} {incr k} {
     set servers_($k) [$ns node ]

     $servers_($k) set X_ [expr $posX_shift + $k*$posX_shift]
     $servers_($k) set Y_ 100.0	
     $servers_($k) set Z_ 0.0
}

# Interconnection of Hosts (H) and ACCESS Switches (C3)
set l 0
for {set i 0} {$i < [expr $top(NCore)*$top(NAccess)] } { incr i } {
	for {set j [expr $l*$top(NRackHosts)]} {$j < [expr ($l+1)*$top(NRackHosts)] } {incr j} {
	     $ns duplex-link $switch_C3_($i) $servers_($j) $top(bw_C3H) $top(ptime_C3H) DropTail
	      
	     
	     if { $sim(linkload_stats) == "enabled" } {
	
		#Attach queue monitor
		set qmon_C3_hosts_($j) [$ns monitor-queue $switch_C3_($i) $servers_($j) ""]
		set qmon_hosts_C3_($j) [$ns monitor-queue $servers_($j) $switch_C3_($i) ""]
	     }
	}
	set $j [expr $j +$top(NRackHosts)]
	if {$l< $top(NCore)*$top(NAccess)} {
 		incr l
	}
}

# Data center node
set datacenter [$ns node]

$datacenter set X_ 500.0
$datacenter set Y_ 500.0
$datacenter set Z_ 0.0

# -----------------------------------------------------------------------------------------------
# -----------------------------------------Data Center 2 ----------------------------------------
# -----------------------------------------------------------------------------------------------

# Set topology size
#set x_topology	6000
#set y_topology	1000
#
#puts ""
#puts ""
#puts "*****************"
#puts "BUILDING TOPOLOGY 2"
#puts "*****************"
#puts ""
#puts "Data center architecture: $sim(dc_type)"
#puts "Creating switches CORE($top(NCore)) AGGREGATION ($top(NAggr)) ACCESS($top(NRacks))..."
#
## Creating Data Center
#set DCenter2 [new DataCenter]
#
#set topoDC2 [new Topography]
#$topoDC load_flatgrid $x_topology $y_topology
#
## CORE Swithces (C1)
#set posX_shift [expr $x_topology/[expr $top(NCore) + 1]]
#for {set i 0} {$i < $top(NCore) } {incr i} {
#     set switch_C1_($i) [$ns node]
#
#     # Switch position
#     $switch_C1_($i) set X_ [expr $posX_shift + $i*$posX_shift]
#     $switch_C1_($i) set Y_ 400.0	
#     $switch_C1_($i) set Z_ 0.0
#
#     # Switch energy model
#     set energyModel_C1_($i) [new SwitchEnergyModel]
#     $energyModel_C1_($i) set eChassis_ $switches(eCore_Chassis)
#     $energyModel_C1_($i) set eLineCard_ $switches(eCore_LineCard)
#     $energyModel_C1_($i) set ePort_ $switches(eCore_Port)
#     $energyModel_C1_($i) set eSimEnd_ $sim(end_time)
#
#     $energyModel_C1_($i) set eDVFS_enabled_ $switches(eDVFS_enabled)
#     $energyModel_C1_($i) set eDNS_enabled_ $switches(eDNS_enabled)
#     $energyModel_C1_($i) set eDNS_delay_ $switches(eDNS_delay)
#     
#     $ns at $sim(start_time) "$energyModel_C1_($i) start"
#     $ns at $sim(end_time) "$energyModel_C1_($i) stop"
#
#     set classifier [$switch_C1_($i) set classifier_]
#     $classifier attach-energymodel $energyModel_C1_($i)
#}
#
## AGGREGATION Switches (C2)
#set posX_shift [expr $x_topology/[expr $top(NAggr) + 1]]
#for {set j 0} {$j < $top(NAggr)} {incr j} {
#     set switch_C2_($j) [$ns node]
#
#     # Switch position
#     $switch_C2_($j) set X_ [expr $posX_shift + $j*$posX_shift]
#     $switch_C2_($j) set Y_ 300.0	
#     $switch_C2_($j) set Z_ 0.0
#
#     # Switch energy model
#     set energyModel_C2_($j) [new SwitchEnergyModel]
#     $energyModel_C2_($j) set eChassis_ $switches(eAggr_Chassis)
#     $energyModel_C2_($j) set eLineCard_ $switches(eAggr_LineCard)
#     $energyModel_C2_($j) set ePort_ $switches(eAggr_Port)
#     $energyModel_C2_($j) set eSimEnd_ $sim(end_time)
#
#     $energyModel_C2_($j) set eDVFS_enabled_ $switches(eDVFS_enabled)
#     $energyModel_C2_($j) set eDNS_enabled_ $switches(eDNS_enabled)
#     $energyModel_C2_($j) set eDNS_delay_ $switches(eDNS_delay)
#     
#     $ns at $sim(start_time) "$energyModel_C2_($j) start"
#     $ns at $sim(end_time) "$energyModel_C2_($j) stop"
#
#     set classifier_C2_ [$switch_C2_($j) set classifier_]
#     $classifier attach-energymodel $energyModel_C2_($j)		
#}
#
#  # Three-tier and three-tier high-speed
#  # Interconnection of AGGREGATION (C2) and CORE (C1)
#for {set i 0} {$i < $top(NCore)} {incr i} {
#	  for {set j 0} {$j < [expr 2*$top(NCore)] } {incr j} {
#	      $ns duplex-link $switch_C1_($i) $switch_C2_($j) $top(bw_C1C2) $top(ptime_C1C2) DropTail
#		  
#		  #Attach queue monitor
#	      if { $sim(linkload_stats) == "enabled" } {
#		  set qmon_C1_C2_($i-$j) [$ns monitor-queue $switch_C1_($i) $switch_C2_($j) ""]
#		  set qmon_C2_C1_($j-$i) [$ns monitor-queue $switch_C2_($j) $switch_C1_($i) ""]
#	      }  
#	  }
# }
#
## ACCESS Switches (C3)
#set posX_shift [expr $x_topology/[expr $top(NRacks) + 1]]
#for {set k 0} {$k < [expr $top(NRacks)] } {incr k} {
#     set switch_C3_($k) [$ns node ]
#     
#     # Switch position
#     $switch_C3_($k) set X_ [expr $posX_shift + $k*$posX_shift]
#     $switch_C3_($k) set Y_ 200.0	
#     $switch_C3_($k) set Z_ 0.0
#	
#     # Switch energy model
#     set energyModel_C3_($k) [new SwitchEnergyModel]
#     $energyModel_C3_($k) set eChassis_ 146
#     $energyModel_C3_($k) set eLineCard_ 0
#     $energyModel_C3_($k) set ePort_ 0.42
#     $energyModel_C3_($k) set eSimEnd_ $sim(end_time)
#
#     $energyModel_C3_($k) set eDVFS_enabled_ $switches(eDVFS_enabled)
#     $energyModel_C3_($k) set eDNS_enabled_ $switches(eDNS_enabled)
#     $energyModel_C3_($k) set eDNS_delay_ $switches(eDNS_delay)
#     
#     $ns at $sim(start_time) "$energyModel_C3_($k) start"
#     $ns at $sim(end_time) "$energyModel_C3_($k) stop"
#
#     set classifier [$switch_C3_($k) set classifier_]
#     $classifier attach-energymodel $energyModel_C3_($k)
#
#     # Setup rack objects
#     set racks_($k) [new DcRack]
#     $racks_($k) set rack_id_ $k
#     $racks_($k) set stat_interval $sim(interval)
#     $racks_($k) set uplink_B [expr $top(bw_C2C3)]
#
#}
#
#set l 0
## Interconnection of ACCESS (C3) and AGGREGATION (C2)
#for {set i 0} {$i < [expr 2*$top(NCore)] } { set i [expr $i + 2] } {
#	for {set j [expr $l*$top(NAccess)]} {$j < [expr ($l+1)*$top(NAccess)] } {incr j} {
#	     $ns duplex-link $switch_C2_($i) $switch_C3_($j) $top(bw_C2C3) $top(ptime_C2C3) DropTail
#	     if { $sim(linkload_stats) == "enabled" } {
#		set qmon_C2_C3_($i-$j) [$ns monitor-queue $switch_C2_($i) $switch_C3_($j) ""]
#		set qmon_C3_C2_($j-$i) [$ns monitor-queue $switch_C3_($j) $switch_C2_($i) ""]
#		
#		# Attaching qmon ($j-$i) to rack $j
#		$racks_($j) add-uplink-qmon $qmon_C3_C2_($j-$i)
#
#	     }  
#	     incr i 	
#	     $ns duplex-link $switch_C2_($i) $switch_C3_($j) $top(bw_C2C3) $top(ptime_C2C3) DropTail
#	     if { $sim(linkload_stats) == "enabled" } {
#		set qmon_C2_C3_($i-$j) [$ns monitor-queue $switch_C2_($i) $switch_C3_($j) ""]
#		set qmon_C3_C2_($j-$i) [$ns monitor-queue $switch_C3_($j) $switch_C2_($i) ""]
#		
#		# Attaching qmon ($j-$i) to rack $j
#		$racks_($j) add-uplink-qmon $qmon_C3_C2_($j-$i)
#	     }  	     
#	     set i [expr $i - 1]	
#	}
#	set $j [expr $j +$top(NAccess)]
#	if {$l< $top(NCore)} {
#		 incr l
#	}
#}
#
#for {set k 0} {$k < [expr $top(NRacks)] } {incr k} {
#
#	$racks_($k) start
#}
#
## Hosts (H) attached to ACCESS switches
#puts "Creating $top(NServers) servers..."
#set posX_shift [expr $x_topology/$top(NServers) + 1]
#for {set k 0} {$k < $top(NServers)} {incr k} {
#     set servers_($k) [$ns node ]
#
#     $servers_($k) set X_ [expr $posX_shift + $k*$posX_shift]
#     $servers_($k) set Y_ 100.0	
#     $servers_($k) set Z_ 0.0
#}
#
## Interconnection of Hosts (H) and ACCESS Switches (C3)
#set l 0
#for {set i 0} {$i < [expr $top(NCore)*$top(NAccess)] } { incr i } {
#	for {set j [expr $l*$top(NRackHosts)]} {$j < [expr ($l+1)*$top(NRackHosts)] } {incr j} {
#	     $ns duplex-link $switch_C3_($i) $servers_($j) $top(bw_C3H) $top(ptime_C3H) DropTail
#	      
#	     
#	     if { $sim(linkload_stats) == "enabled" } {
#	
#		#Attach queue monitor
#		set qmon_C3_hosts_($j) [$ns monitor-queue $switch_C3_($i) $servers_($j) ""]
#		set qmon_hosts_C3_($j) [$ns monitor-queue $servers_($j) $switch_C3_($i) ""]
#	     }
#	}
#	set $j [expr $j +$top(NRackHosts)]
#	if {$l< $top(NCore)*$top(NAccess)} {
#		 incr l
#	}
#}
#
## Data center node
#set datacenter2 [$ns node]
#
#$datacenter2 set X_ 4500.0
#$datacenter2 set Y_ 500.0
#$datacenter2 set Z_ 0.0

# -----------------------------------------------------------------------------------------------
# --------------------------Finishing building topology ----------------------------------------
# -----------------------------------------------------------------------------------------------
