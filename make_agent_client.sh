#!/bin/bash
if [ -f agent ]
then
	rm agent
fi
if [ -f client ]
then
	rm client
fi
g++ -static -g -W server_system_test.cc -lpthread -o agent &> output_agent
grep error output_agent
g++ -static -g -W client_system_test.cc -lpthread -o client &> output_client
grep error output_client
rm output_*


