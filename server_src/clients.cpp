#include "clients.h"
#include "message.h"

client *cl_arr[MAX_CLIENTS];

/* Cria um novo cliente */
client* clt_criar(struct sockaddr_in address, int socket, int id, int sv_socket){
    client* cl = (client*) malloc(sizeof(client));
    cl->cl_address = address;
    cl->cl_socket = socket;
    cl->cl_id = id;
    cl->sv_socket = sv_socket;
    return cl;
}


/* Adiciona um cliente na queue de clientes */
int clt_add_queue(client* cl, int max_cl, pthread_mutex_t* mutex){

    pthread_mutex_lock(mutex);

    /* Adiciona o cliente no primeiro espaco disponivel */
    int index;
    for(index = 0; index < max_cl; index++){
        if(!cl_arr[index]){
            cl_arr[index] = cl;
            break;
        }
    }

    pthread_mutex_unlock(mutex);

    /* Numero maximo de clientes ja foi atingido... */
    if(index == max_cl)
        return 1;

    return 0;
}

/* Destroi o cliente */
void clt_destruir(int id){
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i]->cl_id == id) {
            close(cl_arr[i]->cl_socket);
            free(&(*cl_arr[i]));
            break;
        }
    }
}

void clt_destruir_clientes() {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(cl_arr[i] != NULL) {
            close(cl_arr[i]->cl_socket);
            free(&(*cl_arr[i]));
        }
    }
}

/* Remove um cliente da queue de clientes*/
client* clt_remove_queue(int id, int max_cl, pthread_mutex_t* mutex){
    
    pthread_mutex_lock(mutex);  

    /* Procura pelo cliente, atraves de seu id, e o retira do array */
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

/* Recuperar o cliente pelo id */
client* clt_get_by_id(int id, int max_clients){

    for(int i = 0; i < max_clients; i++){
        if(cl_arr[i] && cl_arr[i]->cl_id == id){
            return cl_arr[i];
        }
    }
    return NULL;
}

// Envia mensagem para cl_socket, se falhar tenta mais 5 vezes.
// Returna true se conseguiu enviar a mensagem.
bool clt_send_message(int cl_socket, char* buffer) {
    int try_send = 0;

    // Tenta enviar a mensagem até receber confirmação. Se após 
    while(send(cl_socket, buffer, strlen(buffer), 0) < 0 && try_send < 6) {
        try_send += 1;
    }
    return (try_send == 6) ? false : true;
}

/* Envia a mensagem para todos os clientes */
void clt_send_message_all(int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer){

    /* Configuracao da mensagem a ser enviada (adiciona que enviou a mensagem no conteudo) */
    char msg_buffer[BUFFER_SIZE];
    sprintf(msg_buffer, ">> <CLIENTE %d>: %s\n", id_cur, buffer);

    pthread_mutex_lock(mutex);

    /* Envio da mensagem aos clientes */
    for(int i = 0; i < max_conn; i++){
        if(cl_arr[i]){
            if(clt_send_message(cl_arr[i]->cl_socket, msg_buffer)) {
                msg_send_cliente(id_cur, cl_arr[i]->cl_id);
            } else {
                msg_client_no_response(cl_arr[i]->cl_id);
                close(cl_arr[i]->cl_socket);
            } 
        }
    }

    pthread_mutex_unlock(mutex);
}

/* Read commands of user */ 
bool clt_read_msg(client* cl, char* buffer) {
  
    if(strncmp(buffer, "/ping", 5) == 0) {

        msg_info_ping(cl->cl_id);
        memset(buffer, '\0', BUFFER_SIZE);
        strcpy(buffer, ">> <SERVER>: pong\n\0");
        
        if(clt_send_message(cl->cl_socket, buffer)) {
            msg_info_pong(cl->cl_id);
        } 
        memset(buffer, '\0', BUFFER_SIZE);
        return false;

    } else if(strncmp(buffer, "/quit", 5) == 0) {
        msg_cliente_desconexao(cl->cl_id);
        return false;
    } 

    return true;
}

/* Gerencia o cliente */
void clt_run(int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex){

    /* Buffer com as mensagens dos clientes */    
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    /* Recupera as informacoes do cliente... */
    client* cl = clt_get_by_id(id_cur, max_conn);

    // No clients avaible
    // if(cl == NULL) {
    //     return;
    // }

    /* Mensagem com as informacoes do cliente */
    msg_info_client(id_cur, cl->cl_socket, cl->cl_address);

    while(true){

        /* Mensagem recebida ! */
        if(recv(cl->cl_socket, buffer, BUFFER_SIZE, 0) > 0){
            if(clt_read_msg(cl, buffer)) {
                msg_recv_cliente(id_cur, buffer);
                clt_send_message_all(id_cur, max_conn, mutex, buffer);
                bzero(buffer, BUFFER_SIZE);
            }
        }
        /* Ocorreu um erro na conexao... */
        else{
            break;
        }
    }
}
