set resSpec [new ResourceSpec]

$resSpec set-name "High-end processor 8 cores"
$resSpec set-type "Computing"
$resSpec set-arch 1
$resSpec add-capacity 1500150
$resSpec add-capacity 1500150
$resSpec add-capacity 1500150
$resSpec add-capacity 1500150
$resSpec add-capacity 1500150
$resSpec add-capacity 1500150
$resSpec add-capacity 1500150
$resSpec add-capacity 1500150
$resSpec add-power-state 1


source "powerModels/component/301W033idle.tcl"
#source "powerModels/component/cpu_pmodel.tcl"
$resSpec set-power-model $pModel