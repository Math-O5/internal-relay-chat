/*
filename server_ipaddress port_num

argv[0] filename
argv[1] server_ipaddress
argv[2] port_num


*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h> 

#include<netdb.h> //hostent


char default_port[] = "9003";

void error(const char *msg)
{
    perror(msg);
    exit(1);    
}

void flag_sucess(const char *msg)
{
    printf("[+] %s", msg);
}

int main(int argc, char *argv[]) {
    int sock_fd, port_num, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[255];
    if(argc < 3)
    {
        fprintf(stderr, "usage %s hostname  port\n", argv[0]);
        exit(0);
    }

    port_num = atoi(argv[2]);
    
    // create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0) {
        error("[x] Error open socket.\n");
    }

    server = gethostbyname(argv[1]);
    if(server == NULL)
    {
        error("[x] Error no such host.\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port_num);

    if(connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("[x] Connection failed.\n");
    }
    flag_sucess("Connected.\n");

    while(1) 
    {
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        n = write(sock_fd,  buffer, strlen(buffer));
        if(n < 0)
        {
            error("[x] Error on writting.\n");
        }
        bzero(buffer, 255);
        n = read(sock_fd, buffer, 255);
        printf("Server: %s\n", buffer);
        if(strncmp(":exit", buffer, 5) == 0)
            break;

    }

    close(sock_fd);
}
