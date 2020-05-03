#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/types.h>
#include<sys/socket.h>

#include<unistd.h>
#include<arpa/inet.h> 

int main() {
    FILE    *html_data;
    char    response_data[1024],
            html_header[2048] = "HTTP/1.1 OK\r\n\n";    
    int server_socket;

    // open the file
    html_data = fopen("index.html", "r");

    // get file data into response_data
    fgets(response_data, 1024, html_data);

    //concat http_header with response
    strcat(html_header, response_data);

    //create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //define address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons()
    


    return 0;
}
