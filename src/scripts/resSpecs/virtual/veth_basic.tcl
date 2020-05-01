set resSpec [new ResourceSpec]

$resSpec set-name "Nominal 1GbE virtual NIC"
$resSpec set-type "Networking"
$resSpec set-arch 1
$resSpec add-capacity 1000000
$resSpec add-power-state 1