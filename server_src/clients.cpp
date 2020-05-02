#include "clients.h"

// Array com os structs dos clientes
client *cl_arr[MAX_CLIENTS];

// @Comentários em "clients.h"
client* clt_criar(struct sockaddr_in address, int socket, int id, int sv_socket){
    client* cl = (client*) malloc(sizeof(client));
    cl->cl_address = address;
    cl->cl_socket = socket;
    cl->cl_id = id;
    cl->sv_socket = sv_socket;
    return cl;
}


// @Comentários em "clients.h"
int clt_add_queue(client* cl, int max_cl, pthread_mutex_t* mutex){

    pthread_mutex_lock(mutex);

    // Adiciona o cliente no primeiro espaco disponivel
    int index;
    for(index = 0; index < max_cl; index++){
        if(!cl_arr[index]){
            cl_arr[index] = cl;
            break;
        }
    }

    pthread_mutex_unlock(mutex);

    // Numero maximo de clientes ja foi atingido...
    if(index == max_cl)
        return FAIL;

    return SUCCESS;
}

// @Comentários em "clients.h"
void clt_destruir(int id){
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i] != NULL && cl_arr[i]->cl_id == id) {
            close(cl_arr[i]->cl_socket);
            cl_arr[i] = NULL;
            free(cl_arr[i]);
            break;
        }
    }
}

// @Comentários em "clients.h"
void clt_destruir_clientes() {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i] != NULL) {
            close(cl_arr[i]->cl_socket);
            free(&(*cl_arr[i]));
        }
    }
}

// @Comentários em "clients.h"
client* clt_remove_queue(int id, int max_cl, pthread_mutex_t* mutex){
    
    pthread_mutex_lock(mutex);  

    // Procura pelo cliente, atraves de seu id, e o retira do array
    client* temp = NULL;
    for(int i = 0; i < max_cl; i++){
        if(cl_arr[i] && cl_arr[i]->cl_id == id){
            temp = cl_arr[i];
            cl_arr[i] = NULL;
            break;
        }
    }

    pthread_mutex_unlock(mutex);

    return temp;
}

// @Comentários em "clients.h"
client* clt_get_by_id(int id, int max_clients){

    for(int i = 0; i < max_clients; i++){
        if(cl_arr[i] && cl_arr[i]->cl_id == id){
            return cl_arr[i];
        }
    }
    return NULL;
}

// @Comentários em "clients.h"
bool clt_send_message(int cl_socket, char* buffer) {
    int try_send = 0;

    // Tenta enviar a mensagem até receber confirmação. Se após 
    while(send(cl_socket, buffer, strlen(buffer), 0) < 0 && try_send < 6) {
        try_send += 1;
    }
    return (try_send == 6) ? false : true;
}

// @Comentários em "clients.h"
void clt_send_message_all(int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer){

    // Configuracao da mensagem a ser enviada (adiciona que enviou a mensagem no conteudo)
    char msg_buffer[BUFFER_SIZE];
    sprintf(msg_buffer, ">> <CLIENTE %d>: %s\n", id_cur, buffer);

    pthread_mutex_lock(mutex);

    // Envio da mensagem aos clientes
    for(int i = 0; i < max_conn; i++){
        if(cl_arr[i] != NULL){
            if(clt_send_message(cl_arr[i]->cl_socket, msg_buffer)) {
                msg_send_cliente(id_cur, cl_arr[i]->cl_id);
            } else {
                msg_client_no_response(cl_arr[i]->cl_id);
                clt_destruir(cl_arr[i]->cl_id);
            } 
        }
    }

    pthread_mutex_unlock(mutex);
}

// @Comentários em "clients.h"
int clt_read_buffer(client* cl, char* buffer) {
    
    // Comando: /ping
    if(strncmp(buffer, "/ping", 5) == 0) {
        
        char log[] = ">> <SERVER>: pong\n";
        memset(buffer, '\0', BUFFER_SIZE);
        msg_info_ping(cl->cl_id);

        if(clt_send_message(cl->cl_socket, log)) {
            msg_info_pong(cl->cl_id);
        } 
        return PING;

    // Comando: /quit
    } else if(strncmp(buffer, "/quit", 5) == 0) {
        memset(buffer, '\0', BUFFER_SIZE);
        return QUIT;
    } 

    // Mensagem a ser enviada para os outros clientes
    return CONTINUE;
}

// @Comentários em "clients.h"
void clt_run(int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex){

    // Buffer com as mensagens dos clientes 
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    // Recupera as informacoes do cliente...
    client* cl = clt_get_by_id(id_cur, max_conn);

    // Cliente indisponível
    if(cl == NULL) {
        return;
    }

    // Mensagem com as informacoes do cliente
    msg_info_client(id_cur, cl->cl_socket, cl->cl_address);

    while(true){

        // Mensagem recebida !
        if(recv(cl->cl_socket, buffer, BUFFER_SIZE, 0) > 0){
            int input = clt_read_buffer(cl, buffer);
            switch(input) {
                case CONTINUE:
                    msg_recv_cliente(id_cur, buffer);
                    clt_send_message_all(id_cur, max_conn, mutex, buffer);
                    bzero(buffer, BUFFER_SIZE);
                    break;
                case QUIT:
                    msg_cliente_desconexao(cl->cl_id);
                    return;
                default: break;
            }
        }
        // Ocorreu um erro na conexao...
        else{
            return;
        }
    }
}
