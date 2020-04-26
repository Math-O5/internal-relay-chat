#ifndef SERVER_CONN_H
    #define SERVER_CONN_H 8080
    
    #include <stdio.h> /* sprintf */
    #include <string.h> /* memset */
    #include <stdlib.h> /* atoi */
    #include <netdb.h> /* gethostbyname */
    #include <netinet/in.h> /* struct sockaddr_in */
    #include <unistd.h> /* close */
    #include <sys/socket.h>
    #include <sys/types.h>

    /* Informacoes do Server */

    typedef struct _server_conn{
        int port;
        int sv_socket;
        int max_conn;
        struct hostent *server;
        char sserver[24], sport[24];
    }server_conn;

    /* Funcoes de Criacao/Destruicao de Server */

    server_conn criar_server();
    void destruir_server(server_conn* sv);

    /* Funcoes de Conexao do Server */

    int abrir_server(server_conn* sv, int max_conn);
    int fechar_server(server_conn* sv);

    /* Informacoes do Servidor (Server / Clients) */

    int info_server(server_conn* sv);
    void info_server_clients(server_conn* sv);

    /* Roda o Servidor */
    int run_server(server_conn* sv);

#endif