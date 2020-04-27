#ifndef RELAY_CHAT_H
    #define RELAY_CHAT_H 1337

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>     /* struct sockaddr_in, */
    #include <netdb.h>          /* gethostbyname(), */
    #include <unistd.h>         /* close, sleep */
    #include <pthread.h>        /* pthread_create, pthread_mutex_t */

    /**
     * @ chat.h
     * 
     * Responsável por conhecer o protocolo e realizar as trocas de
     * mensagens do aplicativo client com o servidor.
     */

        #define CONNECTION_CLOSED -1
        #define CONNECTION_OPEN    0

        #define MAX_MESSAGE_LENGHT 4096
        #define BUFFER_SIZE ((MAX_MESSAGE_LENGHT * 30) + 1)


        typedef struct _relay_chat {
            // Dados de conexão
            char            sserver[24], sport[24];
            struct hostent  *server;
            int             port;
            int             network_socket;
            int             connection_status;

            // Buffers de conexão
            char *send_buff, *recv_buff;
            int  send_buff_size, recv_buff_size;

            // Mutexes
            pthread_mutex_t* send_mutex;
            pthread_mutex_t* recv_mutex;
        } relay_chat;

    /**
     * Funções de estância
     */
        // Função para inicializar o struct
        relay_chat criar_relay_chat(pthread_mutex_t* send_mutex, pthread_mutex_t* recv_mutex);

        // Função para destruir o struct
        int destruir_relay_chat(relay_chat* rc);

    /**
     * Funções de Conexão
     */
        // Abre uma conexão baseada nos dados definidso em shost e sport
        int abrir_conexao(relay_chat* rc);

        // Fecha a conexão e muda o status corretamente.
        int fechar_conexao(relay_chat* rc);

    /**
     * Threads de Trocas de dados
     */ 
        // Verifica o buffer do socket e salva no buffer de input
        void* recv_msg_handler(void* rc);

        // Verifica o buffer de envio e escreve no socket
        void* send_msg_handler(void* rc);





#endif