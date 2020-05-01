set pModel [new PerComponentModel]

set unique_name "Per component power model"
$pModel set-name $unique_name

if { $printPModel == 1 } {
puts -nonewline "Loaded: Power Model: "
puts $unique_name
}