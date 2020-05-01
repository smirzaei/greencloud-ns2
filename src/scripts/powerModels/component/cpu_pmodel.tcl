set pModel [new LinearPModel]

set unique_name "cpu model"
$pModel set-name $unique_name

$pModel set-coef-number 2
$pModel set-coefficient-numeric "0" 23.61
$pModel set-coefficient "Intercept" 14.45

if { $printPModel == 1 } {
puts -nonewline "Loaded: Power Model: "
puts $unique_name
}