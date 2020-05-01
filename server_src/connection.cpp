#include "connection.h"

pthread_mutex_t mutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_t t[MAX_CLIENTS];
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

vector<int> clients_wait;
server_conn *SV;

int _server_socket = 0;
int cl_count = 0;

// Finalização do servidor
void conn_catch_ctrl_c_and_exit(int sig){
    clt_destruir_clientes();
    conn_destruir_server(SV);
    msg_encerrar_server();
    exit(EXIT_SUCCESS);
}

// Cria o Server sem nenhuma configuracao
server_conn conn_criar_server(){
    server_conn sv;
    sv.sv_socket = -1;
    sv.port = -1;
    sv.max_conn = 0;
    // sv.server_address = NULL;
    sprintf(sv.sport, "9002");
    return sv;
}

// Abre e configura o server
int conn_abrir_server(server_conn* sv, int max_conn){

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
    sv->server_address.sin_port = htons(sv->port);   

    /* Configura o socket para aceitar multipla conexão (most commun detault: true) */
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

    return SUCCESS;
}

// Fecha a conexao do server e atualiza o seu socket
int conn_destruir_server(server_conn* sv){
    if(sv == NULL)
      return ERRO_SOCKET;
    if(sv->sv_socket < 0)
        return ERRO_SOCKET;
    close(sv->sv_socket);

    for(int i = 0; i < MAX_CLIENTS; i++) {
        pthread_cancel(t[i]);  
    }

    return SUCCESS;
}

// Funcao que checa se o limite de clientes foi excedido
int conn_check_max_clients(server_conn* sv, int cl_count){

    if(cl_count == sv->max_conn){
        return ERRO_MAX_CLIENT_REACHED;
    }
    return SUCCESS;
}

/* Thread do cliente */
void* conn_client_thread(void *args){
    while(true) {
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

        /* Destroi o cliente (fecha a conexao e desaloca) */
        clt_destruir(id_client);

        /* Mensagem de desconexao do cliente */
        msg_cliente_desconexao(id_client);

        /* Diminui a quantidade de clientes no servidor */
        cl_count--;
    }
    return NULL;
}

// Inicializa as threads pooolings dos clientes
void conn_client_pooling(){
    for(int i = 0; i < MAX_CLIENTS; i++) {
        pthread_create(&t[i], NULL, conn_client_thread, NULL);
    } 
}

// Roda o servidor
int conn_run_server(server_conn* sv){
    SV = sv;
    /* Mensagem com as informacoes do servidor */
    msg_info_server(sv->port, sv->sv_socket, sv->max_conn, cl_count);

    /* ID inicializada para os clientes e armazenamento do socket do server */
    int id = ID_INITIALAZED;
    _server_socket = sv->sv_socket;

    // Inicializa as threads dos clientes
    conn_client_pooling();

    /* Aguarda a conexao com cliente */
    struct sockaddr_in cl_addr;
    socklen_t cl_len = sizeof(cl_addr);
    
    while(true){

        // TODO mandar mensagem de erro para clients, aceitar um a mais
		int cl_conn = accept(sv->sv_socket, (struct sockaddr*)&(cl_addr), &cl_len);

        // erro na conexão
        if(cl_conn < 0) {
            continue;
        }
        
        /* Verifica se o numero maximo de clientes no server foi atingido */
        if(conn_check_max_clients(sv, cl_count)){
            close(cl_conn);
            msg_max_client(cl_addr);
            continue;
        }

        /* Cliente Conectado (aceito !) */
        msg_cliente_conexao();

        /* Adiciona um novo cliente (atualizando atributos) */
        client* cl = clt_criar(cl_addr, cl_conn, id, sv->sv_socket);
        clients_wait.push_back(id++);
        clt_add_queue(cl, MAX_CLIENTS, &mutex);

        /* Liberar uma thread para o cliente */
        pthread_cond_signal(&condition_var);
    }

    return FAIL;
}