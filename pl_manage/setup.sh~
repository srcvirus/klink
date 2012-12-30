#!/bin/bash
./find_non_ready_nodes.sh
./all_node_idgen.sh
./all_node_idcopy.sh
cat not_ready >> ready_nodes
if [ -f nodes ]
then
	rm nodes
fi

touch nodes
nodecount=`wc -l pssh_nodes | cut -d' ' f 1`
echo $nodecount >> nodes
for machine in `cat pssh_nodes`
do
	echo "$machine 20000" >> nodes
done
for machine in `cat pssh_nodes`
do
	scp -i ~/.ssh/id_rsa -o StrictHostKeyChecking=no nodes ../imonitorlist ../agent ../client ../config uwaterloo_pweb@$machine:~/
done

