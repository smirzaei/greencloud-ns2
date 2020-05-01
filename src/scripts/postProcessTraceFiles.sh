#!/bin/bash
# This script should be called after the run script finishes (or incorporated into it) 
# Format: postProcessTraceFiles.sh <traceFileDirectory>

traceFileDirectory=$1
JSON_FILE_NAME=$1/data.js

# NOTE: this must match between ./run and ./scripts/postProcessTraceFiles.sh
RESULTS_DIR_PREFIX=simulation

echo 'var rawData = {};' > $JSON_FILE_NAME

ls -1 "$traceFileDirectory" | grep "^$RESULTS_DIR_PREFIX" | while read subDirectory
do
    echo "rawData['$subDirectory'] = {" >> $JSON_FILE_NAME

    subDirectory=$traceFileDirectory/$subDirectory
    ls -1 "$subDirectory" | grep '\.tr$' | while read traceFileName
    do
        traceFileFullPath="$subDirectory/$traceFileName"
	    echo "processing $traceFileFullPath"
	
	    dataSourceName="${traceFileName%.*}"
	    echo "    '$dataSourceName': [" >> $JSON_FILE_NAME
	
	    cat "$traceFileFullPath" | while read fileLine
	    do
	        # split the line into key/value, based on the FIRST space separator
            key=`expr "$fileLine" : '^\([^ ]\+\)'`
            value=`expr "$fileLine" : '^[^ ]\+ \+\(.*\)$'`
            echo "        {'x': '$key', 'y': '$value'}," >> $JSON_FILE_NAME
	    done
	    echo "    ]," >> $JSON_FILE_NAME
    done

    echo '};' >> $JSON_FILE_NAME
done

