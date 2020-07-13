all:
	g++ -o client terminal.cpp chat.cpp codec.cpp main.cpp -pthread
run:
	./client || true

makegdb:
	g++ -g -o client terminal.cpp chat.cpp codec.cpp main.cpp -pthread
