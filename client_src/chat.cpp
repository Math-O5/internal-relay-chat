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

    return rc;
}

// @Comentários em "chat.h"
int destruir_relay_chat(relay_chat* rc){
    if(rc->connection_status == CONNECTION_OPEN && rc->network_socket >= 0)
        if(fechar_conexao(rc)){
            return 1;
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
    if(rc == NULL || rc->connection_status == CONNECTION_OPEN)
        return 1;
    
    // Se o buffer não tiver sido alocado então essa variável ainda não foi inicializada.
    if(rc->send_buff == NULL || rc->recv_buff == NULL){
        return 2;
    }

    // Buscando dados do servidor
    rc->port = atoi(rc->sport);
    rc->server = gethostbyname(rc->sserver);
    if(rc->server == NULL){
        return 3;
    }

    // Criando o socket
    rc->network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(rc->network_socket < 0){
        return 4;
    }

    // Definindo endereço para conexão
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) rc->server->h_addr, (char *) &server_address.sin_addr.s_addr, rc->server->h_length); // localhost : 127.0.0.1 or any other IP
    server_address.sin_port = htons(rc->port);

    // Abrindo conexão com o servidor
    rc->connection_status = connect(rc->network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
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
    memset(server_response,0,strlen(server_response));

    // Verifica se há uma conexão ativa
    while(rc->connection_status == CONNECTION_OPEN){
        
        // Verifica se há mensagens para ler no buffer do socket.
        if(recv(rc->network_socket, &server_response, sizeof(server_response), 0) > 0){
            
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

            // limpando a variável auxiliar.
            memset(server_response,0,strlen(server_response));
        }

    }

}

// @Comentários em "chat.h"
void* send_msg_handler(void* vrc){
    relay_chat* rc = (relay_chat*) vrc;

    char client_message[MAX_MESSAGE_LENGHT+1];
    int  it = 0, it_message = 0;
    
    while(rc->connection_status == CONNECTION_OPEN) {
        
        pthread_mutex_lock(rc->send_mutex);
            
            if(rc->send_buff_size <= 0){}
                pthread_cond_wait(rc->cond_send_waiting, rc->send_mutex); 

            if(rc->send_buff_size > 0){
                it = 0;

                // Percorre todo o buffer enviando as mensagens na fila
                // ps: lembrando que as mensagens no protocolo são finalizadas
                // não por um \0, mas por um \r\n (CR-LF).
                while( it < BUFFER_SIZE && rc->send_buff[it] != '\0'){
                    
                    memset(client_message, '\0', sizeof(client_message));
                    it_message = 0;

                    while( it < BUFFER_SIZE && rc->send_buff[it] != '\n' && rc->send_buff[it] != '\0') {
                        client_message[it_message++] = rc->send_buff[it];
                        rc->send_buff[it++] = '\0';
                    };
                    client_message[it_message] = '\n';

                    write(rc->network_socket, client_message, strlen(client_message));
                    it++;
                }

                rc->send_buff_size  = 0;
            
            } else {

            }

        pthread_mutex_unlock(rc->send_mutex);
        
    }
}

// @COmentários em "chat.h"
char** encode_message(relay_chat *rc, const char* raw_str, int raw_str_len){
    if(rc == NULL || rc->connection_status != CONNECTION_OPEN || raw_str == NULL || raw_str_len <= 0){
        return NULL;
    }

    /** 1ª Etapa - Calculando quantos pacotes serão necessários
     *  @TODO: Nas próximas etapas deverá ser levado em conta o prefixo e outros
     *  strings obrigatórias como definido no protocolo. Por enquanto só quebrar as
     *  strings em blocos ja é o suficiente.
     * 
     * 
     * Obs: (MAX_MESSAGE_LENGHT-1) é necessário porque temos que descontar o espaço 
     * consumido pelo \n que é adicionado ao fim de cada mensagem do protocolo.
     */
    
    int qtd_mensagens  = raw_str_len / (MAX_MESSAGE_LENGHT - 1); 
    qtd_mensagens     += (raw_str_len % (MAX_MESSAGE_LENGHT - 1) > 0) ? 1 : 0;

    /**
     * 2ª Gerando os blocos e populando-os
     */
    
    int raw_it = 0;

    char** mensagem = (char**) calloc(qtd_mensagens+1, sizeof(char*));
    for(int i = 0; i < qtd_mensagens; i++){
        mensagem[i] = (char*) calloc(MAX_MESSAGE_LENGHT+1, sizeof(char));

        int msg_it = 0;
        while(raw_it < raw_str_len && msg_it < (MAX_MESSAGE_LENGHT - 1))
            mensagem[i][msg_it++] = raw_str[raw_it++];
        mensagem[i][msg_it] = '\n';
    }

    return mensagem;
}