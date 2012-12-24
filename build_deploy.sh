#/bin/bash
if [ -f pweb_agent ]
then
	rm pweb_agent
fi

if [ -f client ]
then
	rm client
fi

g++ -static -g -W server_system_test.cc ./webinterface/mongoose.c ./plnode/protocol/plexus/golay/golay.c -lpthread -ldl -o pweb_agent &> server_output
g++ -static -g -W client_system_test.cc ./plnode/protocol/plexus/golay/golay.c -lpthread -o client &> client_output
grep error server_output
grep error client_output
rm server_output client_output

scp config pweb_agent ihostlist imonitorlist pweb@cn101.cs.uwaterloo.ca:~/klink
ssh pweb@cn101.cs.uwaterloo.ca "cd klink; ./copy_script_src.sh"
