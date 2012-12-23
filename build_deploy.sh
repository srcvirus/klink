#/bin/bash
if [ -f agent ]
then
	rm agent
fi

if [ -f client ]
then
	rm client
fi

g++ -g -static server_system_test.cc ./webinterface/mongoose.c ./plnode/protocol/plexus/goley/golay.c -lpthread -ldl -o agent &> server_output
g++ -g -static client_system_test.cc ./plnode/protocol/plexus/goley/golay.c -lpthread -o client &> client_output
grep error server_output
grep error client_output
rm server_output client_output

scp config agent ihostlist pweb@cn101.cs.uwaterloo.ca:~/klinkfb
ssh pweb@cn101.cs.uwaterloo.ca "cd klinkfb; ./copy_script.sh"

