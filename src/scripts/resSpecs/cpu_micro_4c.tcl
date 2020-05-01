set resSpec [new ResourceSpec]

$resSpec set-name "Micro processor 4 cores"
$resSpec set-type "Computing"
$resSpec set-arch 1
$resSpec add-capacity 150015
$resSpec add-capacity 150015
$resSpec add-capacity 150015
$resSpec add-capacity 150015
$resSpec add-power-state 1


source "powerModels/component/6W05idle.tcl"
$resSpec set-power-model $pModel