// source https://docs.oracle.com/cd/E19455-01/806-1017/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h> 

char default_port[] = "9003";

void error(const char *msg)
{
    perror(msg);
    exit(1);    
}

void flag_sucess(const char *msg)
{
    printf("[+] %s\n", msg);
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        // printf("[+] Port number default %s\n", default_port);
        // argc = 2;
        // argv[1] = default_port;
        error("[x] No port given.");
    }

    int sock_fd, newsock_fd, port_num, n;
    char buffer[255];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;

    // create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0) {
        error("[x] Error open socket.");
    }

    // define address
    bzero((char * ) &serv_addr, sizeof(serv_addr));

    port_num = atoi(argv[1]);
    printf("[+] Port number %d\n", port_num);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num); // host to network
    
    //binding socket 
    if(bind(sock_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        error("[x] Can't binding the server.");
    }
    flag_sucess("Binding the server...");

    // start listing
    listen(sock_fd, 5);
    cli_len = sizeof(cli_addr);

    // accept connections (5)
    newsock_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &cli_len);
    printf("sock %d", newsock_fd);
    
    if(newsock_fd < 0) 
    {
        error("[x] Connection not accepted.\nTry again.");
    }
    

    while(1)
    {
        bzero(buffer, 255);
        n = read(newsock_fd, buffer, 255);
        if(n < 0) 
        {
            error("[x] Error on reading.");
        }

        printf("Client: %s\n", buffer);
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);

        n = write(newsock_fd, buffer, strlen(buffer));
        if(n < 0)
        {
            error("[x] Error on writing.");
        }

        if(!strncmp(":exit", buffer, sizeof(char)*5))
        {
            printf("[-] Disconnected from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            break;
        }

    }
    close(newsock_fd);
    close(sock_fd);

    return 0;
}