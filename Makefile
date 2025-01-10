all: server client

# to build the server executable
server: server.o
	gcc server.o -o server

# to compile the server object file
server.o: server.c
	gcc -c server.c -o server.o

# to build the client executable
client: client.o
	gcc client.o -o client

# to compile the client object file
client.o: client.c
	gcc -c client.c -o client.o

# Build debug version of the server
debug_server: server.c
	gcc -g server.c -o debug_server

# Build debug version of the client
debug_client: client.c
	gcc -g client.c -o debug_client

# Clean all generated files
clean:
	rm -f server client debug_server debug_client *.o *.out
