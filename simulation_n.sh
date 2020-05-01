#!/bin/bash
ITERATIONS=30
TRACES_DIR=""
WALLTIME="5:00:00"
BESTEFFORT=false
SCHEDULER="Green"
TOPOLOGY="three-tier debug"
DEADLINE=1.2

# for (( num=1 ; num <=$ITERATIONS ; num++ ))


usage()
{
    echo "Batch running script"
    echo ""
    echo "./simulation_n.sh"
    echo "    -h --help"
    echo "    -i --iterations=$ITERATIONS"
    echo "    -t --trace-dir=$TRACES_DIR"
    echo "    -w --walltime=$WALLTIME"
    echo "    -s --scheduler=${SCHEDULER}"
    echo "    -top --topology=${TOPOLOGY}"
    echo "    -d --deadline=$DEADLINE"
    echo "    --best=$BESTEFFORT"
    echo ""
}
 
while [ "$1" != "" ]; do
    PARAM=`echo $1 | awk -F= '{print $1}'`
    VALUE=`echo $1 | awk -F= '{print $2}'`
    case $PARAM in
        -h | --help)
            usage
            exit
            ;;
        -i | --iterations)
            ITERATIONS=$VALUE
            ;;
        -t | --trace-dir)
           TRACES_DIR=$VALUE
            ;;
	-w | --walltime)
	   WALLTIME=$VALUE
	    ;;
	--best)
	  BESTEFFORT=true
	    ;;
	-s |  --scheduler)
		SCHEDULER="${VALUE}"
	    ;;
    -top | --topology)
		TOPOLOGY="${VALUE}"
        ;;
    -d |  --deadline)
	  	DEADLINE=$VALUE
        ;;
        *)
            echo "ERROR: unknown parameter \"$PARAM\""
            usage
            exit 1
            ;;
    esac
    shift
done	
if [ -z "$TRACES_DIR" ];
then 
    echo "No TRACES_DIR set. Use -t or --trace-dir!";
    echo "Aborting execution.";
    exit 0;
fi
for i in $(seq $ITERATIONS)
do
    mkdir -p "experiments/$TRACES_DIR/$i"
    if [[ "$BESTEFFORT" == false ]];
    then
	  #echo "Normal job submitted."
	 oarsub -l core=1,walltime=$WALLTIME -O "experiments/$TRACES_DIR/$i/OAR.stdout" -E "experiments/$TRACES_DIR/$i/OAR.stderr" "./run -b -t=experiments/$TRACES_DIR/$i -r=$i -s=\"${SCHEDULER}\" -top=\"${TOPOLOGY}\" -d=$DEADLINE"
    else
      #echo "Besteffort job submitted."
	 oarsub -l core=1,walltime=$WALLTIME -O "experiments/$TRACES_DIR/$i/OAR.stdout" -E "experiments/$TRACES_DIR/$i/OAR.stderr" -t besteffort -t idempotent "./run -b -t=experiments/$TRACES_DIR/$i -c=true -r=$i -s=\"${SCHEDULER}\" -top=\"${TOPOLOGY}\" -d=$DEADLINE"
    fi
  done
