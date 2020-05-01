set resSpec [new ResourceSpec]

$resSpec set-name "Nominal 1GB virtual memory"
$resSpec set-type "Memory"
$resSpec set-arch 1
$resSpec add-capacity 1000000000
$resSpec add-power-state 1