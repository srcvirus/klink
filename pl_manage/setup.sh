#!/bin/bash
./find_non_ready_nodes.sh
cat ready_nodes not_ready > ready_nodes
./all_node_idgen.sh
./all_node_idcopy.sh

for machine in `cat pssh_nodes`
do
	scp -i ~/.ssh/id_rsa -o StrictHostKeyChecking=no ../ihostlist ../imonitorlist ../agent ../client ../config uwaterloo_pweb@$machine:~/
done 
