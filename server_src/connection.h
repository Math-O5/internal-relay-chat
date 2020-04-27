#ifndef SERVER_CONN_H
    #define SERVER_CONN_H 8080
    #define MAX_CLIENTS 30

    #define ERRO_MAX_CLIENTS 1
    #define ERRO_SOCKET 2
    #define ERRO_MULTIPLE_CONNECTION 3
    #define ERRO_BIND 4
    #define ERRO_LISTEN 5
    #define ERRO_PORT 7
    
    #include <stdio.h> /* sprintf */
    #include <string.h> /* memset */
    #include <stdlib.h> /* atoi */
    #include <netdb.h> /* gethostbyname */
    #include <netinet/in.h> /* struct sockaddr_in */
    #include <unistd.h> /* close */
    #include <sys/socket.h>
    #include <sys/types.h>

/**
 * Informacoes do Server 
 */
    typedef struct _server_conn{
        // Dados de conexão
        int     port;
        int     sv_socket;
        int     max_conn;
        int     client_socket[30];   
        char    sport[24];

        // Buffers de conexão
        char *send_buff, *recv_buff;
        int  send_buff_size, recv_buff_size;
    }server_conn;

/**
 *  Funções de estância
 */
    //Funcoes de Criacao
    server_conn criar_server();

    //Destruicao de Server
    void destruir_server(server_conn* sv);

/**
 *  Funcoes de Conexao do Server 
 */
    // Função para inicializar o struct server
    int abrir_server(server_conn* sv, int max_conn);
    
    // Função para destruir o struct server
    int fechar_server(server_conn* sv);

/**
 *  Informacoes do Servidor (Server / Clients) 
 */

    // Checar o STATUS de conexão do struct server
    int info_server(server_conn* sv);

    // Checar o STATUS de conexão com os struct dos clients 
    void info_server_clients(server_conn* sv);

/**
 * Roda o Servidor 
 */
    // Função deixa o struct sv pronto para receber conexão. 
    int run_server(server_conn* sv);

#endif