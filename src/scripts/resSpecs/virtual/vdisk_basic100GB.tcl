set resSpec [new ResourceSpec]

$resSpec set-name "Nominal 100GB virtual disk"
$resSpec set-type "Storage"
$resSpec set-arch 1.0
$resSpec add-capacity 100000000000
$resSpec add-power-state 1