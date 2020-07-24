all:
	server

server:
	g++ -o server server_src/channel.cpp server_src/decodec.cpp server_src/message.cpp server_src/clients.cpp server_src/connection.cpp server_src/main.cpp -lpthread

client:
	g++ -o client client_src/terminal.cpp client_src/chat.cpp client_src/codec.cpp client_src/main.cpp -pthread

server_run:
	echo "Server running on " $(shell pwd)
	./server ${PORT} ${nLISTEN} || true

client_run:
	echo "Client running on " $(shell pwd) 
	./client