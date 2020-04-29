#include "connection.h"

pthread_mutex_t mutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

vector<int> clients_wait;
server_conn *SV;

int _server_socket = 0;
int cl_count = 0;

pthread_t t[MAX_CLIENTS];
int t_status[MAX_CLIENTS];

void catch_ctrl_c_and_exit(int sig){
    clt_destruir_clientes();
    conn_destruir_server(SV);

    //TODO message out msg_encerrar_server();
    exit(EXIT_SUCCESS);
}

// Cria o Server sem nenhuma configuracao
server_conn criar_server(){
    server_conn sv;
    sv.sv_socket = -1;
    sv.port = -1;
    sv.max_conn = 0;
    // sv.server_address = NULL;
    sprintf(sv.sport, "9002");
    return sv;
}

void destruir_server(server_conn* sv){
    /* TODO */
}

// Abre e configura o server
int abrir_server(server_conn* sv, int max_conn){

    if(max_conn > MAX_CLIENTS) {
        return ERRO_MAX_CLIENTS;
    }

    // Buscando dados do servidor
    sv->port = atoi(sv->sport);
    sv->max_conn = max_conn;

    // Cria o socket do servidor
    sv->sv_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(sv->sv_socket < 0)
        return ERRO_SOCKET;

    // Definicao do endereco da conexao com o servidor
    memset(&(sv->server_address), 0, sizeof(sv->server_address));
    sv->server_address.sin_family = AF_INET;
    sv->server_address.sin_addr.s_addr = INADDR_ANY;   
    sv->server_address.sin_port = htons( sv->port );   

    /* enable multiple connections (options) */
    int option = 1;
    if(setsockopt(sv->sv_socket, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
    	return ERRO_MULTIPLE_CONNECTION;
	}

    /* Bind */
    if(bind(sv->sv_socket, (struct sockaddr*)&(sv->server_address), sizeof(sv->server_address)) < 0){
        return ERRO_BIND;
    }

    /* Listen */
    sv->max_conn = max_conn;
    if(listen(sv->sv_socket, max_conn) < 0){
        return ERRO_LISTEN;
    }

    /* Inicia os clients connectados */
    // sv->cl = (client**) malloc(max_conn * sizeof(client*));
    // for (int index = 0; index < MAX_CLIENTS; index++)   
    // {   
    //     sv->client_socket[index] = 0;  
    // }   

    return 0;
}

// Fecha a conexao do server e atualiza o seu socket
int conn_destruir_server(server_conn* sv){
    if(sv == NULL)
      return ERRO_SOCKET;
    if(sv->sv_socket < 0)
        return ERRO_SOCKET;
    close(sv->sv_socket);
    sv->sv_socket = -1;
    return 0;
}

// Funcao que checa se o limite de clientes foi excedido
int check_max_clients(server_conn* sv, int cl_count){

    if((cl_count + 1) >= sv->max_conn){
        return ERRO_MAX_CLIENT_REACHED;
    }

    return 0;
}

/* Thread do cliente */
void* client_thread(void *args){

    pthread_mutex_lock(&mutex);
    
    /* Aguarda a conexao com um cliente */
    while(clients_wait.empty())
        pthread_cond_wait(&condition_var, &mutex);

    /* Id do novo cliente */
    int id_client = clients_wait.back();
    clients_wait.pop_back();

    /* Aumenta a quantidade de clientes no servidor */
    cl_count++;
    
    pthread_mutex_unlock(&mutex);

    /* Fica rodando o client */
    clt_run(_server_socket, id_client, MAX_CLIENTS, &mutex);

    /* Atualizacao do status do cliente (removido) */

    // client* temp = get_client_by_id(cl_arr, id_client, MAX_CLIENTS);
    // remove_queue_client(cl_arr, id_client, MAX_CLIENTS, &mutex);
    // if(temp){
    //     destruir_cliente(temp);
    // }

    /* Mensagem de desconexao do cliente */
    msg_cliente_desconexao(id_client);

    /* Diminui a quantidade de clientes no servidor */
    cl_count--;

    /* Novo status da thread(finalizada)... */
    int* status = (int*) args;
    *status = pthread_detach(pthread_self());

    return NULL;
}

// Inicializa as threads pooolings dos clientes
void client_pooling(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        t_status[i] = 1;
        pthread_create(&t[i], NULL, client_thread, &t_status[i]);
    }
}

// Verifica se alguma threads foi finalizada para inicia-la novamente (alem de atualizar seu status)
void atualizar_threads(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(t_status[i] == 0){
            pthread_create(&t[i], NULL, client_thread, &t_status[i]);
            t_status[i] = 1;
        }
    }
}


// Roda o servidor
int run_server(server_conn* sv){
    SV = sv;
    /* Mensagem com as informacoes do servidor */
    msg_info_server(sv->port, sv->sv_socket, sv->max_conn, cl_count);

    /* ID inicializada para os clientes e armazenamento do socket do server */
    int id = ID_INITIALAZED;
    _server_socket = sv->sv_socket;

    // Inicializa as threads dos clientes
    client_pooling();

    while(true){

        /* Aguarda a conexao com cliente */
        struct sockaddr_in cl_addr;
        socklen_t cl_len = sizeof(cl_addr);
		int cl_conn = accept(sv->sv_socket, (struct sockaddr*)&(cl_addr), &cl_len);

        /* Cliente Conectado (a ser verificado...) */

        /* Verifica se o numero maximo de clientes no server foi atingido */
        if(check_max_clients(sv, cl_count)){
            close(cl_conn);
            msg_max_client(cl_addr);
            continue;
        }

        /* Cliente Conectado (aceito !) */
        msg_cliente_conexao();

        /* Verifica se alguma threads foi finalizada para inicia-la novamente (alem de atualizar seu status) */
        atualizar_threads();

        /* Adiciona um novo cliente (atualizando atributos) */
        client* cl = clt_criar(cl_addr, cl_conn, id, sv->sv_socket);
        clients_wait.push_back(id++);
        clt_add_queue(cl, MAX_CLIENTS, &mutex);

        /* Liberar uma thread para o cliente */
        pthread_cond_signal(&condition_var);
    }
    

    return 1;
}