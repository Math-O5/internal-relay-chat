#ifndef RELAY_CHAT_H
    #define RELAY_CHAT_H 1337

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>     /* struct sockaddr_in, */
    #include <netdb.h>          /* gethostbyname(), */
    #include <unistd.h>         /* close */

    /**
     * chat.h
     * 
     * Responsável por conhecer o protocolo e realizar as trocas de
     * mensagens do aplicativo client com o servidor.
     */

        #define CONNECTION_CLOSED -1
        #define CONNECTION_OPEN    0

        typedef struct _relay_chat {
            // Dados de conexão
            char            sserver[24], sport[24];
            struct hostent  *server;
            int             port;
            int             network_socket;
            int             connection_status;
        } relay_chat;

    /**
     * Funções de estância
     */
        // Função para inicializar o struct
        relay_chat criar_relay_chat();

        // Função para destruir o struct
        void destruir_relay_chat(relay_chat* rc);

    /**
     * Funções de Conexão
     */
        // Abre uma conexão baseada nos dados definidso em shost e sport
        int abrir_conexao(relay_chat* rc);

        // Fecha a conexão e muda o status corretamente.
        int fechar_conexao(relay_chat* rc);

    /**
     * 
     */






#endif