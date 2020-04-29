#ifndef SERVER_CONN_H
    #define SERVER_CONN_H 8080

    #define MAX_CLIENTS 30

    #define ERRO_MAX_CLIENTS 1
    #define ERRO_SOCKET 2
    #define ERRO_MULTIPLE_CONNECTION 3
    #define ERRO_BIND 4
    #define ERRO_LISTEN 5
    #define ERRO_PORT 6

    #define ERRO_MAX_CLIENT_REACHED 7

    #define ID_INITIALAZED 10
    
    #include <stdio.h> /* sprintf */
    #include <string.h> /* memset */
    #include <stdlib.h> /* atoi */
    #include <netdb.h> /* gethostbyname */
    #include <netinet/in.h> /* struct sockaddr_in */
    #include <unistd.h> /* close */
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <pthread.h> /* pthread_t */
    #include <iostream>

    #include "clients.h"
    #include "message.h"

    using namespace std;

/**
 * Informacoes do Server 
 */
    typedef struct _server_conn{
        // Dados de conexão
        int     port;
        int     sv_socket;
        int     max_conn;
        char    sport[24];
        struct sockaddr_in server_address;

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
    void catch_ctrl_c_and_exit(int sig);
    // int fechar_server(server_conn* sv);

/**
 * Threads dos clientes 
 */
    // Inicializa as threads dos clientes
    void client_pooling();

    // Verifica se alguma threads foi finalizada para inicia-la novamente (alem de atualizar seu status)
    void atualizar_threads();

/**
 * Roda o Servidor 
 */
    // Funcao que checa se o limite de clientes foi excedido
    int check_max_clients(server_conn* sv, int cl_count);

    // Thread do cliente
    void* client_thread(void *args);

    // Função deixa o struct sv pronto para receber conexão. 
    int run_server(server_conn* sv);


#endif