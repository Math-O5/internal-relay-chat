// source https://docs.oracle.com/cd/E19455-01/806-1017/
#include<stdio.h>
#include<stdlib.h>
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

#define PORT 8888

int main() {
    //1 create a socket
    int network_socket;
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
    char server_response[256];
    char client_message[4096];
    printf("Serve's answer: %s\n", server_response);

    while(1) {
        printf("Client: \t");
        scanf("%s", &client_message[0]);
        send(network_socket, client_message, strlen(client_message), 0);

        if(strcmp(client_message, ":exit") == 0) {
            close(network_socket);
            printf("[-]Disconnected from server.\n");
            exit(1);
        }

        if(recv(network_socket, &server_response, sizeof(server_response), 0) < 0) {
            printf("[-]Error in receing data.\n");
            exit(1);
        } else {
            printf("Server: \t%s\n", server_response);
        }

    }
    close(network_socket);

}