make:
	g++ server.cpp -o server
	g++ client.cpp -o client
run server:
	echo "Server running on " $(shell pwd)
	./server ${PORT}

# run client:
# 	echo "Client running on " $(shell pwd)
# 	./client ${host} ${PORT}