#!/bin/bash
if [ -f agent ]
then
	rm agent
fi
if [ -f client ]
then
	rm client
fi
g++ -g -W server.cc ./webinterface/mongoose.c ./plnode/protocol/plexus/golay/golay.c ./plnode/db/database.cc -lpthread -lsqlite3 -ldl -o agent &> output_agent
grep error output_agent
g++ -g -W client.cc ./plnode/protocol/plexus/golay/golay.c -lpthread -o client &> output_client
grep error output_client
rm output_*
