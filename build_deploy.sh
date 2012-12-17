#/bin/bash
rm server client server_system_test client_system_test
g++ -static server_system_test.cc -lpthread -o server
g++ -static client_system_test.cc -lpthread -o client
scp client server ihostlist pweb@cn101.cs.uwaterloo.ca:~/klink
ssh pweb@cn101.cs.uwaterloo.ca "cd klink; ./copy_script.sh"
