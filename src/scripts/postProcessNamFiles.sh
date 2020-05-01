#!/bin/bash
# This script should be called after the run script finishes (or incorporated into it) 
# Format: postProcessNamFiles.sh <traceFileDirectory>

### VERY IMPORTANT! ###
# This script makes the assumption that all nodes are configured in one block of content.
# That is, all node-config lines (in the format 'n*') are grouped together, and when any other lines start,
# then no more 'n*' lines will occur for the remainder of the file.

# for efficiency, this script assumes that all the layout configuration happens in the first $NUM_CONFIG_ROWS lines
NUM_CONFIG_ROWS=10000


traceFileDirectory=$1

ls -1 "$traceFileDirectory" | grep '\.nam$' | while read namFileName
do
    namFileFullPath="$traceFileDirectory/$namFileName"
    sortedNamFileFullPath=$namFileFullPath.sorted

	echo "processing $namFileFullPath -> $sortedNamFileFullPath"
	
	# copy the top content (before the node configuration)
	cat $namFileFullPath | head -n$NUM_CONFIG_ROWS | grep '^[VWA]' > $sortedNamFileFullPath
	
	# copy the node configuration - sort it to fix the NAM display bug!
    cat $namFileFullPath | 
        head -n$NUM_CONFIG_ROWS |
        grep '^n' | 
        sed 's/.*-s\s\+\(\w\+\)\s.*/\1<<THE_REAL_STARTING_POINT>>\0/g' |
        sort -n |
        sed 's/.*<<THE_REAL_STARTING_POINT>>\(.*\)/\1/g' >> $sortedNamFileFullPath
    
    # copy this re-ordered stream back into the file (use dd's notrunc so that the remainder of the file is preserved as-is)
    dd if=$sortedNamFileFullPath of=$namFileFullPath conv=notrunc
    
    # remove the temp file
    rm $sortedNamFileFullPath
done
