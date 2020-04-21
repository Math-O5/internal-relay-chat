// source https://docs.oracle.com/cd/E19455-01/806-1017/
/*
family: AF_INET6, for IPv6  (128 bits) (16 bytes)
types SOCK_STREAM
*/
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h> 
#include<string.h>
#include<sys/types.h>  // SOCK_STREAM and others
#include<sys/socket.h> // socket()
#include<unistd.h>
#include<arpa/inet.h> 
#include<netinet/in.h>
#include <cstdio>
#include <iostream> 
#include <time.h>
#include<netdb.h> //hostent
#define STREAM_SIZE 4096

using namespace std;

#define TRUE   1  
#define FALSE  0  

int PORT,
    flag = 0,
    network_socket;
struct hostent *SERVER;

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void sucess(const char *msg) {
    printf("%s\n", msg);
    fflush(stdout);
}


void* recv_msg_handler(void* args)
{
    char server_response[STREAM_SIZE];
    while(TRUE)
    {
        if(recv(network_socket, &server_response, sizeof(server_response), 0) > 0)
        {
            printf("%s\n",server_response);
            fflush(stdout);
        }
    }
}

void* send_msg_handler(void* args)
{
    char client_message[STREAM_SIZE];

    while(TRUE)
    {
        fgets(client_message, STREAM_SIZE, stdin);
        write(network_socket, client_message, strlen(client_message));
        for(int i = (int) strlen(client_message); i < STREAM_SIZE; i++)
        {
            client_message[i] = '\0';
        }
        if(strncmp(client_message, "/quit", 4) == 0)
        {
            flag = 1;
        }
    }
}

// argv[0] filename
// argv[1] server_ipaddress
// argv[2] port_num
void load_const(char *argv[]) {
    PORT = atoi(argv[2]);
    SERVER = gethostbyname(argv[1]);
    if(SERVER == NULL)
    {
        error("[x] Error no such host.\n");
    }
}

void create_socket(char *argv[]) {

    load_const(argv);
    
    //create a socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(network_socket < 0) 
    {
        error("[x] No socket created.");
    }

    //specify an address for thesocket
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) SERVER->h_addr, (char *) &server_address.sin_addr.s_addr, SERVER->h_length); // localhost : 127.0.0.1 or any other IP
    server_address.sin_port = htons(PORT);

    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(connection_status < 0) {
        error("[x] Cannot connect to socket.\n");
    }

    //reiceve data
    char server_response[STREAM_SIZE];
    recv(network_socket, &server_response, sizeof(server_response), 0);
    printf("SERVER's: %s\n", server_response);

    return;
}

int main(int argc, char *argv[]) {

    if(argc < 3)
        error("[x] No host to found. Pass it with make run_client host=[INT] PORT=[INT].\nYou can check your local host with ifconfig. Normally, the localhost is 127.0.0.1\n");

    create_socket(argv);

    pthread_t send_msg_thread;
    if(pthread_create(&send_msg_thread, NULL, &send_msg_handler, NULL) != 0)
		error("ERROR: pthread\n");

	pthread_t recv_msg_thread;
    if(pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, NULL) != 0)
		error("ERROR: pthread\n");

	while(TRUE)
    {
		if(flag)
        {
			printf("\nBye\n");
			break;
        }
	}

    close(network_socket);
}

    