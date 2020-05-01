set resSpec [new ResourceSpec]

$resSpec set-name "Nominal processor low power"
$resSpec set-type "Computing"
$resSpec set-arch 1
$resSpec add-capacity 1000100
$resSpec add-power-state 1


source "powerModels/component/95W05idle.tcl"
$resSpec set-power-model $pModel