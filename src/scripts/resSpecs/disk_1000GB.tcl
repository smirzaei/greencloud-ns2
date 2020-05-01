set resSpec [new ResourceSpec]

$resSpec set-name "Nominal 1000GB disk"
$resSpec set-type "Storage"
$resSpec set-arch 1.0
$resSpec add-capacity 1000000000000
$resSpec add-power-state 1

source "powerModels/component/disk_pmodel.tcl"
$resSpec set-power-model $pModel