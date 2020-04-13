#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h> // family: AF_INET6, AF_INET (...), types SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW, 

#include<netinet/in.h>

#define PORT 9001
int main() {

    char server_message[256] = "[+] You have reched the server!";

    // 1. create a socket
    // parameters family, domain, protocol
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // 2. define server address
    struct sockaddr_in server_address;
    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our  specified IP and port
    if(bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        printf("[-]Error in biding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d.\n", PORT);

    if(listen(server_socket, 2) == 0) {
        printf("Listening...\n");
    } else {
        printf("[-]Error in biding.\n");
    }

    int client_socket;
    // struct sockaddr_in client_address;

    // while(1) {
        client_socket = accept(server_socket, NULL, NULL);

        if(client_socket < 0) {
            exit(1);
        }
        printf("Connected accepted\n");
        send(client_socket, server_message, sizeof(server_message), 0);
    // }

    
    close(server_socket);

    return 0;
}