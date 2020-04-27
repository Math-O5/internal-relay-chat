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

    // Buscando dados do servidor
    sv->port = atoi(sv->sport);
    sv->max_conn = max_conn;
    sv->server = gethostbyname(sv->sserver);
    if(sv->server == NULL)
        return 1;

    // Cria o socket do servidor
    sv->sv_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(sv->sv_socket < 0)
        return 2;

    // Definicao do endereco da conexao com o servidor
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( sv->server );   

    /* enable multiple connections */
    int option = 1;
    if(setsockopt(sv->sv_socket, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
    	return 3;
	}

    /* Bind */
    if(bind(sv->sv_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        return 4;
    }

    /* Listen */
    sv->max_conn = max_conn;
    if(listen(sv->sv_socket, max_conn) < 0){
        return 5;
    }

    printf("=== CHATROOM IS OPEN !!! ===\n");

    return 0;
}

// Fecha a conexao do server e atualiza o seu socket
int fechar_server(server_conn* sv){
    if(sv->sv_socket < 0)
        return 1;
    close(sv->sv_socket);
    sv->sv_socket = -1;
    return 0;
}

// Informacoes do servidor
int info_server(server_conn* sv){

    if(sv == NULL)
        return 1;

    if(sv->port < 0)
        return 2;

    printf("[+] IP: %s\n", sv->sserver);
    printf("[+] Listener on port %d\n", sv->port);
    printf("[+] Socket Server: %d\n", sv->sv_socket);
    printf("[+] Max Connections: %d\n", sv->max_conn);   
    
    return 0;
}

// Roda o servidor
int run_server(server_conn* sv){

    while(1){
        
    }
}