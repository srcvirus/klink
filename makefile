CC = g++
LIBRPCFLAGS = -c

all: librpc.a binder

librpc.a : rpc.o comms.o marshall.o rpc_client.o database.o
	ar -cvq librpc.a rpc.o comms.o marshall.o rpc_client.o database.o

rpc.o : rpc.cc
	$(CC) $(LIBRPCFLAGS) rpc.cc

comms.o : comms.cc
	$(CC) $(LIBRPCFLAGS) comms.cc

marshall.o : marshall.cc
	$(CC) $(LIBRPCFLAGS) marshall.cc

rpc_client.o : rpc_client.cc
	$(CC) $(LIBRPCFLAGS) rpc_client.cc

database.o : database.cc
	$(CC) $(LIBRPCFLAGS) database.cc

binder : binder.o comms.o marshall.o database.o
	$(CC) comms.o marshall.o database.o binder.o -o binder

binder.o : binder.cc
	$(CC) $(LIBRPCFLAGS) binder.cc

#librpc.a : rpc.cc comms.cc marshall.cc rpc_client.cc database.cc
#	$(CC) $(LIBRPCFLAGS) rpc.cc comms.cc marshall.cc rpc_client.cc database.cc
#	ar -cvq librpc.a rpc.o comms.o marshall.o rpc_client.o database.o

#binder : binder.cc comms.cc marshall.cc database.cc
#	$(CC) binder.cc comms.cc marshall.cc database.cc -o binder

clean :
	rm binder *.o *.a

