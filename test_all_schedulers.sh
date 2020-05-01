#/bin/nash
for SCHEDULER in Green Random RoundRobin RandDENS BestDENS HEROS; do
  ~/greencloud/simulation_n.sh --best=true -w=5:00:00 -t="${1}${SCHEDULER}" -s="${SCHEDULER}" -top="${2}" -i=50
done
