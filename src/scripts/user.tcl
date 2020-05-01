# -----------------------------------------------------------------------------------------------
# -------------Creating DataCenter CloudUsers					    -------------
# -----------------------------------------------------------------------------------------------

puts ""
puts "*********************"
puts "Creating cloud users"
puts "*********************"
puts "Data center total computing capacity: [$DCenter set mips_capacity_] MIPS"
# Number of cloud users
set top(NCloudUsers)  1

# Compute task generation rate
set task(genrate) [expr [$DCenter set mips_capacity_]/$task(mips)*$dc(target_load)]	;# Number of tasks to be generated per second to maintain target Data Center load
set task(netrate) [expr $task(genrate)*$task(size)*8]					;# Required bitrate

# Creating Cloud Users
puts "Creating $top(NCloudUsers) cloud user(s)..."
for {set i 0} {$i < $top(NCloudUsers) } {incr i} {
	set clouduser_($i) [new Application/Traffic/ExpCloudUser]
	$clouduser_($i) set id_ $i
	$clouduser_($i) set packetSize_ $task(size)
	$clouduser_($i) set tskmips_ $task(mips)
	$clouduser_($i) set memory_ $task(memory)
	$clouduser_($i) set storage_ $task(storage)
	$clouduser_($i) set tsksize_ $task(size)
	$clouduser_($i) set tskmaxduration_ $task(duration)
	$clouduser_($i) set toutputsize_ $task(outputsize)
	$clouduser_($i) set tintercom_ $task(intercom)

	$clouduser_($i) set burst_time_ 950ms
	$clouduser_($i) set idle_time_ 50ms
# 	Use this values if you would like to test more variable load:	
#	$clouduser_($i) set burst_time_ 2000ms
#	$clouduser_($i) set idle_time_ 1000ms
	$clouduser_($i) set rate_ $task(netrate)

# 	Example of scripting chnge in the task generation rate:
#	$ns at 30.0 "$clouduser_($i) set-rate [expr $task(netrate)*2]"
	
	$clouduser_($i) set-randomized 0

	$clouduser_($i) join-datacenter $DCenter
	


}

# -----------------------------------------------------------------------------------------------
# -------------------------------------Creation Complete----------------------------------------
# -----------------------------------------------------------------------------------------------
