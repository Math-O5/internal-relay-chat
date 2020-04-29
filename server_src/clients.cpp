#include "clients.h"
#include "message.h"

/* Cria um novo cliente */
client* criar_cliente(struct sockaddr_in address, int socket, int id, int sv_socket){
    client* cl = (client*) malloc(sizeof(client));
    cl->cl_address = address;
    cl->cl_socket = socket;
    cl->cl_id = id;
    cl->sv_socket = sv_socket;
    return cl;
}

/* Destroi o cliente */
void destruir_cliente(client* cl){
    free(cl);
}

/* Adiciona um cliente na queue de clientes */
int add_queue_client(client** cl_arr, client* cl, int max_cl, pthread_mutex_t* mutex){

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

/* Remove um cliente da queue de clientes*/
client* remove_queue_client(client** cl_arr, int id, int max_cl, pthread_mutex_t* mutex){
    
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
client* get_client_by_id(client** cl_arr, int id, int max_clients){

    for(int i = 0; i < max_clients; i++){
        if(cl_arr[i] && cl_arr[i]->cl_id == id){
            return cl_arr[i];
        }
    }

    return NULL;
}

/* Envia a mensagem para todos os clientes */
void send_message(client** cl_arr, int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer){

    /* Configuracao da mensagem a ser enviada (adiciona que enviou a mensagem no conteudo) */
    char msg_buffer[BUFFER_SIZE];
    sprintf(msg_buffer, ">> <CLIENTE %d>: %s", id_cur, buffer);

    pthread_mutex_lock(mutex);

    /* Envio da mensagem aos clientes */
    for(int i = 0; i < max_conn; i++){
        if(cl_arr[i] && cl_arr[i]->cl_id != id_cur){
            send(cl_arr[i]->cl_socket, msg_buffer, strlen(msg_buffer) - 1, 0);
            msg_send_cliente(id_cur, cl_arr[i]->cl_id);
        }
    }

    pthread_mutex_unlock(mutex);
}

/* Gerencia o cliente */
void run_client(client** cl_arr, int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex){

    /* Buffer com as mensagens dos clientes */    
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    /* Recupera as informacoes do cliente... */
    client* cl = get_client_by_id(cl_arr, id_cur, max_conn);

    /* Mensagem com as informacoes do cliente */
    msg_info_client(id_cur, cl->cl_socket, cl->cl_address);

    while(true){

        /* Mensagem recebida ! */
        if(recv(cl->cl_socket, buffer, BUFFER_SIZE, 0) > 0){
            msg_recv_cliente(id_cur, buffer);
            send_message(cl_arr, id_cur, max_conn, mutex, buffer);
            bzero(buffer, BUFFER_SIZE);
        }
        /* Ocorreu um erro na conexao... */
        else{
            break;
        }
    }
}
