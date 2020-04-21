//Example code: A simple server side code, which echos back the received server_response. 
//Handle multiple socket connections with select and fd_set on Linux  
#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#include <string>

#define TRUE   1  
#define FALSE  0  
#define STREAM_SIZE 4096

using namespace std;

fd_set readfds;         //set of socket descriptors  

int PORT;  
int max_sd, max_clients; 
int client_socket[30];   
        
char buffer[STREAM_SIZE];      // data buffer of 1K   
char server_response[STREAM_SIZE] = "[+] Server: Welcome !!!";  // Serve message  

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void sucess(const char *msg) {
    printf("%s\n", msg);
    fflush(stdout);
}

void create_socket(int *master_socket, int num_listen) {
    int opt = TRUE;
    struct sockaddr_in address;   

    //create a master socket  
    if((*master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        error("[x] Socket creating failed");   
    }   
     
    //set master socket to allow multiple connections ,  
    if(setsockopt(*master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )   
    {   
        error("[x] setsockopt");   
    }   
     
    //type of socket created  
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 9002  
    if (bind(*master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)   
    {   
        error("[x] bind failed on this port");   
    }   
    printf("[+] Listener on port %d\n", PORT);   
         
    //try to specify maximum of num_list pending connections for the master socket  
    if (listen(*master_socket, num_listen) < 0)   
    {   
        error("[x] Listen");   
    }   
         
    //accept the incoming connection  
    sucess("[+] Waiting for connections ...");  

    return;
}   

void build_sets(int master_socket) {
    
    int sd; 

    FD_ZERO(&readfds);    //clear the socket set  
    FD_SET(master_socket, &readfds);   //add master socket to set  
    max_sd  = master_socket;   
            
    //add child sockets to set  
    for (int index = 0 ; index < max_clients ; ++index)   
    {   
        //socket descriptor  
        sd = client_socket[index];   
                
        //if valid socket descriptor then add to read list  
        if(sd > 0)   
            FD_SET(sd , &readfds);   
                
        //highest file descriptor number, need it for the select function  
        if(sd > max_sd )   
            max_sd = sd;   
    } 
    return;
}

void send_all_message(const int from_id, const char *from_name, char *msg) {
    string s = from_name;
    s.append("(");
    s.append(to_string(from_id));
    s.append("): ");
    s.append(msg);
    printf("%s", &s[0]);
    fflush(stdout);

    for(int index = 0; index <= max_clients; ++index){
        if(client_socket[index] > 0){
            send(client_socket[index] , &s[0] , s.size() , 0 );
        }
    }
}

int main(int argc , char *argv[])   
{   
    int master_socket, sd, new_socket,
        valread, index,
        activity,
        addrlen;
    struct sockaddr_in address;   
    
    if(argc < 3)
    {
        error("[x] No port given. Set PORT=[INT] nLISTEN=[INT]");
    }

    PORT = atoi(argv[1]);
    max_clients = atoi(argv[2]);
    create_socket(&master_socket, max_clients);  
     
    //initialise all client_socket[] to 0 so not checked  
    for (index = 0; index < max_clients; ++index)   
    {   
        client_socket[index] = 0;   
    }   

    while(TRUE)   
    {      
        build_sets(master_socket);

        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            error("[x] Select error");  
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                error("[x] accept");     
            }   

            //inform user of socket number - used in send and receive commands  
            printf("[x] New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs (address.sin_port));   
            fflush(stdout);

            //send new connection greeting server_response  
            if((size_t)send(new_socket, server_response, strlen(server_response), 0) != (size_t)strlen(server_response))   
            {   
                error("[x] On send");   
            }   
                 
            sucess("Welcome server_response sent successfully");   
                 
            //add new socket to array of sockets  
            for(index = 0; index < max_clients; ++index)   
            {   
                //if position is empty  
                if(client_socket[index] == 0)   
                {   
                    client_socket[index] = new_socket;   
                    printf("Adding to list of sockets as %d\n", index);   
                    fflush(stdout);
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for(index = 0; index < max_clients; ++index)   
        {   
            sd = client_socket[index];   

            if(sd == master_socket) continue;
                 
            if(FD_ISSET(sd , &readfds))   
            {    
                //Check if it was for closing , and also read the  
                //incoming server_response  
                if ((valread = read(sd , &buffer, sizeof(buffer))) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);   
                    string addr = inet_ntoa(address.sin_addr);
                    string s = "Host disconnected, client: " + to_string(sd) + " ip: " + addr + " port: " + to_string(ntohs(address.sin_port)) + "\n\0";

                    send_all_message(0, "Server", &s[0]);

                    //Close the socket and mark as 0 in list for reuse  
                    close(sd);   
                    client_socket[index] = 0;   
                }   
                     
                //Echo back the server_response that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end of the data read  
                    for(int index = valread; index <= STREAM_SIZE; ++index){
                        buffer[index] = '\0';
                    }  

                    send_all_message(sd, "Client", buffer);
                }   
            }   
        }   
    }   
         
    return 0;   
}   