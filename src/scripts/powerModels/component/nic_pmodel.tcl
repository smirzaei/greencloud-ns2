set pModel [new LinearPModel]

set unique_name "nic model"
$pModel set-name $unique_name

$pModel set-coef-number 2
$pModel set-coefficient-numeric "0" 0.03
$pModel set-coefficient "Intercept" 0

if { $printPModel == 1 } {
puts -nonewline "Loaded: Power Model: "
puts $unique_name
}