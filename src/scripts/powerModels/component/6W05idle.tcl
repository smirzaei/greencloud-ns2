set pModel [new LinearPModel]

set unique_name "Linear model: 6W max, idle 50% of max"
$pModel set-name $unique_name

$pModel set-coef-number 2
$pModel set-coefficient-numeric "0" 3.0
$pModel set-coefficient "Intercept" 3.0

if { $printPModel == 1 } {
puts -nonewline "Loaded: Power Model: "
puts $unique_name
}