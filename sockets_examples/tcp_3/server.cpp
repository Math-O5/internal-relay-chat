// source https://docs.oracle.com/cd/E19455-01/806-1017/
/*
family: AF_INET6, IPv6  pack: (128 bits) (16 bytes)
types SOCK_STREAM (TCP) 
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h> 
#include<iostream>
#include<set>

#include<netinet/in.h>

#define PORT 9002

using namespace std;

int main() {
    set<int> clients_socket;
    int high = 0;
    char server_message[4096] = "[+] You have reched the server!";
    char buffer[4096];
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
        printf("Listening....\n");
    } else {
        printf("[-]Error in biding.\n");
    }

    fd_set master;
    high = max(high, server_socket);

    while(true) {
        
        FD_ZERO(&master);
        FD_SET(server_socket, &master);
        for(set<int>::iterator it = clients_socket.begin(); it != clients_socket.end(); ++it) {
            FD_SET(*it, &master);
            high = max(high, *it);
        }

        int socket_count = select(high+1, &master, nullptr, nullptr, nullptr); //

        if(FD_ISSET(server_socket, &master)) {

            // new connection
            client_socket = accept(server_socket, nullptr, nullptr);
            clients_socket.insert(client_socket);
            
            printf("[+] new sock %d.\n", client_socket);
        	fflush(stdout);

            // add to the list of connected
            char welcome[] = "Welcome to the Awesome chat Server.\n";
            send(client_socket, welcome, strlen(welcome), 0);
            FD_CLR(server_socket, &master);
        }

        for(int i = 0; i < high+1; ++i) {
            
            if(FD_ISSET(i, &master)) {
                // new menssage
                memset(buffer, '\0', 4096*sizeof(char));
                int bytes_in = recv(i, buffer, 4096, 0);
                
                buffer[bytes_in] = '\0';

                if(bytes_in > 0) {
                    for(set<int>::iterator it = clients_socket.begin(); it != clients_socket.end(); ++it) {
                        send(*it, buffer, bytes_in, 0);
                    }
                }     
            }
        }
    }
    return 0;
}