#include "chat.h"
#include <iostream>
#include <stdlib.h> /* atoi, */
#include <string.h> /* memset, */

// Função para inicializar o struct
relay_chat criar_relay_chat(pthread_mutex_t* send_mutex, pthread_mutex_t* recv_mutex) {
    relay_chat rc;
    // Dados de conexão
    sprintf(rc.sserver, "127.0.0.1");
    sprintf(rc.sport, "9002");
    rc.server = NULL;
    rc.port   = -1;
    rc.network_socket = -1;
    rc.connection_status = CONNECTION_CLOSED;

    // Buffers de Conexão
    rc.send_buff = NULL;
    rc.recv_buff = NULL;
    rc.recv_buff_size = 0;
    rc.send_buff_size = 0;

    // Mutexes
    rc.send_mutex = send_mutex;
    rc.recv_mutex = recv_mutex; 
    return rc;
}

// Função para destruir o struct
int destruir_relay_chat(relay_chat* rc){
    if(rc->connection_status == CONNECTION_OPEN && rc->network_socket >= 0)
        if(fechar_conexao(rc)){
            return 1;
        }

    (*rc) = criar_relay_chat(rc->send_mutex, rc->recv_mutex);
    return 0;
}

// Abre uma conexão baseada nos dados definidso em shost e sport
int abrir_conexao(relay_chat* rc){

    // Buscando dados do servidor
    rc->port = atoi(rc->sport);
    rc->server = gethostbyname(rc->sserver);
    if(rc->server == NULL)
        return 1;
    
    // Criando o socket
    rc->network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(rc->network_socket < 0)
        return 2;

    // Definindo endereço para conexão
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) rc->server->h_addr, (char *) &server_address.sin_addr.s_addr, rc->server->h_length); // localhost : 127.0.0.1 or any other IP
    server_address.sin_port = htons(rc->port);

    // Abrindo conexão com o servidor
    rc->connection_status = connect(rc->network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(rc->connection_status != CONNECTION_OPEN) 
        return 3;
    
    // Abrindo os buffers
    rc->send_buff = (char*) calloc(BUFFER_SIZE, sizeof(char));
    rc->recv_buff = (char*) calloc(BUFFER_SIZE, sizeof(char));

    if(rc->send_buff == NULL || rc->recv_buff == NULL){
        fechar_conexao(rc);
        return 4;
    }

    return 0;
}

// Fecha a conexão e muda o status corretamente.
int fechar_conexao(relay_chat* rc){
    if(rc->connection_status == CONNECTION_CLOSED || rc->network_socket < 0)
        return 1;
    
    close(rc->network_socket);
    rc->connection_status = CONNECTION_CLOSED;
    rc->network_socket    = -1;

    // Excluindo os buffers utilizados
    if(rc->send_buff != NULL){
        free(rc->send_buff);
        rc->send_buff == NULL;
    }
    if(rc->recv_buff != NULL){
        free(rc->recv_buff);
        rc->recv_buff == NULL;
    }

    return 0;
}

// Verifica o buffer do socket e salva no buffer de input
void* recv_msg_handler(void* vrc){
    relay_chat* rc = (relay_chat*) vrc;

    char server_response[MAX_MESSAGE_LENGHT+1];
    memset(server_response,0,strlen(server_response));

    while(rc->connection_status == CONNECTION_OPEN){
        
        if(recv(rc->network_socket, &server_response, sizeof(server_response), 0) > 0){
            
            // Mutex protege o buffer de mensagens recebidas
            pthread_mutex_lock(rc->recv_mutex);

            if(strlen(rc->recv_buff) == 0){
                strcpy(rc->recv_buff, server_response);
            } else {
                strcat(rc->recv_buff, server_response);
            }
           rc->recv_buff_size = strlen(rc->recv_buff); 

            pthread_mutex_unlock(rc->recv_mutex);

            memset(server_response,0,strlen(server_response));
        }
    }

}

// Verifica o buffer de envio e escreve as mensagens no socket
void* send_msg_handler(void* vrc){
    relay_chat* rc = (relay_chat*) vrc;

    char client_message[MAX_MESSAGE_LENGHT+1];
    int  it = 0, it_message = 0, msg_len = 0;
    

    while(rc->connection_status == CONNECTION_OPEN) {
        
        it = 0;
        it_message = 0;
        msg_len = 0;

        if(rc->send_buff_size > 0){
            pthread_mutex_lock(rc->send_mutex);

            // Percorre todo o buffer enviando as mensagens na fila
            // ps: lembrando que as mensagens no protocolo são finalizadas
            // não por um \0, mas por um \r\n (CR-LF).
            while(rc->send_buff[it] != '\0'){
                memset(client_message,0,strlen(client_message));
                while(rc->send_buff[it] != '\n'){
                    client_message[it_message++] = rc->send_buff[it];
                    rc->send_buff[it++] = '\0';
                }
                write(rc->network_socket, client_message, strlen(client_message));
                it_message = 0;
            }

            rc->send_buff_size  = 0;
            pthread_mutex_unlock(rc->send_mutex);
        }
        
    }
}