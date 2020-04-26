#include "chat.h"
#include <iostream>
#include <stdlib.h> /* atoi, */
#include <string.h> /* memset, */

// Função para inicializar o struct
relay_chat criar_relay_chat(){
    relay_chat rc;
    sprintf(rc.sserver, "127.0.0.1");
    sprintf(rc.sport, "9002");
    rc.server = NULL;
    rc.port   = -1;
    rc.network_socket = -1;
    rc.connection_status = CONNECTION_CLOSED;
    return rc;
}

// Função para destruir o struct
void destruir_relay_chat(relay_chat* rc){
    // @TODO: Verificar o que precisa ser desalocado e implementar
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
    
    return 0;
}

// Fecha a conexão e muda o status corretamente.
int fechar_conexao(relay_chat* rc){
    if(rc->connection_status == CONNECTION_CLOSED || rc->network_socket < 0)
        return 1;
    close(rc->network_socket);
    rc->connection_status = CONNECTION_CLOSED;
    rc->network_socket    = -1;
    return 0;
}