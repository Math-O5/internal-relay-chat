// source https://docs.oracle.com/cd/E19455-01/806-1017/
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h> 
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h> 
/*
family: AF_INET6, for IPv6  (128 bits) (16 bytes)
        AF_INET, for IPv4 (32 bits), sustenta 2^32 ips
        AF_UNIX, for local socket (using a file)
        It is a network protocol. 
types SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW, 
*/

#include<netinet/in.h>

#include <cstdio>
#include <iostream>

#include <time.h>

using namespace std;

#define PORT 9002

int flag = 0;
int network_socket;

void* recv_msg_handler(void* args){

    char server_response[4096];
    while(1){
        recv(network_socket, &server_response, sizeof(server_response), 0);
        printf("%s",server_response);
    }

    
}

void* send_msg_handler(void* args){

    char client_message[4096];
    while(1){
        fgets(client_message,4096,stdin);
        write(network_socket,client_message,strlen(client_message));
        for(int i = (int) strlen(client_message); i < 4096; i++){
            client_message[i] = '\0';
        }
        if(strcmp(client_message, "/quit") == 0){
            flag = 1;
            break;
        }

    }
}

int main() {
    //1 create a socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //specify an address for thesocket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY; // localhost : inet_addr("127.0.0.1")

    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(connection_status < 0) {
        printf("Cannot connect to another socket.\n");
        exit(1);
    }

    //reiceve data
    char server_response[4096];
    recv(network_socket, &server_response, sizeof(server_response), 0);
    char client_message[4096];
    printf("Server's answer: %s\n", server_response);

    pthread_t send_msg_thread;
    if(pthread_create(&send_msg_thread, NULL, &send_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
        return EXIT_FAILURE;
	}

	pthread_t recv_msg_thread;
    if(pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	while (1){
		if(flag){
			printf("\nBye\n");
			break;
        }
	}

    close(network_socket);

}

    