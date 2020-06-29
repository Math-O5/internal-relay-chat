#include "chat.h"
#include <iostream>
#include <stdlib.h> /* atoi, */
#include <string.h> /* memset, */

// @Comentários em "chat.h"
relay_chat criar_relay_chat(pthread_mutex_t* send_mutex, pthread_mutex_t* recv_mutex, 
                                pthread_cond_t* cond_send_waiting, pthread_cond_t* cond_recv_waiting) {
    relay_chat rc;
    
    // Dados de conexão
    sprintf(rc.sserver, DEFAULT_SERVER_HOST);
    sprintf(rc.sport, DEFAULT_SERVER_PORT);
    rc.server = NULL;
    rc.port   = -1;
    rc.network_socket = -1;
    rc.connection_status = CONNECTION_CLOSED;

    // Abrindo os Buffers Conexão
    rc.send_buff = (char*) calloc(BUFFER_SIZE, sizeof(char));
    rc.recv_buff = (char*) calloc(BUFFER_SIZE, sizeof(char));
    rc.recv_buff_size = 0;
    rc.send_buff_size = 0;

    // Mutexes
    rc.send_mutex = send_mutex;
    rc.recv_mutex = recv_mutex; 

    rc.cond_send_waiting   = cond_send_waiting;
    rc.cond_recv_waiting = cond_recv_waiting;

    // Definindo o estado do cliente
    rc.is_admin = rc.has_channel = rc.has_nick = false;
    memset(rc.nickname, 0, sizeof(rc.nickname));
    memset(rc.channel, 0, sizeof(rc.channel));
    rc.nick_len = rc.channel_len = 0;

    return rc;
}

// @Comentários em "chat.h"
int destruir_relay_chat(relay_chat* rc){
    if(rc->connection_status == CONNECTION_OPEN && rc->network_socket >= 0){
        if(fechar_conexao(rc)){
            return 1;
        }
    }
    
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

// @Comentários em "chat.h"
int abrir_conexao(relay_chat* rc){
    // variável não existe ou já há uma conexão ativa.
    if(rc == NULL || rc->connection_status == CONNECTION_OPEN){
        return 1;
    }
    
    // Se o buffer não tiver sido alocado então essa variável ainda não foi inicializada.
    if(rc->send_buff == NULL || rc->recv_buff == NULL){
        return 2;
    }

    // Buscando dados do servidor
    rc->port   = atoi(rc->sport);
    rc->server = gethostbyname(rc->sserver);
    if(rc->server == NULL){
        return 3;
    }
    // printf("[x]   | --- Convertendo hostname and port.\n");

    // Criando o socket
    rc->network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(rc->network_socket < 0){
        return 4;
    }
    // printf("[x]   | --- Abrindo socket de conexão.\n");

    // Definindo endereço para conexão
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) rc->server->h_addr, (char *) &server_address.sin_addr.s_addr, rc->server->h_length); // localhost : 127.0.0.1 or any other IP
    server_address.sin_port = htons(rc->port);
    // printf("[x]   | --- Configurando endereços de conexão.\n");

    // Abrindo conexão com o servidor
    rc->connection_status = connect(rc->network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    // printf("[x]   | --- Conectand ao servidor...\n");
    
    if(rc->connection_status == CONNECTION_CLOSED){ 
        return 3;
    }

    return 0;
}

// @Comentários em "chat.h"
int fechar_conexao(relay_chat* rc){
    if(rc->connection_status == CONNECTION_CLOSED || rc->network_socket < 0)
        return 1;
    
    close(rc->network_socket);
    rc->connection_status = CONNECTION_CLOSED;
    rc->network_socket    = -1;

    return 0;
}

// @Comentários em "chat.h"
void* recv_msg_handler(void* vrc){
    relay_chat* rc = (relay_chat*) vrc;

    // Variável auxiliar temporária.
    char server_response[MAX_MESSAGE_LENGHT+1];

    // Verifica se há uma conexão ativa
    while(rc->connection_status == CONNECTION_OPEN){
        
        // limpando a variável auxiliar.
        memset(server_response,0,sizeof(server_response));

        // Obs: Recv é uma chamada bloqueante que retorna 0 caso a conexão com o servidor
        // tenha caido. Caso contrário ela irá aguardar até que de fato haja algo no buffer
        // para ser lido.
        int recv_return = recv(rc->network_socket, &server_response, MAX_MESSAGE_LENGHT, 0);
        // printf("return [%d]\n", recv_return);

        // Verifica se há mensagens para ler no buffer do socket.
        if( recv_return > 0 ){
            
            // Mutex protege orecv_buffer e cond_recv_sinaliza a thread de Output
            pthread_mutex_lock(rc->recv_mutex);
                pthread_cond_signal(rc->cond_recv_waiting); 

                // Concatenando os dados no buffer.
                if(strlen(rc->recv_buff) == 0){
                    strcpy(rc->recv_buff, server_response);
                } else {
                    strcat(rc->recv_buff, server_response);
                }
                rc->recv_buff_size = strlen(rc->recv_buff); 

            pthread_mutex_unlock(rc->recv_mutex);
        
        // Verifica se a conexão morreu
        } else if(recv_return <= 0){
            pthread_mutex_lock(rc->state_mutex);
                fechar_conexao(rc);
            pthread_mutex_unlock(rc->state_mutex);
        }

        // sleep(2);
    }

    return NULL;
}

// @Comentários em "chat.h"
void* send_msg_handler(void* vrc){
    relay_chat* rc = (relay_chat*) vrc;

    char client_message[MAX_MESSAGE_LENGHT+1];
    int  it = 0, it_message = 0;
    
    while(rc->connection_status == CONNECTION_OPEN) {
        
        pthread_mutex_lock(rc->send_mutex);
            
            if(rc->send_buff_size <= 0){
                pthread_cond_wait(rc->cond_send_waiting, rc->send_mutex); 
            }

            if(rc->send_buff_size > 0){
                it = 0;

                // Percorre todo o buffer enviando as mensagens na fila
                // ps: lembrando que as mensagens no protocolo são finalizadas
                // não por um \0, mas por um \r\n (CR-LF).
                while( it < rc->send_buff_size ){
                    
                    memset(client_message, '\0', sizeof(client_message));
                    it_message = 0;

                    while( it < rc->send_buff_size && rc->send_buff[it] != '\n' && rc->send_buff[it] != '\0') {
                        client_message[it_message++] = rc->send_buff[it++];
                    }
                    client_message[it_message] = '\n';
                    it++;
                    // printf("Mensagem: '%s'", client_message);

                    // int write_return = write(rc->network_socket, client_message, strlen(client_message));
                    int send_return = send(rc->network_socket, client_message, strlen(client_message), 0);

                    if(send_return <= 0){
                        pthread_mutex_lock(rc->state_mutex);
                            rc->connection_status = CONNECTION_CLOSED;
                        pthread_mutex_unlock(rc->state_mutex);
                        break;
                    } 

                }

                rc->send_buff_size  = 0;
                memset(rc->send_buff, 0, sizeof(rc->send_buff));
            } 

        pthread_mutex_unlock(rc->send_mutex);
        
    }

    return NULL;
}

