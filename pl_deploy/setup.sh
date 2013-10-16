#!/bin/bash

# Find the not prepared nodes in planet lab
./find_non_ready_nodes.sh

# Generate public-private key pair for the planet lab nodes
./all_node_idgen.sh

# Copy the public key of planet lab nodes to the cluster nodes
./all_node_idcopy.sh

# since the "not ready" nodes are ready now, add them to ready nodes list

cat not_ready >> ready_nodes

# removal of some temoporary files
if [ -f nodes_temp ]
then
	rm nodes_temp
fi

if [ -f nodes ]
then
	rm nodes
fi

touch nodes

#count the number of planet lab nodes
nodecount=`wc -l pssh_nodes | cut -d' ' -f 1`
done=0
echo $nodecount >> nodes

for machine in `cat pssh_nodes`
do
	# get the ip address of the machine
	ip=`host $machine | egrep '[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}' | cut -d' ' -f 4`
	# 55321 is the listening port number of planet lab nodes
	echo "$machine 55231 $ip" >> nodes_temp
done

# give a name to the planet lab nodes. e.g., uwaterloo0, uwaterloo1. The names are derived from the first
# part of the FQDN

cat nodes_temp | awk '{len=split($1,array,"."); print $1, $2, $3, array[len-1] map[array[len-1]]++;}' >> nodes

# copy the required files to cn102. cn102 will spawn multiple processes to upload all the files to 
# all the planet lab nodes

scp pssh_nodes ../config ../imonitorlist ../agent nodes ../dnsgw-pl1.1.tar.bz2 pweb@cn102.cs.uwaterloo.ca:~/klink/pl_deploy
sleep 5
ssh pweb@cn102.cs.uwaterloo.ca "cd klink/pl_deploy ; ./upload_wrap.sh &"
scp -i ~/.ssh/id_rsa ../config ../imonitorlist nodes uwaterloo_pweb@plink.cs.uwaterloo.ca:~/

#for machine in `cat pssh_nodes`
#do
#	echo $machine
#	scp -i ~/.ssh/id_rsa -o StrictHostKeyChecking=no nodes ../imonitorlist ../agent ../config uwaterloo_pweb@$machine:~/
#	done=$(($done + 1))
#	echo "$done/$nodecount complete"
#done

