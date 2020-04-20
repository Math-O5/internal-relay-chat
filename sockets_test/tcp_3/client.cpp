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
#define STDIN 0
int main() {
    //1 create a socket
    int network_socket;
    fd_set readys;
    int retval, len;
    char buffer[4096] = {0};

    //reiceve data
    char server_response[4096];
    char client_message[4096];

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

    while(true) {
        FD_ZERO(&readys);
        FD_SET(STDIN, &readys);
        FD_SET(network_socket, &readys);
        retval = select(network_socket+1, &readys, NULL, NULL, NULL);

        if (retval <= -1){
            perror("select()");
            exit(EXIT_FAILURE);
        }
        else {
            if(FD_ISSET(STDIN, &readys)){
                printf("\nUser input - stdin");
                /* FD_ISSET(0, &rfds) is true so input is available now. */

                /* Read data from stdin using fgets. */
                fgets(buffer, sizeof(buffer), stdin);

                /* Remove trailing newline character from the input buffer if needed. */
                len = strlen(buffer) - 1;
                if (buffer[len] == '\n')
                    buffer[len] = '\0';

                printf("'%s' was read from stdin.\n", buffer);
                send(network_socket, buffer, strlen(buffer), 0);
                // FD_CLR(network_socket, &readys);

            }
            if(FD_ISSET(network_socket, &readys)){
                // socket code
                printf("ops");
                if(recv(network_socket, &server_response, sizeof(server_response), 0) < 0) {
                    printf("[-]Error in receing data.\n");
                    exit(1);
                } else {
                    printf("Server: \t%s\n", server_response);
                }
            }
        }
    }

    close(network_socket);
    return 0;
}