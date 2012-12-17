#!/bin/bash
if [ -f agent ]
then
	rm agent
fi
if [ -f client ]
then
	rm client
fi
g++ -static -W server_system_test.cc -lpthread -o agent &>> output
g++ -static -W client_system_test.cc -lpthread -o client &>> output
grep error output
rm output


