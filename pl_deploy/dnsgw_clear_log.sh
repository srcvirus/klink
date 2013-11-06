#!/bin/bash
#parallel-ssh -h pssh_nodes -x "-l uwaterloo_pweb" -x "-i ~/.ssh/id_rsa" -x "-o StrictHostKeyChecking=no" "sudo /etc/init.d/dnsgw start &"
for node in `cat pssh_nodes`
do
	ssh -t -i ~/.ssh/id_rsa uwaterloo_pweb@$node "sudo echo '' > /var/log/dnsgw.log"
done
