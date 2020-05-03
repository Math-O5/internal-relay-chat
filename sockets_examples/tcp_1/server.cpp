// source https://docs.oracle.com/cd/E19455-01/806-1017/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h> 
#include<map>

using namespace std;

/*
family: AF_INET6, for IPv6  (128 bits) (16 bytes)
        AF_INET, for IPv4 (32 bits), sustenta 2^32 ips
        AF_UNIX, for local socket (using a file)
        It is a network protocol. 
types SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW, 
*/

#include<netinet/in.h>

#define PORT 9002

map<int, int> clients;

int main() {

    char server_message[256] = "[+] You have reched the server!";
    char client_message[256];
    // 1. create a socket
    // parameters family, domain, protocol
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // storage the fd of client and your address.    
    int client_socket;
    struct sockaddr_in client_address;
    pid_t childpid;
    socklen_t add_size;

    // 2. define socket address
    struct sockaddr_in server_address;
    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    /*
        Associate the process we're running to a buffer(socket).
    */
    if(bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        printf("[-]Error in biding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d.\n", PORT);

    /*
        Start running the process of the socket on IP address. 
    */
    // parameters sockt id, backlog the number of connect it can support.
    if(listen(server_socket, 2) == 0) {
        printf("Listening...\n");
    } else {
        printf("[-]Error in biding.\n");
    }


    while(1) {
        /*
            When the clients calls, the event happens. (the process sleep while no events arrive).
            parametors: fp os server sockt, NULL or teh address struct of client socket, NULL or the size of the struct address client.
        */
        client_socket = accept(server_socket, (struct sockaddr*) &client_address, &add_size);
        if(clients.find(client_socket) == clients.end()) {
            printf("[+] Create client socket: %d\n", client_socket);
        }

        if(client_socket < 0) {
            exit(1);
        }
        printf("Connected accepted %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        if((childpid = fork()) == 0) {
            close(server_socket);

            while(1) {
                recv(client_socket, client_message, 256, 0);
                printf("message from client socket: %d\n", client_socket);
                if(strcmp(client_message, ":exit") == 0) {
                    printf("Disconnected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                    break;
                } else {
                    printf("Client: %s\n", client_message);
                    send(client_socket, client_message, strlen(client_message), 0);
                    
                    bzero(client_message, sizeof(client_message));
                }
            }
        }
    }

    close(client_socket);

    return 0;
}