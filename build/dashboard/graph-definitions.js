// Every trace file name determines that data's "Data Source Name" (as seen in data.js)
// By default, the Data Source Name is also the name of the "Data Group" (i.e. graph container)
// within which that data is displayed.
// By setting dataSources[dataSourceName].dataGroup, you can group data sources together into a single group.

// Size and Title are specified at the Data Group level, whereas series title is set at the Data Source level. 

var dataSources = {
    'energySummary': {
        'labels': {
            'energy.servers': 'Server energy',
            'energy.switches.core': 'Switch energy (core)',
            'energy.switches.aggregation': 'Switch energy (agg.)',
            'energy.switches.access': 'Switch energy (access)',
            'TOTAL': 'Total energy',
        },
        'units': 'W*h',
    },
    'loadSummary': {
        'labels': {
            'load.average': 'Average Load/Server',
            'load.datacenter': 'Datacenter Load',
        },
    },
    'taskSummary': {
        'labels': {
            'tasks.average': 'Average tasks/server',
            'tasks.total': 'Total tasks',
            'tasks.failed.dc': 'Tasks rejected by DC',
            'tasks.failed.servers': 'Tasks failed by servers',
        },
    },
    'simulation': {
        'labels': {
            'time.simulation.end': 'Simulation time',
            'time.simulation.duration': 'Simulation duration (sec.)',
        }
    },
    'parameters': {
        'labels': {
            'count.switches.core': 'Switches (core)',
            'count.switches.aggregation': 'Switches (agg.)',
            'count.switches.access': 'Switches (access)',
            'count.servers': 'Servers',
            'count.users': 'Users',
            'architecture.datacenter': 'Datacenter architecture',
            'powermanagement.servers': 'Power mode (servers)',
            'powermanagement.switches': 'Power mode (switches)',
        }
    },
    'serv_load_time': {
        'title': 'Server',
        'dataGroup': 'load_times',
    },
    'metricsSummary': {
        'labels': {
            'cnee': 'Communication Network Energy Efficiency (CNEE)',
						'npueFinal': 'Network Power Usage Effectiveness (NPUE)'
        },
        'descriptions': {
            'cnee': 'Energy to deliver a single bit of information',
						'npueFinal': 'Ratio between total IT power and power consumed by network equipment'
        },
    },
};

// dataGroups are different dataSourceName's that share a graph.
// for instance, multiple data series on a single bar chart.
var dataGroups = {
    'dcLoad': {
        'title': 'Datacenter Computational Load',
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Data center load',
    },
    'dcLoadMem': {
        'title': 'Datacenter Memory Load',
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Data center load',
    },
    'dcLoadStor': {
        'title': 'Datacenter Storage Load',
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Data center load',
    },
    'dcServLoad': {
        'title': 'Servers Comput. Load',
        		'description': 'Average CPU load per computing server.',
        'xaxis': 'Server number',
        'yaxis': 'Load factor',
    },
    'dcServLoadMem': {
        'title': 'Servers Memory Load',
        		'description': 'Average memory load per computing server.',
        'xaxis': 'Server number',
        'yaxis': 'Load factor',
    },
    'dcServLoadStor': {
        'title': 'Servers Storage Load',
        		'description': 'Average storage load per computing server.',
        'xaxis': 'Server number',
        'yaxis': 'Load factor',
    },
    'dcVmLoad': {
        'title': 'VM Comput. Load',
        'xaxis': 'VM number',
        'yaxis': 'Load factor',
    },
    'dcVmLoadMem': {
        'title': 'VM Memory Load',
        'xaxis': 'VM number',
        'yaxis': 'Load factor',
    },
    'dcVmLoadStor': {
        'title': 'VM Storage Load',
        'xaxis': 'VM number',
        'yaxis': 'Load factor',
    },
    'dcServTasks': {
        'title': 'Server Tasks',
        		'description': 'Average number of submitted tasks per server.',
        'xaxis': 'Server number',
        'yaxis': 'Tasks',
    },
    'dcVmTasks': {
        'title': 'VM Tasks',
        'xaxis': 'VM number',
        'yaxis': 'Tasks',
    },
    'dcServTasksFailed': {
        'title': 'Failed Tasks by Server',
        		'description': 'Average number of failed tasks per server. A task is considered failed when it is not able to finish within a specified deadline.',
        'xaxis': 'Server number',
        'yaxis': 'Failed tasks',
    },
    'dcVmTasksFailed': {
        'title': 'Failed Tasks by VM',
        		'description': 'Average number of failed tasks per VM. A task is considered failed when it is not able to finish within a specified deadline.',
        'xaxis': 'VM number',
        'yaxis': 'Failed tasks',
    },
    'energySummary': {
        'title': 'Energy Summary',
        'width': 800,
        'height': 600,
    },
    'eAccessSwitches': {
        'title': 'Energy Access Switches',
        		'description': 'Total energy consumed by access switches during runtime.',
        'xaxis':'Access switch number',
        'yaxis':'Energy consumed (W*h)',
    },
    'eAggrSwitches': {
        'title': 'Energy Agg. Switches',
        		'description': 'Total energy consumed by aggregation switches during runtime.',
        'xaxis':'Agg. switch number',
        'yaxis':'Energy consumed (W*h)',
    },
    'eCoreSwitches': {
        'title': 'Energy Core Switches',
        		'description': 'Total energy by core switches during runtime.',
        'xaxis':'Core switch number',
        'yaxis':'Energy consumed (W*h)',
    },
    'eServers': {
        'title': 'Energy Servers',
        		'description': 'Total energy consumed by each computing server during runtime.',
        'xaxis':'Server number',
        'yaxis':'Energy consumed (W*h)',
    },
    'dcPower': {
        'title': 'Power Consumed by Servers',
        		'description': 'Power consumed by computing servers during runtime. The reported power values are collected during the change of a power state in servers and in practice may differ from the real values.',
        'xaxis':'Simulation time (s)',
        'yaxis':'Power',
    },
    'link_load': {
        'title': 'Link Load'
    },
    'link_C1C2_load': {
        'title': 'Links Core-to-Agg.',
        		'description': 'Average load on network links connecting core switches to aggregation switches in the downlink.',
        'xaxis': 'Link number',
        'yaxis': 'Link load (%)',
    },
    'link_C2C1_load': {
        'title': 'Links Agg.-to-Core',
        		'description': 'Average load on network links connecting aggregation switches to core switches in the uplink.',
        'xaxis': 'Link number',
        'yaxis': 'Link load (%)',
    },
    'link_C2C3_load': {
        'title': 'Links Agg.-to-Racks',
        		'description': 'Average load on network links connecting aggregation switches to Top-of-Rack (ToR) switches in the downlink.',
        'xaxis': 'Link number',
        'yaxis': 'Link load (%)',
    },
    'link_C3C2_load': {
        'title': 'Links Racks-to-Agg',
        		'description': 'Average load on network links connecting Top-of-Rack (ToR) switches to aggregation switches in the uplink.',
        'xaxis': 'Link number',
        'yaxis': 'Link load (%)',
    },
    'link_C3H_load': {
        'title': 'Links Racks-to-Hosts',
        		'description': 'Average load on network links connecting computing servers to thier Top-of-Rack (ToR) switches in the downlink.',
        'xaxis': 'Link number',
        'yaxis': 'Link load (%)',
    },
    'link_HC3_load': {
        'title': 'Links Hosts-to-Racks',
        		'description': 'Average load on network links connecting computing servers to thier Top-of-Rack (ToR) switches in the uplink.',
        'xaxis': 'Link number',
        'yaxis': 'Link load (%)',
    },
    'load_times': {
        'title': 'Load Times'
    },
    'queue_HC3_pkts_avg': {
        'title': 'Queue Host-to-Rack',
        		'description': 'Average queue size in packets on network links connecting computing servers to their Top-of-Rack (ToR) switches in the uplink.',
        'xaxis': 'Link number',
        'yaxis': 'Queue size (packets)',
    },
    'link_HC3_load_time': {
        'title': 'Link Host-to-Rack (Single)',
        		'description': 'Load dynamics of a single link connecting a given server to its Top-of-Rack (ToR) switch in the uplink. The link number is specified by mon(link_hosts_C3) variable in setup_params.tcl.',        
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Link load (%)',
    },
    'queue_C3C2-0_pkts': {
        'title': 'C3/C2 - 0',
    },
    'queue_C3C2-1_pkts': {
        'title': 'C3/C2 - 1',
    },
    'queue_HC3_pkts_time': {
        'title': 'Queue Host-to-Rack (Single)',
        		'description': 'Queue load dynamics in packets of a single link connecting a given server to its Top-of-Rack (ToR) switch in the uplink. The queue number is specified by mon(queue-HC3) variable in setup_params.tcl.',
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Queue size (packets)',
    },    
    'npue': {
        'title': 'Network Power Usage Effectiveness (NPUE)',
				'description': 'Ratio between total IT power and power consumed by network equipment',
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Ratio (W/W)',
    },
		'iscl': {
        'title': 'Inter-Server Communication Latency (ISCL)',
				'description': 'Communication latency between computing servers',
        'xaxis': 'Server number',
        'yaxis': 'rtt (ms)',
    },
		'alurCore': {
        'title': 'Average Link Utilization Ratio (ALUR) Core to Aggregation',
				'description': 'Utilization ratio of the links which connect Core to Aggregation switches',
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Alur',
    },
		'alurAgg': {
        'title': 'Average Link Utilization Ratio (ALUR) Aggregation to Access',
				'description': 'Utilization ratio of the links which connect Aggregation to Access switches',
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Alur',
    },
		'alurAcc': {
        'title': 'Average Link Utilization Ratio (ALUR) Access to Host',
				'description': 'Utilization ratio of the links which connect Access switches to the servers',
        'xaxis': 'Simulation time (s)',
        'yaxis': 'Alur',
    },
}
