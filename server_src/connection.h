#ifndef SERVER_CONN_H
    #define SERVER_CONN_H 8080

    /**
     * Bibliotecas importadas
     * ----------------------
     * 
     * Devido ao uso da biblioteca "pthread.h", é necessário que o código
     * seja compilado com o parâmetro -pthread junto ao g++
    */
    
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

    /**
     * Bibliotecas do própio projeto
     * -----------------------------
    */
        #include "clients.h"
        #include "message.h"

    /**
     * Constantes e Macros
     * -------------------
     * 
     * ERROS
     * Valores que indicam o local/motivo do erro
     * Obs.: Por padrão, o valor de SUCESS será 0 e valores de erro
     * serão maiores ou iguais a 1
     * 
     * SUCCESS e FAIL
     * Valores genéricos de sucesso e erro
     * 
     * ID_INITIALAZED
     * Id do primeiro cliente do server. Após o estabelecimento da conexão,
     * com estre cliente, o valor será incrementado a cada nova conexão
     * 
     * MAX_CLIENTS
     * Número máximo de clientes aceitos no servidor
    */

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
        
        #define MAX_CLIENTS 20




        using namespace std;

    /**
     * @struct
     * typedef struct _server_conn server_conn
     * ---------------------------------------
     * 
     * Struct que administra as informações do servidor necessárias para
     * abrir e fechar o servidor, interagir com clientes e realizar a interação
     * entre clientes
     * 
     * atributos de conexão
     * --------------------
     * - port: porta em que está conectado o servidor.
     * - sv_socket: socket(valor) utilizado para abrir o servidor.
     * - max_conn: número máximo de conexões permitidas do servidor, a qual foi exigida
     *   por quem iniciou o servidor.
     * - sport[24]: string da porta do servidor.
     * - server_address: informações do endereço do servidor
     * 
     * buffers de conexão
     * ------------------
     */
        typedef struct _server_conn{
            // atributos de conexão
            int     port;
            int     sv_socket;
            int     max_conn;
            char    sport[24];
            struct sockaddr_in server_address;
            struct sockaddr_in server_address;

            // buffers de conexão
            char *send_buff, *recv_buff;
            int  send_buff_size, recv_buff_size;
        }server_conn;

    
    /**
     * @function
     * server_conn conn_criar_server()
     * -------------------------------
     * 
     * Funcao responsavel por criar um servidor e retorná-lo
     * São inicialiazadas os atributos do servidor
     * 
     * return: server_conn com atributos inicializados
    */
    server_conn conn_criar_server();


    /**
     * @function
     * 
     * int conn_abrir_server(server_conn* sv, int max_conn)
     * ------------------------------------------------
     * 
     * Configura o servidor e o incializa.  O servidor a ser configurado 
     * é o do parâmetro sv e o máximo de conexões permitidas está definida no
     * parâmetro max_conn
     * 
     * return: == 0 o servidor foi configurado e iniciado com sucesso
     *         >= 1 algum erro ocorreu
    */
    int conn_abrir_server(server_conn* sv, int max_conn);
    
    /**
     * @function
     * 
     * void conn_catch_ctrl_c_and_exit(int sig)
     * ----------------------------------------
     * 
     * Função que chama todas funções necessarias para finalizar 
     * o programa com sucesso. É fechada a conexão com todos os clientes
     * e logo após isso o servidor é fechado. Por fim, o programa é terminado
    */
    void conn_catch_ctrl_c_and_exit(int sig);
    
    /**
     * @function
     * 
     * int conn_destruir_server(server_conn* sv)
     * -----------------------------------------
     * 
     * Finaliza thread ativas e limpa memória alocada pelo servidor.
     * Por fim, fecha o servidor. 
     *
     * return:  SUCESS      o servidor foi finalizado com sucesso
     *          ERRO_SOCKET ocorreu um erro ao fechar a conexão do servidor 
    */
    int conn_destruir_server(server_conn* sv);

    /**
     * @function
     * 
     * void conn_client_pooling()
     * --------------------------
     * 
     * Inicializa todas as threads do servidor com a thread 
     * client_thread (void* client_thread(void* args)). 
     * Cada thread atende um cliente.
     */
    void conn_client_pooling();

     /**
     * @function
     * 
     * int conn_check_max_clients(server_conn* sv, int cl_count)
     * ---------------------------------------------------------
     * 
     * Checa se o limite de clientes aceitos pelo servidor já está no máximo 
     * 
     * return:  ERRO_MAX_CLIENT_REACHED número de clientes conectados já está no limite
     *          0                       número de clients abaixo do limite
     */
    int conn_check_max_clients(server_conn* sv, int cl_count);

     /**
     * @thread
     * 
     * void* conn_client_thread(void *args)
     * ------------------------------------
     * 
     * Thread que executa umm cliente. Primeiro checa se há cliente para atender na fila de 
     * clientes (clients_wait). Se está estiver vazia, a thread fica em WAIT.
     * Quando clientes são atribuidos a fila, um sinal é enviado para WAKE UP a thread, ela
     * retira o cliente da fila e fica excutando ele em clt_run(). Quando clt_run() retorna
     * ela atende outro cliente.
     * 
     * return: NULL em caso de erro inesperado
     */
    void* conn_client_thread(void *args);

    /**
     * @function
     * 
     * int conn_run_server(server_conn* sv)
     * ------------------------------------
     * 
     * Funcao aceita conexões com clientes. Ela inicializa as threads e adiciona o cliente a fila quando
     * ele tenta uma conexão. O número de clientes será MAX_CLIENTS - 1, pois o ultimo socket
     * está reservado com responder aos outros clients que o servidor está em máxima capidade no momento.
     * 
     * return: FAIL interrompido por algum sinal ou erro inesperado.
     *          
     */
    int conn_run_server(server_conn* sv);


#endif