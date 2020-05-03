all:
	server

server: server_src/server.cpp
	g++ -Wall server_src/server.cpp -o server

client: client.cpp
	g++ -Wall client_src/client.cpp -lpthread -o client

run_server:
	echo "Server running on " $(shell pwd)
	./server ${PORT} ${nLISTEN}

run_client:
	echo "Client running on " $(shell pwd)
	./client ${host} ${PORT}