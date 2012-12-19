#/bin/bash
if [ -f agent ]
then
	rm agent
fi

if [ -f client ]
then
	rm client
fi

g++ -g -static server_system_test.cc -lpthread -o agent &> server_output
g++ -static client_system_test.cc -lpthread -o client &> client_output
grep error server_output
grep error client_output
rm server_output client_output

scp agent ihostlist pweb@cn101.cs.uwaterloo.ca:~/klink
ssh pweb@cn101.cs.uwaterloo.ca "cd klink; ./copy_script.sh"
