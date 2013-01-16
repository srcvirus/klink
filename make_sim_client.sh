#!/bin/bash
if [ -f agent ]
then
	rm sim
fi
if [ -f client ]
then
	rm client
fi
g++ -g -W simulator.cc ./webinterface/mongoose.c ./plnode/protocol/plexus/golay/golay.c -lpthread -ldl -o simulator &> output_agent
grep error output_agent
g++ -g -W client_system_test.cc ./plnode/protocol/plexus/golay/golay.c -lpthread -o client &> output_client
grep error output_client
rm output_*
