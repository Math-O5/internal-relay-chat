#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>
#define PORT 9001


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
    printf("Anser: %s", server_response);

    while(1) {
        printf("Client: \t");
        scanf("%s", &client_message[0]);
        send(network_socket, client_message, strlen(client_message), 0);

        if(strcmp(client_message, ":exit") == 0) {
            printf("[-]Disconnected from server.\n");
            exit(1);
        }

        if(recv(network_socket, &server_response, sizeof(server_response), 0) < 0) {
            printf("[-]Error in receing data.\n");
            exit(1);
        }

        printf("Server: \t%s\n", server_response)

    }
    close(network_socket);

}