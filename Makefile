all:
	server

server: server.cpp
	g++ -Wall server.cpp -o server

client:
	g++ -Wall client.cpp -lpthread -o client

run_server:
	echo "Server running on " $(shell pwd)
	./server ${PORT} ${nLISTEN}

run_client:
	echo "Client running on " $(shell pwd)
	./client ${host} ${PORT}