set resSpec [new ResourceSpec]

$resSpec set-name "Nominal 10GB memory"
$resSpec set-type "Memory"
$resSpec set-arch 1
$resSpec add-capacity 10000000000
$resSpec add-power-state 1