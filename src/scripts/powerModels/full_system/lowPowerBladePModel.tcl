set pModel [new LinearPModel]

#model source: Rivoire, S.; Shah, M.A.; Ranganathan, P.; Kozyrakis, C.; Meza, J., "Models and Metrics to Enable Energy-Efficiency Optimizations," Computer , vol.40, no.12, pp.39,48, Dec. 2007
set unique_name "Low power blade (Rivoire)"
$pModel set-name $unique_name


$pModel set-coefficient "Computing" 23.61
$pModel set-coefficient "Memory" -0.85
$pModel set-coefficient "Storage" 22.32
$pModel set-coefficient "Networking" 0.03
$pModel set-coefficient "Intercept" 14.45


if { $printPModel == 1 } {
	puts -nonewline "Loaded: Power Model: "
	puts $unique_name
}


