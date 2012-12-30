#!/bin/bash
if [ -f not_ready ]
then
	rm not_ready
fi

for node in `cat pssh_nodes`
do
	for ready in `cat ready_nodes`
	do
		found=0
		if [ "$ready" == "$node" ]
		then
			found=1
			break
		fi
	done
	if [ $found -eq 1 ]
	then
		break
	else 
		echo $node >> not_ready
	fi
done
