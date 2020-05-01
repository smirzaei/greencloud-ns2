set resSpec [new ResourceSpec]

$resSpec set-name "Nominal 4GB memory"
$resSpec set-type "Memory"
$resSpec set-arch 1
$resSpec add-capacity 4000000000
$resSpec add-power-state 1


source "powerModels/component/mem_pmodel.tcl"
$resSpec set-power-model $pModel