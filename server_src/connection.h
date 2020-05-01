#ifndef SERVER_CONN_H
    #define SERVER_CONN_H 8080

    #define MAX_CLIENTS 3

    #define ERRO_MAX_CLIENTS 1
    #define ERRO_SOCKET 2
    #define ERRO_MULTIPLE_CONNECTION 3
    #define ERRO_BIND 4
    #define ERRO_LISTEN 5
    #define ERRO_PORT 6

    #define ERRO_MAX_CLIENT_REACHED 7

    #define SUCCESS 0
    #define FAIL 1

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
    server_conn conn_criar_server();

    /**
     *  @Funcoes de Conexao do Server 
     */
    // Função para inicializar o struct server
    int conn_abrir_server(server_conn* sv, int max_conn);
    
    // Função que chama todas funções necessarias para finalizar o programa com sucesso.
    void conn_catch_ctrl_c_and_exit(int sig);
    
    // Função Finaliza thread ativas e limpa memória alocada pelo servidor.
    int conn_destruir_server(server_conn* sv);

    /**
     * @Funcao
     * Inicializa todas as threads do servidor com a thread client_thread (void* client_thread(void* args)). 
     * Cada thread atende um cliente.
     */
    void conn_client_pooling();

     /**
     * @Funcao
     * Funcao que checa se o limite de clientes foi excedido
     * 
     * return: 
     *          ERRO_MAX_CLIENT_REACHED     : número de clients no limite;
     *          0                           : número de clients abaixo do limite;
     */
    int conn_check_max_clients(server_conn* sv, int cl_count);

     /**
     * @Thread
     * Thread que executa umm cliente. Primeiro checa se há cliente para atender na fila de 
     * clientes (clients_wait). Se está estiver vazia, a thread fica em WAIT.
     * Quando clientes são atribuidos a fila, um sinal é enviado para WAKE UP a thread, ela
     * retira o cliente da fila e fica excutando ele em clt_run(). Quando clt_run() retorna
     * ela atende outro cliente.
     * 
     * return: 
     *          NULL     : em caso de erro inesperado;
     */
    void* conn_client_thread(void *args);

    /**
     * @Funcao
     * Funcao aceita conexões com clientes. Ela inicializa as threads e adiciona o cliente a fila quando
     * ele tenta uma conexão. O número de clientes será MAX_CLIENTS - 1, pois o ultimo socket
     * está reservado com responder aos outros clients que o servidor está em máxima capidade no momento.
     * 
     * return: 1 interrompido por algum sinal ou erro inesperado.
     *          
     */
    int conn_run_server(server_conn* sv);


#endif