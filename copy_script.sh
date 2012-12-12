#/bin/bash
basename="cn"
for i in {1..10}
do
	cluster=$basename$i.cs.uwaterloo.ca
	scp client server hostlist pweb@$cluster:~/klink
done
