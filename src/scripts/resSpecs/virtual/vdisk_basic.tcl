set resSpec [new ResourceSpec]

$resSpec set-name "Nominal 10GB virtual disk"
$resSpec set-type "Storage"
$resSpec set-arch 1.0
$resSpec add-capacity 10000000000
$resSpec add-power-state 1