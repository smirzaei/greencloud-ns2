# ------------------------------------------------------------------------------------------------
# -------------Creating DataCenter Resources, DcHosts, VMs, TskComAgents, TskComSinks-------------
# ------------------------------------------------------------------------------------------------



# The list must contain all required resource specification files.
#Used resource library:
set resourceFiles {"resSpecs/cpu_basic.tcl" "resSpecs/cpu_basic_low_power.tcl" "resSpecs/cpu_basic2.tcl" "resSpecs/cpu_commodity_4c.tcl" "resSpecs/cpu_high_end_8c.tcl" "resSpecs/cpu_micro_4c.tcl" "resSpecs/mem_basic2GB.tcl" "resSpecs/mem_basic4GB.tcl" "resSpecs/mem_basic6GB.tcl" "resSpecs/mem_basic8GB.tcl" "resSpecs/mem_basic10GB.tcl" "resSpecs/mem_basic32GB.tcl" "resSpecs/disk_basic.tcl" "resSpecs/disk_500GB.tcl" "resSpecs/disk_1000GB.tcl" "resSpecs/eth_basic.tcl"}
#Used virtual resource library:
set virtualResourceFiles {"resSpecs/virtual/vcpu_basic.tcl" "resSpecs/virtual/vcpu_basic_4core.tcl" "resSpecs/virtual/vmem_basic1GB.tcl" "resSpecs/virtual/vmem_basic2GB.tcl"  "resSpecs/virtual/vmem_basic4GB.tcl" "resSpecs/virtual/vmem_basic6GB.tcl" "resSpecs/virtual/vmem_basic8GB.tcl" "resSpecs/virtual/vdisk_basic.tcl" "resSpecs/virtual/vdisk_basic100GB.tcl" "resSpecs/virtual/veth_basic.tcl" }



puts "Loading resource specifications configuration files..."

#proc loadComponentPowerModel {file_path} {
#	source $file_path
#	set compPowerModels_($next_comp_model_id) $pModel
#	incr $next_comp_model_id
#	puts $next_comp_model_id
#}

set i 0
foreach resFile $resourceFiles {
	source $resFile
	set resSpecification($i) $resSpec
	$DCenter add-resspec $resSpecification($i)
	incr i
	
}

set i 0
foreach vResFile $virtualResourceFiles {
	source $vResFile
	set vResSpecification($i) $resSpec
	$DCenter add-vresspec $vResSpecification($i)
	incr i
	
}


# Creating TaskComAgents
# Create Agents, equal to no of Servers
for {set k 0} {$k < [expr $top(NAccess)*$top(NCore)*$top(NRackHosts)] } {incr k} {
	set tskcomagnt_C_($k) [new Agent/TskComAgent]
	$tskcomagnt_C_($k) set packetSize_ 1500
	
	#Add pointers to Agents in DataCenter
	$DCenter add-hosttaskagent $tskcomagnt_C_($k)
}

# Three-tier and three-tier high-speed
# Attach Agents to L1 switches equally
set NTSwitches $top(NCore)
set m [expr $top(NAccess)*$top(NRackHosts)]
for {set j 0} {$j < $top(NCore) } {incr j} {
   for {set jj 0} {$jj < $m } {incr jj} {
 	  set m_pos [expr $m*$j+$jj]
	  $ns attach-agent $switch_C1_($j) $tskcomagnt_C_($m_pos)
   }
}

# Set scheduler:
$DCenter set-scheduler $dc(scheduler)


$DCenter set mips_capacity_ 0


# Create TskComSinks equal to Number of L4 Servers
for {set k 0} {$k < $top(NServers)} {incr k} {

	# Create task receivers
	set tsksink_($k) [new Agent/TskComSink]
	$ns attach-agent $servers_($k) $tsksink_($k)
	
	# Task output connections
	set tskoutputagent_($k) [new Agent/TCP/ProvOutAgent]; #[$ns create-connection TCP $servers_($k) TCPSink $switch_C1_([expr $k/($top(NServers)/$NTSwitches)]) 01]
	$ns attach-agent $servers_($k) $tskoutputagent_($k)
	set tskoutputsink_($k) [ new Agent/TCPSink/TskOutSink ]
	$ns attach-agent $switch_C1_([expr $k/($top(NServers)/$NTSwitches)]) $tskoutputsink_($k)
	$ns connect $tskoutputagent_($k) $tskoutputsink_($k)
	$tskoutputsink_($k) connect-tskoutagent $tskoutputagent_($k)
	$tskoutputagent_($k) set packetSize_ 1500
	$tskoutputagent_($k) set window_ 32

	# Task output connections
#	set tskoutputagent_($k) [$ns create-connection TCP $servers_($k) TCPSink $switch_C1_([expr $k/($top(NServers)/$NTSwitches)]) 01]
#	set tskoutputsink_($k) [new Agent/TskComSink]
#	$tskoutputagent_($k) set packetSize_ 1500
#	$tskoutputagent_($k) set window_ 32
	
	
	# Create DcHosts and send pointers to corresponding TskComSink
	set hosts_($k) [new DcHost]
	$hosts_($k) set id_ $k
	$hosts_($k) set eDVFS_enabled_ $serv(eDVFS_enabled)
	$hosts_($k) set eDNS_enabled_ $serv(eDNS_enabled)

	$hosts_($k) attach-agent $tskoutputagent_($k)

	$tsksink_($k) connect-resprovider $hosts_($k)
	
	$hosts_($k) set-taskcomagent $tskcomagnt_C_($k)
	#Set power model:
	source "powerModels/full_system/perComponentPowerModel.tcl"
#	source "powerModels/full_system/linearPowerModel.tcl"
#	OR e.g.
#	source "powerModels/full_system/lowPowerBladePModel.tcl"
	set powerModels_($k) $pModel
	$hosts_($k) set-power-model $powerModels_($k)
	
	#Computing
	set DCResCPU($k) [new CPU]
	
	switch -regexp $sim(dc_type) {
	  "three-tier heterogenous(.*)" {
		  
		  switch $sim(dc_type) {
			  "three-tier heterogenous debug" { 
				set het(commodity) 48
				set het(high-end) 12
			  } 
			  default {
				set het(commodity) 512
				set het(high-end) 128 
			  }
		  }

	#Uncomment for mixed topology:
	if { $k  < $het(commodity) } {
	#Commodity
	#Computing	
		$DCenter configure-resource $DCResCPU($k) "Commodity processor 4 cores"
		#Memory
		set DCResMem($k) [new DcResource]
		$DCenter configure-resource $DCResMem($k) "Nominal 8GB memory"
		$hosts_($k) add-resource $DCResMem($k)
	#Storage
		set DCResStor($k) [new DcResource]
		$DCenter configure-resource $DCResStor($k) "Nominal 500GB disk"
		$hosts_($k) add-resource $DCResStor($k)
	} elseif { $k  < [expr $het(commodity) + $het(high-end)]  } {
	#High-end 
	#Computing
		$DCenter configure-resource $DCResCPU($k) "High-end processor 8 cores"
	#Memory
		set DCResMem($k) [new DcResource]
		$DCenter configure-resource $DCResMem($k) "Nominal 32GB memory"
		$hosts_($k) add-resource $DCResMem($k)
	#Storage	
		set DCResStor($k) [new DcResource]
		$DCenter configure-resource $DCResStor($k) "Nominal 1000GB disk"
		$hosts_($k) add-resource $DCResStor($k)	
#		$DCenter configure-resource $DCResCPU($k) "Nominal processor 2"; # Heterogenous processors
#		$DCenter configure-resource $DCResCPU($k) "Nominal processor low power"; # Heterogenous power consumption
	} else {
	#Micro
	#Computing
		$DCenter configure-resource $DCResCPU($k) "Micro processor 4 cores"
	#Memory
		set DCResMem($k) [new DcResource]
		$DCenter configure-resource $DCResMem($k) "Nominal 8GB memory"
		$hosts_($k) add-resource $DCResMem($k)
	#No Storage
#		set DCResStor($k) [new DcResource]
#		$DCenter configure-resource $DCResStor($k) "Nominal 500GB disk"
#		$hosts_($k) add-resource $DCResStor($k)
	}
		  
	} 
	default	{
	#Commodity
	#Computing	
		$DCenter configure-resource $DCResCPU($k) "Commodity processor 4 cores"
		#Memory
		set DCResMem($k) [new DcResource]
		$DCenter configure-resource $DCResMem($k) "Nominal 8GB memory"
		$hosts_($k) add-resource $DCResMem($k)
	#Storage
		set DCResStor($k) [new DcResource]
		$DCenter configure-resource $DCResStor($k) "Nominal 500GB disk"
		$hosts_($k) add-resource $DCResStor($k)
	}
	}

	$hosts_($k) add-resource $DCResCPU($k)
	
	$DCResCPU($k) getMIPS
 	$DCenter set mips_capacity_ [expr [$DCenter set mips_capacity_] +  $tmp_cpu_mips]
	
	#Memory
	set DCResMem($k) [new DcResource]
	$DCenter configure-resource $DCResMem($k) "Nominal 8GB memory"
	$hosts_($k) add-resource $DCResMem($k)
	
	#Networking
	set DCResNet($k) [new NIC]
	$DCenter configure-resource $DCResNet($k) "Nominal 1GbE NIC"
	$hosts_($k) add-resource $DCResNet($k)
	
	
#	$hosts_($k) print
	
	# Place DcHosts pointers in Data Center
	$DCenter add-dchost $hosts_($k)
	
	# Add hosts into appropriate racks
	$racks_([expr $k/$top(NRackHosts)]) add-dchost $hosts_($k)
		
	# Schedule start and stop for energy tracking
	$ns at $sim(start_time) "$hosts_($k) start"
	$ns at $sim(end_time) "$hosts_($k) stop"
}

# Interconnect TskComAgents(L1) to TskComSinks(L4) 
for {set k 0} {$k < [expr $top(NAccess)*$top(NCore)*$top(NRackHosts)] } {incr k} {
	$ns connect $tskcomagnt_C_($k) $tsksink_($k)
}

# -----------------------------------------------------------------------------------------------
#----------------------------------VM static configuration---------------------------------------
# -----------------------------------------------------------------------------------------------
if { $vm(static_virtualization) == 1 } {

# use vms list to perfrom scheduling
  $DCenter schedule-on-vms

# Create number of VMs equal to number of servers.
	
puts "VM static configuration..."
# Creating TaskComAgents
# Create Agents, equal to no of VMs
for {set k 0} {$k < $top(NServers) } {incr k} {
	set vmtskcomagnt_C_($k) [new Agent/TskComAgent]
	$vmtskcomagnt_C_($k) set packetSize_ 1500
	
	#Add pointers to Agents in DataCenter
	$DCenter add-vmtaskagent $vmtskcomagnt_C_($k)
}

# Three-tier and three-tier high-speed
# Attach Agents to L1 switches equally
set NTSwitches $top(NCore)
set m [expr $top(NAccess)*$top(NRackHosts)]
for {set j 0} {$j < $top(NCore) } {incr j} {
   for {set jj 0} {$jj < $m } {incr jj} {
	   set m_pos [expr $m*$j+$jj]
	  $ns attach-agent $switch_C1_($j) $vmtskcomagnt_C_($m_pos)
   }
}

#configure virtual resources of each vm 
for {set k 0} {$k < $top(NServers)} {incr k} {
	
	# Create task receivers
	set vmtsksink_($k) [new Agent/TskComSink]
	$ns attach-agent $servers_($k) $vmtsksink_($k)

	# Task output connections
	set vmtskoutputagent_($k) [new Agent/TCP/ProvOutAgent]; #[$ns create-connection TCP $servers_($k) TCPSink $switch_C1_([expr $k/($top(NServers)/$NTSwitches)]) 01]
	$ns attach-agent $servers_($k) $vmtskoutputagent_($k)
	set vmtskoutputsink_($k) [ new Agent/TCPSink/TskOutSink ]
	$ns attach-agent $switch_C1_([expr $k/($top(NServers)/$NTSwitches)]) $vmtskoutputsink_($k)
	$ns connect $vmtskoutputagent_($k) $vmtskoutputsink_($k)
	$vmtskoutputsink_($k) connect-tskoutagent $vmtskoutputagent_($k)
	$vmtskoutputagent_($k) set packetSize_ 1500
	$vmtskoutputagent_($k) set window_ 32
	
	set vms_($k) [new VM]
	$vms_($k) set id_ $k
	#The DVFS settings affects scheduling behavior of vms.
	$vms_($k) set eDVFS_enabled_ $vm(eDVFS_enabled)

	$vms_($k) attach-agent $vmtskoutputagent_($k)
	$vmtsksink_($k) connect-resprovider $vms_($k)
	
	$vms_($k) set-taskcomagent $vmtskcomagnt_C_($k)
	
	#VCPU
	set VDCResCPU($k) [new CPU]
	$DCenter configure-vresource $VDCResCPU($k) "Nominal vcpu 4 core"
	$vms_($k) add-resource $VDCResCPU($k)
	
	#VMem
	set VDCResMem($k) [new DcResource]
	$DCenter configure-vresource $VDCResMem($k) "Nominal 1GB virtual memory"
	$vms_($k) add-resource $VDCResMem($k)
	
	#VDisk
	set VDCResStor($k) [new DcResource]
	$DCenter configure-vresource $VDCResStor($k) "Nominal 100GB virtual disk"
	$vms_($k) add-resource $VDCResStor($k)
	
	set VDCResNet($k) [new NIC]
	$DCenter configure-vresource $VDCResNet($k) "Nominal 1GbE virtual NIC"
	$vms_($k) add-resource $VDCResNet($k) 
	

	# Place DcHosts pointers into Data Center
	$DCenter add-vm $vms_($k)
	#Allocate vm on its hosts
	$hosts_($k) add-vm $vms_($k)
	
	$vms_($k) start	
	
	incr virt(NVms)
}

# Interconnect TskComAgents(L1) to TskComSinks(L4) 
for {set k 0} {$k < [expr $top(NAccess)*$top(NCore)*$top(NRackHosts)] } {incr k} {
	$ns connect $vmtskcomagnt_C_($k) $vmtsksink_($k)
}
}

# Dynamically configure the initial VM placement (Not available yet).
#	$DCenter initially-configure-vms


# -----------------------------------------------------------------------------------------------
# -------------------------------------Creation Complete-----------------------------------------
# -----------------------------------------------------------------------------------------------


# -----------------------------------------------------------------------------------------------
# -------------------------------------Static migration -----------------------------------------
# -----------------------------------------------------------------------------------------------

# Migration sandbox:
# ensure that this is a valid move! (tip: use CPU config: "Nominal processor 2" 
# to enable hosting 2 or more nomnial vCPUs on a host)

# puts "Testing VM migration..."
# $ns at 1.0 "$DCenter migrate-vm $vms_(0) $hosts_(4)"
# puts "Migration initialized correctly"
 




