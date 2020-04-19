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

#define PORT 9002

int main() {

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
        printf("Listening...\n");
    } else {
        printf("[-]Error in biding.\n");
    }


    fd_set master;
    FD_ZERO(&master);

    FD_SET(server_socket, &master);

    while(true) {
        
        fd_set copy = master; //copy master because it ll be destroyed at end of while

        int socket_count = select(0, &copy, nullptr, nullptr, nullptr, nullptr); //

        for(int i = 0; i < socket_count; ++i) {
            int socket_copy = copy.fd_array[i];
            if(socket_copy == server_socket) {
                // new connection
                client_socket = accept(server_socket, nullptr, nullptr);

                // add to the list of connected
                FD_SET(client_socket, &master);

                char welcome[] = "Welcome to the Awesome chat Server.\n";
                send(client_socket, welcome, strlen(welcome), 0);
            } else {
                // new menssage
                memset(buffer, '\0', 4096*sizeof(char));
                int bytes_in = recv(socket_copy, buffer, 4096, 0) <= 0) {
                if(bytes_in <= 0) {
                    // drop the client
                    close(socket_copy);
                    FD_CLR(socket_copy, &master);
                } else {
                    // send to ohter clients 
                    for(int i = 0; i < master.fd_count; ++i) {
                        int out_socket= copy.fd_array[i];
                        if(out_socket != client_socket && out_socket != server_socket) {
                            send(out_socket, buffer, bytes_in, 0);
                        }
                    }
                }
            }
        }
    }

    // while(1) {
    //     /*
    //         When the clients calls, the event happens. (the process sleep while no events arrive).
    //         parametors: fp os server sockt, NULL or teh address struct of client socket, NULL or the size of the struct address client.
    //     */
    //     client_socket = accept(server_socket, (struct sockaddr*) &client_address, &add_size);
    //     printf("[+] Create client socket: %d\n", client_socket);
    //     if(client_socket < 0) {
    //         exit(1);
    //     }
    //     printf("Connected accepted %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    //     if((childpid = fork()) == 0) {
    //         close(server_socket);

    //         while(1) {
    //             recv(client_socket, buffer, 256, 0);
    //             printf("message from client socket: %d\n", client_socket);
    //             if(strcmp(buffer, ":exit") == 0) {
    //                 printf("Disconnected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    //                 break;
    //             } else {
    //                 printf("Client: %s\n", buffer);
    //                 send(client_socket, buffer, strlen(buffer), 0);
    //                 bzero(buffer, sizeof(buffer));
    //             }
    //         }
    //     }
    // }

    // close(client_socket);

    return 0;
}