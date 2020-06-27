#ifndef CHANNEL_CONN_H
#define CHANNEL_CONN_H 8080

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
     * Id do primeiro cliente do CHANNEL. Após o estabelecimento da conexão,
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
     * typedef struct _CHANNEL_conn CHANNEL_conn
     * ---------------------------------------
     * 
     * O canal recebe a mensagem do cliente e envia para os membros do canal.
     * 
     * atributos do canal
     * --------------------
     * - participants: porta em que está conectado o servidor.
     * - size_channel: socket(valor) utilizado para abrir o servidor.
     * - name_channel: número máximo de conexões permitidas do servidor, a qual foi exigida
     *   por quem iniciou o servidor.
     * 
     * buffers de conexão
     * ------------------
     */
        typedef struct _CHANNEL_conn{

            // atributos do canal
            char* name_channel;
            int size_channel;
            client* participants;
            
            // buffers de conexão
            char *send_buff, *recv_buff;
            int  send_buff_size, recv_buff_size;
        }CHANNEL_conn;

    
    /**
     * @function
     * CHANNEL_conn conn_criar_CHANNEL()
     * -------------------------------
     * 
     * Funcao responsavel por criar um canal e retorná-lo
     * São inicialiazadas os atributos do Canal
     * 
     * return: CHANNEL_conn com atributos inicializados
    */
    CHANNEL_conn conn_criar_CHANNEL();

    CHANNEL_add_user();
    CHANNEL_remove_user();
    CHANNEL_broadcast(client from_client);
    CHANNEL_commands();
#endif