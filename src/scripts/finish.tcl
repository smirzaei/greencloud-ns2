# --------------------------------------------------------------------
# ------------- Post-simulation procedures and reporting -------------
# --------------------------------------------------------------------

proc finish {} {
  global ns nf DCenter graph avg top hosts_ qmon_C3_hosts_ qmon_hosts_C3_ qmon_C2_C3_ qmon_C3_C2_ qmon_C1_C2_ qmon_C2_C1_ 
  global sim switch_C1_ energyModel_C1_ switch_C2_ energyModel_C2_ switch_C3_ energyModel_C3_ hosts_ task racks_ psrv pswitches vms_ virt
  $ns flush-trace
  close $nf

  puts "******************"
  puts "SIMULATION REPORTS"
  puts "******************"
  puts "- "
  puts "Total tasks submitted: [$DCenter set tskSubmitted_]"
  puts [format "DC load: %.1f%%" [expr $avg(dcload)/$avg(samples)*100]]
  puts "- "

  # Compute Total Energy Consumed
  # Energy of Servers
  set EServers 0
  for {set k 0} {$k < $top(NServers)} {incr k} {
    set eserver [$hosts_($k) set eConsumed_]
    set EServers [expr $EServers + $eserver]
    puts $graph(eServers) "$k $eserver"
  }
  puts [format "Energy consumed by servers: %.1f W*h" $EServers]

  # Energy of Core switches
  set ECoreSwitches 0
  for {set i 0} {$i < $top(NCore) } {incr i} {
    set ecoreswitch [$energyModel_C1_($i) set eConsumed_]
    set ECoreSwitches [expr $ECoreSwitches + $ecoreswitch]
    puts $graph(eCoreSwitches) "$i $ecoreswitch"
  }

  # Energy of Aggregation switches
  set EAggrSwitches 0
  for {set i 0} {$i < $top(NAggr) } {incr i} {
    set eaggrswitch [$energyModel_C2_($i) set eConsumed_]
    set EAggrSwitches [expr $EAggrSwitches + $eaggrswitch]
    puts $graph(eAggrSwitches) "$i $eaggrswitch"
  }

  # Energy of Access switches
  set EAccessSwitches 0
  for {set i 0} {$i < [expr $top(NAccess)*$top(NCore)] } {incr i} {
    set eaccessswitch [$energyModel_C3_($i) set eConsumed_]
    set EAccessSwitches [expr $EAccessSwitches + $eaccessswitch]
    puts $graph(eAccessSwitches) "$i $eaccessswitch"
  }

  puts [format "Energy consumed by switches: Core(%.1f W*h) Aggregation(%.1f W*h) Access(%.1f W*h) - %.1f W*h" $ECoreSwitches $EAggrSwitches $EAccessSwitches [expr $ECoreSwitches + $EAggrSwitches + $EAccessSwitches]]

  puts [format "Total energy consumed (servers + switches): %.1f W*h" [expr $EServers + $ECoreSwitches + $EAggrSwitches + $EAccessSwitches]]

  # Average load of servers: tasks per server
  set AvgTasksperServer 0
  set TotFailedServerTask 0
  for {set k 0} {$k < $top(NServers)} {incr k} {
    set srvTasks [$hosts_($k) set ntasks_]
    set AvgTasksperServer [expr $AvgTasksperServer + $srvTasks]
    puts $graph(dcServTasks) "$k $srvTasks"
    set srvFailedTasks [$hosts_($k) set  tskFailed_]
    set TotFailedServerTask [expr $TotFailedServerTask + $srvFailedTasks]
    puts $graph(dcServTasksFailed) "$k $srvFailedTasks"
  }
  puts ""
  puts [format "Average tasks per server: %d " [expr $AvgTasksperServer/$top(NServers)]]


  # Average load of servers
  set AvgLoadperServer 0
  set AvgLoadperServerMem 0
  set AvgLoadperServerStor 0
  for {set k 0} {$k < $top(NServers)} {incr k} {
    set AvgLoadperServer [expr $AvgLoadperServer + $avg(servload-$k)/$avg(samples)]
    puts $graph(dcServLoad) "$k [expr $avg(servload-$k)/$avg(samples)]"
    set AvgLoadperServerMem [expr $AvgLoadperServerMem + $avg(servloadmem-$k)/$avg(samples)]
    puts $graph(dcServLoadMem) "$k [expr $avg(servloadmem-$k)/$avg(samples)]"
    set AvgLoadperServerStor [expr $AvgLoadperServerStor + $avg(servloadstor-$k)/$avg(samples)]
    puts $graph(dcServLoadStor) "$k [expr $avg(servloadstor-$k)/$avg(samples)]"

  }
  puts [format "Average CPU load per server: %.1f" [expr $AvgLoadperServer/$top(NServers)]]
  puts [format "Average Memory load per server: %.1f" [expr $AvgLoadperServerMem/$top(NServers)]]
  puts [format "Average Storage load per server: %.1f" [expr $AvgLoadperServerStor/$top(NServers)]]
  # Total number of tasks failed on data center scheduler level

  puts [format "Tasks failed on servers: %d" [expr $TotFailedServerTask] ]

  # Total number of tasks failed on scheduler level
  puts "Tasks failed (rejected) on DC scheduler level: [$DCenter set tskFailed_]"
  
	
  # Average load of VMs
  set AvgLoadperVm 0
  set AvgLoadperVmMem 0
  set AvgLoadperVmStor 0
  set TotFailedVmTask 0
  for {set k 0} {$k < $virt(NVms)} {incr k} {
  # TODO: adapt $avg(samples) to running time of each VM!
    set vmTasks [$vms_($k) set ntasks_]
    puts $graph(dcVmTasks) "$k $vmTasks"
    set AvgLoadperVm [expr $AvgLoadperVm + $avg(vmload-$k)/$avg(samples)]
    puts $graph(dcVmLoad) "$k [expr $avg(vmload-$k)/$avg(samples)]"
    set AvgLoadperVmMem [expr $AvgLoadperVmMem + $avg(vmloadmem-$k)/$avg(samples)]
    puts $graph(dcVmLoadMem) "$k [expr $avg(vmloadmem-$k)/$avg(samples)]"
    set AvgLoadperVmStor [expr $AvgLoadperVmStor + $avg(vmloadstor-$k)/$avg(samples)]
    puts $graph(dcVmLoadStor) "$k [expr $avg(vmloadstor-$k)/$avg(samples)]"
    set vmFailedTasks [$vms_($k) set  tskFailed_]
    set TotFailedVmTask [expr $TotFailedVmTask + $vmFailedTasks]
    puts $graph(dcVmTasksFailed) "$k $vmFailedTasks"
  }
global vm
    if { $vm(static_virtualization) == 1 } {
    	puts [format "Average load per vm: %.1f" [expr $AvgLoadperVm/$virt(NVms)]]
	    
	puts [format "Tasks failed on vms: %d" [expr $TotFailedVmTask] ]
    }
	
  # Average load of H-C3 queues
  for {set k 0} {$k < $top(NServers)} {incr k} {
    puts $graph(queueHC3-pkts-avg) "$k [expr $avg(queueHC3_pkts-$k)/$avg(samples)]"
  }

  if { $sim(linkload_stats) == "enabled" } {

    # Links C3-Hosts and Links Hosts-C3
    for {set k 0} {$k < $top(NServers)} {incr k} {
      set bdeparturesC3H [$qmon_C3_hosts_($k) set bdepartures_tot_]
      set butilC3H [expr (1.00*$bdeparturesC3H*8*100/$sim(tot_time))/$top(bw_C3H)]
      puts $graph(LinkC3H_load) "$k $butilC3H"

      set bdeparturesHC3 [$qmon_hosts_C3_($k) set bdepartures_tot_]
      set butilHC3 [expr (1.00*$bdeparturesHC3*8*100/$sim(tot_time))/$top(bw_C3H)]
      puts $graph(LinkHC3_load) "$k $butilHC3"
    }

    # Links C1-C2 and links C2-C1
    for {set i 0} {$i < $top(NCore)} {incr i} {
      for {set j 0} {$j < [expr 2*$top(NCore)] } {incr j} {
        set bdepartures [$qmon_C1_C2_($i-$j) set bdepartures_tot_]
        set butil [expr (1.00*$bdepartures*8*100/$sim(tot_time))/$top(bw_C1C2)]
        puts $graph(LinkC1C2_load) "$i $butil"

        set bdepartures [$qmon_C2_C1_($j-$i) set bdepartures_tot_]
        set butil [expr (1.00*$bdepartures*8*100/$sim(tot_time))/$top(bw_C1C2)]
        puts $graph(LinkC2C1_load) "$i $butil"
      }
    }

    # Links C2-C3
    set l 0
    for {set i 0} {$i < [expr 2*$top(NCore)] } { set i [expr $i + 2] } {
      for {set j [expr $l*$top(NAccess)]} {$j < [expr ($l+1)*$top(NAccess)] } {incr j} {
        set bdepartures [$qmon_C2_C3_($i-$j) set bdepartures_tot_]
        set butil [expr 1.00*$bdepartures*8*100/$sim(tot_time)/$top(bw_C2C3)]
        puts $graph(LinkC2C3_load) "$j $butil"

        incr i

        set bdepartures [$qmon_C2_C3_($i-$j) set bdepartures_tot_]
        set butil [expr 1.00*$bdepartures*8*100/$sim(tot_time)/$top(bw_C2C3)]
        #puts $graph(LinkC2C3_load) "$i$j $butil"

        set i [expr $i - 1]
      }
      set $j [expr $j +$top(NAccess)]
      if {$l< $top(NCore)} {
        incr l
      }
    }

    # Uplink utilization of racks (C3-C2)
    for {set k 0} {$k < [expr $top(NRacks)] } {incr k} {
      $racks_($k) update-stats
      set bdepartures [$racks_($k) set breceived_]
      set butil [expr 1.00*$bdepartures*8*100/$sim(tot_time)/$top(bw_C2C3)]
      puts $graph(LinkC3C2_load) "$k $butil"
    }

  }

  # users statistics
  global clouduser_
  for {set i 0} {$i < $top(NCloudUsers) } {incr i} {
#	$clouduser_($i) print-tasks-status
	$clouduser_($i) post-simulation-test-tasks
	$clouduser_($i) calculate-statistics
	  puts -nonewline	$graph(users)	"[$clouduser_($i) set id_] "
	  puts -nonewline	$graph(users)	"[format "%.4f " [$clouduser_($i) set mean_response_time_] ]" 
	  puts -nonewline	$graph(users)	"[format "%.2e " [$clouduser_($i) set sd_response_time_] ]"
	  puts 			$graph(users)	"[ $clouduser_($i) set unfinished_tasks_]"
  }

  # output a concise version for the dashboard data collector
  puts $graph(loadSummary) [format "load.average %.1f" [expr $AvgLoadperServer/$top(NServers)]]
  puts -nonewline $graph(loadSummary) [format "load.datacenter %.1f" [expr $avg(dcload)/$avg(samples)*100]]
  puts $graph(loadSummary) " %"
  puts $graph(taskSummary) "tasks.total [$DCenter set tskSubmitted_]"
  puts $graph(taskSummary) [format "tasks.average %.1f" [expr double([$DCenter set tskSubmitted_])/double($top(NServers))]]
  puts $graph(taskSummary) "tasks.failed.dc [$DCenter set tskFailed_]"
  puts $graph(taskSummary) "tasks.failed.servers [expr $TotFailedServerTask ]"
	
	
  puts $graph(energySummary) [format "energy.switches.core %.1f" $ECoreSwitches]
  puts $graph(energySummary) [format "energy.switches.aggregation %.1f" $EAggrSwitches]
  puts $graph(energySummary) [format "energy.switches.access %.1f" $EAccessSwitches]
  puts $graph(energySummary) [format "energy.servers %.1f" $EServers]
  
  # TODO clock is disabled because it's causing problems with the auto-loader command:
  # "source -encoding utf-8 [file join $TclLibDir clock.tcl]"
  #puts $graph(parameters) "time.simulation.end' [clock format [clock seconds]]"
  puts $graph(simulation) "time.simulation.duration $sim(tot_time)"
  puts $graph(parameters) "architecture.datacenter $sim(dc_type)"
  puts $graph(parameters) "count.switches.core $top(NCore)"
  puts $graph(parameters) "count.switches.aggregation $top(NAggr)"
  puts $graph(parameters) "count.switches.access $top(NAccess)"
  puts $graph(parameters) "count.servers $top(NServers)"
  puts $graph(parameters) "count.users $top(NCloudUsers)"
  puts $graph(parameters) "powermanagement.servers $psrv"
  puts $graph(parameters) "powermanagement.switches $pswitches"
  puts $graph(parameters) "task.mips $task(mips)"
  puts $graph(parameters) "task.memory $task(memory)"
  puts $graph(parameters) "task.storage $task(storage)"
  puts $graph(parameters) "task.size $task(size)"
  puts $graph(parameters) "task.outputsize $task(outputsize)"
  #puts $graph(parameters) "scheduler $dc(scheduler)"

  puts "  "
  puts "***************"
  puts "BUILDING GRAPHS"
  puts "***************"
  puts ""

  #Close files
  close $graph(DCload)
  close $graph(DCloadMem)
  close $graph(DCloadStor)
  close $graph(DCpower)
  close $graph(eServers)
  close $graph(LinkC1C2_load)
  close $graph(LinkC2C1_load)
  close $graph(LinkC2C3_load)
  close $graph(LinkC3C2_load)
  close $graph(LinkC3H_load)
  close $graph(LinkHC3_load)
  close $graph(eCoreSwitches)
  close $graph(eAggrSwitches)
  close $graph(eAccessSwitches)
  close $graph(dcServTasks)
  close $graph(dcServTasksFailed)
  close $graph(dcVmTasksFailed)
  close $graph(dcServLoad)
  close $graph(dcServLoadMem) 
  close $graph(dcServLoadStor) 
  close $graph(dcVmLoad)
  close $graph(dcVmLoadMem)
  close $graph(dcVmLoadStor)
  close $graph(dcVmTasks)	
  close $graph(linkHC3_load_time)
  close $graph(serv_load_time)
  close $graph(QueueHC3_pkts)
  close $graph(QueueC3C2-0_pkts)
  close $graph(queueHC3-pkts-avg)
  close $graph(users)

  close $graph(loadSummary)
  close $graph(energySummary)
  close $graph(parameters)
	
  $DCenter clear
  
  exit 0
}
