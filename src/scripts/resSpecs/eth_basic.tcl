set resSpec [new ResourceSpec]

$resSpec set-name "Nominal 1GbE NIC"
$resSpec set-type "Networking"
$resSpec set-arch 1
$resSpec add-capacity 125000000
$resSpec add-power-state 1


source "powerModels/component/nic_pmodel.tcl"
$resSpec set-power-model $pModel