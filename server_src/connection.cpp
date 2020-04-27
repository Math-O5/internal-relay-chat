#include "connection.h"

// Cria o Server sem nenhuma configuracao
server_conn criar_server(){
    server_conn sv;
    sv.sv_socket = -1;
    sv.port = -1;
    sv.max_conn = 0;
    sprintf(sv.sport, "9002");
    return sv;
}

void destruir_server(server_conn* sv){
    /* TODO */
}

// Abre e configura o server
int abrir_server(server_conn* sv, int max_conn){

    if(max_conn > MAX_CLIENTS) {
        return ERRO_MAX_CLIENTS
    }

    // Buscando dados do servidor
    sv->port = atoi(sv->sport);
    sv->max_conn = max_conn;

    // Cria o socket do servidor
    sv->sv_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(sv->sv_socket < 0)
        return ERRO_SOCKET;

    // Definicao do endereco da conexao com o servidor
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;   
    server_address.sin_port = htons( sv->port );   

    /* enable multiple connections (options) */
    int option = 1;
    if(setsockopt(sv->sv_socket, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
    	return ERRO_MULTIPLE_CONNECTION;
	}

    /* Bind */
    if(bind(sv->sv_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        return ERRO_BIND;
    }

    /* Listen */
    sv->max_conn = max_conn;
    if(listen(sv->sv_socket, max_conn) < 0){
        return ERRO_LISTEN;
    }

    /* Inicia os clients connectados */
    for (index = 0; index < MAX_CLIENTS; ++index)   
    {   
        sv->client_socket[index] = 0;   
    }   

    return 0;
}

// Fecha a conexao do server e atualiza o seu socket
int fechar_server(server_conn* sv){
    if(sv == NULL)
      return ERRO_SOCKET;

    if(sv->sv_socket < 0)
        return ERRO_SOCKET;
    close(sv->sv_socket);
    sv->sv_socket = -1;
    return 0;
}

// Informacoes do servidor
int info_server(server_conn* sv){

    if(sv == NULL)
        return ERRO_SOCKET;

    if(sv->port < 0)
        return ERRO_PORT;

    printf("[+] Listener on port %d\n", sv->port);
    printf("[+] Socket Server: %d\n", sv->sv_socket);
    printf("[+] Max Connections: %d\n", sv->max_conn);   
    
    return 0;
}

// Roda o servidor
int run_server(server_conn* sv){

    while(true){
       info_server(sv);
        p_thread_t t;
        pthread_create(&t, NULL, &handle_connecton, pclient); 
    }
}