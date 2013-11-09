#!/bin/bash
#parallel-ssh -h pssh_nodes -x "-l uwaterloo_pweb" -x "-i ~/.ssh/id_rsa" -x "-o StrictHostKeyChecking=no" "sudo /etc/init.d/dnsgw start &"
for node in `cat client_list`
do
	echo $node
	ssh -t -i ~/.ssh/id_rsa uwaterloo_pweb@$node "sudo yum -y install bind-utils"
done
