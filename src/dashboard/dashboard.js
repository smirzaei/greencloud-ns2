var DATA_DISPLAY_TYPE = {
        NAV: 0,
        PIE: 1,
        BAR: 2,
        LINE: 3,
}
var DEFAULT_WIDTH = 400;
var DEFAULT_HEIGHT = 300;
var GRAPH_OUTER_DIV_PADDING = 2; // pseudo-padding: inner div is created a bit smaller to avoid stacking/wrapping mess
var LOGGING_ENABLED = false;
var SHOW_CLOSE_BUTTON = false; // allows removing graphs from the UI at runtime
var TITLE_HEIGHT = 25;

var groupedData = {};
var currentSectionNumber = 0;
var currentSubSectionNumber = 0;

var init = function() {
    // loop through each result data set (one for each load-factor test) and create a tabbed <div>
    var tabContainer = $('#tabs');
    tabContainer.hide(); // for performance, don't show until done
        
    for (var loadFactor in rawData) {
        var loadFactorContainer = createTabbedDiv(tabContainer, loadFactor);
        var graphContainer = loadFactorContainer.find('.mainContent'); // the right (main) content column
    
        startSection(graphContainer, "Summary for " + loadFactor);
        
        // Simulation summary
        var navContent = $('<div/>')
            .attr('class', 'nav')
            .appendTo(graphContainer);
        var navData = $('<table/>')
            .attr('class', 'navData')
            .appendTo(navContent);
        
        var navContainer = loadFactorContainer.find('.navData'); // the left-column summary
        
        // navDataDivider(navContainer); // separate the table from the nav title
        showNavData(navContainer, loadFactor, 'simulation');
        showNavData(navContainer, loadFactor, 'parameters');
        showNavData(navContainer, loadFactor, 'loadSummary');
        showNavData(navContainer, loadFactor, 'taskSummary');
        showNavDataSummation(navContainer, loadFactor, 'energySummary');
        showNavData(navContainer, loadFactor, 'energySummary');
        // showNavData(navContainer, loadFactor, 'metricsSummary');
    
        // Main Pie Chart
        drawChart(graphContainer, loadFactor, "energySummary", DATA_DISPLAY_TYPE.PIE);
        
        // Data Center
        startSection(graphContainer, "Data Center");
        startSubSection(graphContainer, "Overall statistics");
        drawChart(graphContainer, loadFactor, "dcLoad", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "dcLoadMem", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "dcLoadStor", DATA_DISPLAY_TYPE.LINE);
        startSubSection(graphContainer, "Physical Servers");
        drawChart(graphContainer, loadFactor, "dcServLoad", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "dcServLoadMem", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "dcServLoadStor", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "dcServTasks", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "dcServTasksFailed", DATA_DISPLAY_TYPE.BAR);
        startSubSection(graphContainer, "Virtual Machines");
        drawChart(graphContainer, loadFactor, "dcVmLoad", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "dcVmLoadMem", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "dcVmLoadStor", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "dcVmTasks", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "dcVmTasksFailed", DATA_DISPLAY_TYPE.BAR);
    
        // Network
        startSection(graphContainer, "Data Center Network");
        
        startSubSection(graphContainer, "Uplink");
        drawChart(graphContainer, loadFactor, "link_HC3_load", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "link_C3C2_load", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "link_C2C1_load", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "queue_HC3_pkts_avg", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "link_HC3_load_time", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "queue_HC3_pkts_time", DATA_DISPLAY_TYPE.LINE);
    
        startSubSection(graphContainer, "Downlink");
        drawChart(graphContainer, loadFactor, "link_C1C2_load", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "link_C2C3_load", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "link_C3H_load", DATA_DISPLAY_TYPE.BAR);
    
        startSection(graphContainer, "Energy Consumption");
        drawChart(graphContainer, loadFactor, "dcPower", DATA_DISPLAY_TYPE.LINE);
        drawChart(graphContainer, loadFactor, "eServers", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "eCoreSwitches", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "eAggrSwitches", DATA_DISPLAY_TYPE.BAR);
        drawChart(graphContainer, loadFactor, "eAccessSwitches", DATA_DISPLAY_TYPE.BAR);
        
		// Charts for Performance and Energy Efficiency Metrics 
        // startSection(graphContainer, "Performance and Energy Efficiency Metrics");
        // drawChart(graphContainer, loadFactor, "npue", DATA_DISPLAY_TYPE.LINE);
        // drawChart(graphContainer, loadFactor, "alurCore", DATA_DISPLAY_TYPE.BAR);
        // drawChart(graphContainer, loadFactor, "alurCore", DATA_DISPLAY_TYPE.LINE);
        // drawChart(graphContainer, loadFactor, "alurAgg", DATA_DISPLAY_TYPE.LINE);
        // drawChart(graphContainer, loadFactor, "alurAcc", DATA_DISPLAY_TYPE.LINE);
        // drawChart(graphContainer, loadFactor, "iscl", DATA_DISPLAY_TYPE.BAR);
    }

    tabContainer.show(); // for performance, don't show until done
    tabContainer.tabs();

		window.addEventListener('click',function(e)
		{
				e = e || window.event;
				var target = e.target || e.srcElement;

				show_description(target);
		}, false);
};

var createTabbedDiv = function(parentContainer, label) {
    var divId = 'tabContent-' + label;

    // ex: <li><a href="#tabContent-load-0.0">Hard Coded</a></li>
    var tabMenu = $('<li/>')
        .appendTo(parentContainer.children('ul'));
    var tabLink = $('<a/>')
        .attr('href', '#' + divId)
        .html(label)
        .appendTo(tabMenu);
    
    // ex: <div id='tabContent-load-0.0' class="workspace"></div>
    var tabDiv = $('<div/>')
        .attr('id', divId)
        .attr('class', 'workspace')
        .appendTo(parentContainer);
    
    // ex: <div class="mainContent"></div><!-- cell -->
    var mainContent = $('<div/>')
        .attr('class', 'mainContent')
        .appendTo(tabDiv);
    
    return tabDiv;
}

var showNavDataSummation = function(container, loadFactor, dataSourceName) {
    var name = getDataPointLabel(dataSourceName, 'TOTAL');
    var value = getDataSourceSum(loadFactor, dataSourceName);
    var units = getDataUnits(dataSourceName);
    addNameValuePairToNav(name, value.toString() + ' ' + units, container);
}

var showNavData = function(container, loadFactor, dataSourceName) {
    var data = rawData[loadFactor][dataSourceName];
    var units = getDataUnits(dataSourceName);
    var descriptions = getDataDescriptions(dataSourceName);
    for (var i in data) {

		var name = getDataPointLabel(dataSourceName, data[i].x);

		if (descriptions != "") 
			if (descriptions[data[i].x] != '') {
				var html = '<b>Description:</b><br />' + descriptions[data[i].x];
				html = '<div class="hidenable popbox" style="display: none;">' + html + '</div>';
				name += ' <img class="do_show_summary" src="images/INFO_IMG.gif" alt="information icon imgage" />';
				name += html;
			}

        var value = data[i].y.toString() + ' ' + units;
        addNameValuePairToNav(name, value, container);
    }
    navDataDivider(container);
};

var navDataDivider = function(container){
    $('<tr/>')
    .html('<td colspan=2><hr class="navDataDivider"/></td>')
    .appendTo(container);
}

var addNameValuePairToNav = function(name, value, container) {
    var $dataRow = $('<tr/>')
        .appendTo(container);
    $('<td/>')
        .attr('class', 'dataLabel')
        .html(name + ':')
        .appendTo($dataRow);
    $('<td class="dataValue"/>')
        .html(value)
        .appendTo($dataRow);
}

var startSection = function(container, sectionName) {
    currentSectionNumber++;
    currentSubSectionNumber = 0;
    var $sectionHeading = $('<h2/>')
        .attr('class','sectionHeading')
        .html(sectionName)
        .appendTo(container);

}

var startSubSection = function(container, sectionName) {
    currentSubSectionNumber++;
    var $sectionHeading = $('<h3/>')
        .attr('class','subSectionHeading')
        .html(sectionName)
        .appendTo(container);

}

var drawChart = function(parentContainer, loadFactor, dataSourceName, dataDisplayType) {
    //add to groupedData
    var data = rawData[loadFactor][dataSourceName];
    data.loadFactor = loadFactor;
    data.dataSourceName = dataSourceName;

    // create the load factor if it doesn't exist yet
    if (!groupedData[loadFactor]) {
        groupedData[loadFactor] = {};
    }

    // the dataGroupKey is how we put multiple series on a single graph
    var dataGroupKey = getDataGroupKey(dataSourceName);
    
    try {
        groupedData[loadFactor][dataGroupKey].push(data);
    }
    catch (e) {
        groupedData[loadFactor][dataGroupKey] = [data];
    }
    
    // create the graph container div if it doesn't exist
    var container = createGraphContainer(parentContainer, dataGroupKey);

    // create graph in container by calling the appropriate graphing function
    // this will also transparently re-draw an existing graph in the case that
    // multiple data series are grouped into a single container
    // NOTE: for nav bar, it just appends a row to the nav table (container is the nav data table)
    var graphingFunctions = {};
    graphingFunctions[DATA_DISPLAY_TYPE.PIE] = showPieChart;
    graphingFunctions[DATA_DISPLAY_TYPE.BAR] = showBarChart;
    graphingFunctions[DATA_DISPLAY_TYPE.LINE] = showLineChart;
    f = graphingFunctions[dataDisplayType];
    f(container, loadFactor, dataGroupKey);
}

var createGraphContainer = function(parentContainer, dataGroupKey) {
    var innerContainer = parentContainer.find("#" + getContainerId(dataGroupKey));
    if (innerContainer.length == 0) { // the container hasn't been created yet
        LOGGING_ENABLED && console.log("Creating inner container ID: ", getContainerId(dataGroupKey));
        var outerContainer = $('<div/>')
            .attr('id', getOuterDivId(dataGroupKey))
            .attr('class','graphOuterDiv')
            .width(getOuterDivWidth(dataGroupKey) + 'px')
            .height(getOuterDivHeight(dataGroupKey) + 'px')
            .appendTo(parentContainer);
    
        var header = $('<h2/>')
            .attr('id', getHeaderId(dataGroupKey))
            .attr('class','graphContainerHeader')// ui-corner-all')
            .html(getDataGroupLabel(dataGroupKey))
            .appendTo(outerContainer);
    
        if (SHOW_CLOSE_BUTTON) {
            var closeResizableDiv = function() {
                outerContainer.hide();
            }
            var $closeButton = $('<span/>')
                .attr('class', 'closeButton ui-icon ui-icon-closethick')
                .html('close')
                .bind('click', closeResizableDiv)
                .appendTo(header);
        }

				if ((des = exist_description(dataGroupKey)) != null) {
					header.html(getDataGroupLabel(dataGroupKey) + ' <img class="do_show" src="images/INFO_IMG.gif" alt="information icon imgage" />');
					var $DescriptionDiv = $('<div/>')
					.attr('id', getHeaderId(dataGroupKey) + "H")
          .attr('class', 'hidenable popbox')
					.attr('style', 'display: none;')
          .html('<b>Description:</b><br />' + des)
          .appendTo(header);			
				}
				
        innerContainer = $('<div/>')
            .attr('id', getContainerId(dataGroupKey) + "H")
            .attr('class','graphContainer')
            .width(getContainerWidth(dataGroupKey) + 'px')
            .height(getContainerHeight(dataGroupKey) + 'px')
            .appendTo(outerContainer);
    }
    else {
        LOGGING_ENABLED && console.log("Using pre-existing inner container ID: ", innerContainer.attr('id'));
    }

    return innerContainer;
}

var exist_description = function(dataGroupKey) {
if (dataGroups[dataGroupKey]['description'] != undefined)
	return dataGroups[dataGroupKey]['description'];
else
	return null;
}

var show_description = function(target) {

	if ((target.querySelector("#" + target.id + "H") != null && target.querySelector("#" + target.id + "H.hidenable").style.display == "none") || target.className == "do_show"  || target.className == "do_show_summary" ){
		if ( target.className == "do_show_summary" ) {
			target = target.parentElement;
			target.querySelector(".popbox").style.display = "block";
		} else {
			if ( target.className == "do_show" ) {
				target = target.parentElement;
			}
			target.querySelector("#" + target.id + "H").style.display = "block";
		}
	} else {
		$(".hidenable").each(function( index ) {
				$( this )[0].style.display = "none";
		})
	}
}

var showPieChart = function(container, loadFactor, dataGroupKey) {
    LOGGING_ENABLED && console.log("showing pie chart for: ", loadFactor, dataGroupKey);

    // pie charts don't use the container mechanism - just show index 0.
    var series = groupedData[loadFactor][dataGroupKey][0];
    var dataSourceName = series.dataSourceName;
    var units = getDataUnits(dataSourceName);

    var formattedData = [];
    var label, value;
    for (var i in series) {
        label = getDataPointLabel(dataSourceName, series[i].x);
        value = series[i].y;
        if (value == null) {
            break;
        }
        formattedData.push({
            'label' : label,
            'data' : parseFloat(value)
        });
    }

    var total = getDataSourceSum(loadFactor, dataSourceName);
    
    // Add the total just below the header
    var header = $('#' + getHeaderId(dataGroupKey));
    $('<div/>')
        .attr('class', 'pieChartTotal')
        .html('Total: ' + total + units)
        .insertAfter(header);
    
    $.plot(
        container,
        formattedData,
        {
            series: {
                pie: {
                    show: true,
                    label: {
                        show: true,
                        radius: 0.85,
                        formatter: 
                            (
                                function(units) {
                                    return function(label, data) {
                                        return '<div style="padding:5px;">' // padding needs to be here because flot does something with it...
                                            + label
                                            + '<br/>'
                                            + data.datapoints.points[1] 
                                            + units
                                            + ' ('
                                            + Math.round(data.percent)
                                            + '%)</div>';
                                    }
                                }
                            )(units),
                        background: { opacity: 0.4 }
                    }    
                }
            },
            legend: {show: false},
        }
    );
};

var showBarChart = function(container, loadFactor, dataGroupKey) {
    LOGGING_ENABLED && console.log("showing bar graph for: ", dataGroupKey);

    var data = [];
    for (var i in groupedData[loadFactor][dataGroupKey]) {
        var series = groupedData[loadFactor][dataGroupKey][i];
        var formattedData = formatXyData(series);
        data.push({
            data: formattedData,
            label: getDataSeriesLabel(series.dataSourceName),
        });
    }

    var options = {
        series : {
            bars : {
                show : true
            }
        },
        xaxis : getAxisConfig(dataGroupKey, 'x'),
        yaxis : getAxisConfig(dataGroupKey, 'y'),
    }

    $.plot(container, data, options);
};

var showLineChart = function(container, loadFactor, dataGroupKey) {
    LOGGING_ENABLED && console.log("showing line graph for: ", dataGroupKey);

    var data = [];
    for (var i in groupedData[loadFactor][dataGroupKey]) {
        var series = groupedData[loadFactor][dataGroupKey][i];
        var formattedData = formatXyData(series);
        data.push({
            data: formattedData,
            label: getDataSeriesLabel(series.dataSourceName),
        });
    }
    
    var options = {
        series : {
            lines : {
                show : true,
                fill : true
            }
        },
        xaxis : getAxisConfig(dataGroupKey, 'x'),
        yaxis : getAxisConfig(dataGroupKey, 'y'),
    };
    
    $.plot(container, data, options);
};

var getOuterDivId = function(dataGroupKey) {
    return 'graph_outer_' + currentSectionNumber + '_' + currentSubSectionNumber + '_' + dataGroupKey;
}

var getHeaderId = function(dataGroupKey) {
    return 'graph_header_' + currentSectionNumber + '_' + currentSubSectionNumber + '_' + dataGroupKey;
}

var getContainerId = function(dataGroupKey) {
    return 'data_container_' + currentSectionNumber + '_' + currentSubSectionNumber + '_' + dataGroupKey;
}

var getDataGroupKey = function(dataSourceName) {
    try {
        if (dataSources[dataSourceName].dataGroup != null) {
            LOGGING_ENABLED && console.log('Using data group ' 
                    + dataSources[dataSourceName].dataGroup
                    + ' for data source '
                    + dataSourceName);
            return dataSources[dataSourceName].dataGroup;
        }
    }
    catch (e) {} // just means that the data isn't configured to be grouped with anything
    return dataSourceName;
}

var getOuterDivWidth = function(dataGroupKey) {
    try {
        if (dataGroups[dataGroupKey].width != null) {
            return dataGroups[dataGroupKey].width;
        }
    }
    catch(e) {}
    return DEFAULT_WIDTH;
}

var getOuterDivHeight = function(dataGroupKey) {
    var height = DEFAULT_HEIGHT;
    try {
        if (dataGroups[dataGroupKey].height != null) {
            height = dataGroups[dataGroupKey].height;
        }
    }
    catch(e) {}
    return height; // optional: + TITLE_HEIGHT
}

var getContainerWidth = function(dataGroupKey) {
    return getOuterDivWidth(dataGroupKey) - 2*GRAPH_OUTER_DIV_PADDING;
}

var getContainerHeight = function(dataGroupKey) {
    return getOuterDivHeight(dataGroupKey) - TITLE_HEIGHT - 2*GRAPH_OUTER_DIV_PADDING;
}

// in: data = [{x:1, y:2},{x:3, y:4}]
// out: formattedData = [[1,2],[3,4]]
var formatXyData = function(data) {
    var formattedData = [];
    for (var xy in data) {
        formattedData.push([
            data[xy].x,  
            data[xy].y,
        ]);
    }
    return formattedData;
}

var getDataSourceSum = function(loadFactor, dataSourceName) {
    var total = 0.0;
    var value;
    for (i in rawData[loadFactor][dataSourceName]) {
        value = parseFloat(rawData[loadFactor][dataSourceName][i].y);
        if (!isNaN(value)) {
            total += value;
        }
    }
    return Math.round(total * 100)/100;
}

var getDataGroupLabel = function(dataGroupKey) {
    try {
        if (dataGroups[dataGroupKey].title != null) {
            return dataGroups[dataGroupKey].title;
        }
    }
    catch(e){}
    return dataGroupKey;
}

var getDataSeriesLabel = function(dataSourceName) {
    try {
        if (dataSources[dataSourceName].title != null) {
            return dataSources[dataSourceName].title;
        }
    }
    catch(e){}
    return null;
}

var getDataPointLabel = function(dataSourceName, dataPointName) {
    try {
        if (dataSources[dataSourceName].labels[dataPointName] != null) {
            return dataSources[dataSourceName].labels[dataPointName];
        }
    }
    catch(e){}
    return dataPointName;
}

var getDataUnits = function(dataSourceName) {
    try {
        if (dataSources[dataSourceName].units != null) {
            return dataSources[dataSourceName].units;
        }
    }
    catch(e){}
    return '';
}

var getDataDescriptions = function(dataSourceName) {
    try {
        if (dataSources[dataSourceName].descriptions != null) {
            return dataSources[dataSourceName].descriptions;
        }
    }
    catch(e){}
    return '';
}

var getAxisConfig = function(dataGroupKey, xOrY) {
    try {
        var label = dataGroups[dataGroupKey][xOrY + 'axis'];
        return {
            axisLabel : label,
            axisLabelUseCanvas : true,
            axisLabelFontSizePixels : 12,
            axisLabelFontFamily : 'sans-serif',
            tickDecimals: 0,
        }
    }
    catch (e) {
        return {};
    }
}
