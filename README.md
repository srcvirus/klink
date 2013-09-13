Required libraries
==================
1. gcc 4.4 and upward (tested on Ubuntu 10.04, 10.10, 11.04, 11.10, 12.04, 12.10, 13.04)
2. sqlite3

sudo apt-get install build-essential sqlite3 libsqlite3-dev

Compile & Build
===================
Run the make_agent_client.sh script to build agent and client
./make_agent_client.sh

Run Command
==============
./agent to run the home agent
./client to run the client to bootstrap the home agents 

Configuration file
==================
1. config
	node_file --> Name of the file containing list of nodes running the home agents. 
				  See the next section for the file's format.
	monitors_file --> Name of the file containing the list of logging servers / monitors.
					  See section 3 for file format

	log_server_user --> The common user name of the logging servers. Home agents use ssh to 
						send logs to the logging/monitoring server. The log servers are assumed
						to have a common user name and the public keys of home agents are assumed
						to be copied in the logging servers. 
						
	check_point_row --> number of rows after which to send log entries to logging server
	k --> number of random links
	timeout --> connection timeout
	retry --> number connection retries
	cache_storage --> end_point | path
	cache_type --> cache type
	client_listen_port --> default listening port for the client
	seq_file --> tracks the sequence of runs [used for experimental purpose]
	name_count --> number name to lookup [used for experimental purpose]
	input --> name database for lookup [used for experimental purpose]

2. host list (name contained in config)
	<number of hosts>
	<hostname> <port number> <ip address> <home agent alias>

3. monitor list (name contained in config)
	<number of hosts>
	<hostname>
