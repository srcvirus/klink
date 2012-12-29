#!/bin/bash
for host in `cat pssh_nodes`
do
	./single_node_idgen.sh $host
done
