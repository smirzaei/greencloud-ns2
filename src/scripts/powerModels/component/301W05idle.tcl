set pModel [new LinearPModel]

set unique_name "Linear model: 301W max, idle 50% of max"
$pModel set-name $unique_name

$pModel set-coef-number 2
$pModel set-coefficient-numeric "0" 150.5
$pModel set-coefficient "Intercept" 150.5

if { $printPModel == 1 } {
puts -nonewline "Loaded: Power Model: "
puts $unique_name
}