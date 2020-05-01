set resSpec [new ResourceSpec]

$resSpec set-name "Commodity processor 4 cores"
$resSpec set-type "Computing"
$resSpec set-arch 1
$resSpec add-capacity 1000100
$resSpec add-capacity 1000100
$resSpec add-capacity 1000100
$resSpec add-capacity 1000100
$resSpec add-power-state 1


#source "powerModels/component/301W033idle.tcl"
source "powerModels/component/201W05idle.tcl"
$resSpec set-power-model $pModel