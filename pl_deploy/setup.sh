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
nodecount=`wc -l pssh_nodes | cut -d' ' -f 1`
done=0
echo $nodecount >> nodes
for machine in `cat pssh_nodes`
do
	echo "$machine 20000" >> nodes
done

scp ../config ../imonitorlist ../agent nodes pweb@cn102.cs.uwaterloo.ca:~/klink/pl_deploy
ssh pweb@cn102.cs.uwaterloo.ca "cd klink/pl_deploy ; ./upload_wrap.sh &"

#for machine in `cat pssh_nodes`
#do
#	echo $machine
#	scp -i ~/.ssh/id_rsa -o StrictHostKeyChecking=no nodes ../imonitorlist ../agent ../config uwaterloo_pweb@$machine:~/
#	done=$(($done + 1))
#	echo "$done/$nodecount complete"
#done

