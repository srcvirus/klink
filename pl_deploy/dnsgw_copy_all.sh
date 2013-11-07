#!/bin/bash
#parallel-ssh -h pssh_nodes -x "-l uwaterloo_pweb" -x "-i ~/.ssh/id_rsa" -x "-o StrictHostKeyChecking=no" "sudo /etc/init.d/dnsgw start &"
for node in `cat pssh_nodes`
do
	scp -i ~/.ssh/id_rsa /etc/dnsgw.conf uwaterloo_pweb@$node:~/
	ssh -t -i ~/.ssh/id_rsa uwaterloo_pweb@$node "sudo cp -f ~/dnsgw /usr/local/bin/dnsgw ; sudo cp -f ~/dnsgw.conf /etc/dnsgw.conf"
done
