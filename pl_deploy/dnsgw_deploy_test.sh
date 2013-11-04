#!/bin/bash

for server in `cat pssh_nodes`
do
	echo $server
	nslookup google.com $server
done
