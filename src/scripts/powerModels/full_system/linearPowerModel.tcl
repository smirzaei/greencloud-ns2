set pModel [new LinearPModel]

set unique_name "GreenCloud 1.0.6 power model"
$pModel set-name $unique_name

$pModel set-coefficient "Computing" 100.33
$pModel set-coefficient "Memory" 0
$pModel set-coefficient "Storage" 0
$pModel set-coefficient "Networking" 0
$pModel set-coefficient "Intercept" 200.67

if { $printPModel == 1 } {
puts -nonewline "Loaded: Power Model: "
puts $unique_name
}