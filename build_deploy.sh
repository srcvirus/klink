#/bin/bash
if [ -f agent ]
then
	rm agent
fi

if [ -f client ]
then
	rm client
fi

<<<<<<< HEAD
g++ -g -static server_system_test.cc ../web_interface/mongoose.c -lpthread -ldl -o agent &> server_output
=======
g++ -g -static server_system_test.cc -lpthread -o pweb_agent &> server_output
>>>>>>> e38d2aee50c88ffdb761835915c1be83608ac1b5
g++ -g -static client_system_test.cc -lpthread -o client &> client_output
grep error server_output
grep error client_output
rm server_output client_output

<<<<<<< HEAD
scp config agent ihostlist pweb@cn101.cs.uwaterloo.ca:~/klinkfb
ssh pweb@cn101.cs.uwaterloo.ca "cd klinkfb; ./copy_script.sh"
=======
scp config pweb_agent ihostlist pweb@cn101.cs.uwaterloo.ca:~/klink
ssh pweb@cn101.cs.uwaterloo.ca "cd klink; ./copy_script_src.sh"
>>>>>>> e38d2aee50c88ffdb761835915c1be83608ac1b5
