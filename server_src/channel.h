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
        #include <map>
        #include <queue>
        #include <set>

        using namespace std;

    /**
     * Bibliotecas do própio projeto
     * -----------------------------
    */
        #include "clients.h"
        #include "message.h"
        #include "decodec.h"

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
    #define ERROR_CREATE_CHANNEL 8
    #define ERROR_USER_BANNED 9
    #define ERROR_INVITEONLYCHAN 10
    #define SUCCESS 0
    #define FAIL 1
    
    #define ID_INITIALAZED 10
    
    #define MAX_SIZE_NAME 30
    #define MAX_CLIENTS 20

    using namespace std;

    /**
     * Compara duas strings char*
     */
    struct cmp_str
    {
        bool operator()(char const *str1, char const *str2) const
        {
            return strcmp(str1, str2) < 0;
        }
    };

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
    typedef struct _CHANNEL_conn
    {
        // atributos do canal
        char* nickname_channel;
        char* nickname_admin;
        bool is_public;
        int size_participants;
        map<const char*, int, cmp_str> participants;                    // nickname, cl_socket 
        map<int, char*> arrived;                         // position arrived, nickname

        set<const char*, cmp_str>   notAllowedParticipants,              // <nickname> dos clientes banidos.
                                    mutedParticipants,                   // <nickname> dos clientes banidos.
                                    invitesParticipants;
        // buffers de conexão
        char *send_buff, *recv_buff;
        int  send_buff_size, recv_buff_size;
    } CHANNEL_conn;


    enum MESSAGE {
        MESSAGE_CLIENT, MESSAGE_NEW_JOIN_CHANNEL, MESSAGE_JOIN_CHANNEL, MESSAGE_NEW_ADMIN_CHANNEL,
        MESSAGE_TOPIC, MESSAGE_QUIT_CHANNEL, MESSAGE_ERR_INVITEONLYCHAN, MESSAGE_ERR_BANNEDFROMCHAN, 
        MESSAGE_KICK_CHANNEL, MESSAGE_UNKICK_CHANNEL, MESSAGE_KICK_ERR_NOSUCHNICK, MESSAGE_KICK_ERR_CHANOPRIVSNEEDED,
        MESSAGE_MUTE_CHANNEL, MESSAGE_UNMUTE_CHANNEL, MESSAGE_MUTE_ERR_NOSUCHNICK, MESSAGE_MUTE_ERR_CHANOPRIVSNEEDED,
        MESSAGE_UNMUTE_ERR_NOSUCHNICK, MESSAGE_UNMUTE_ERR_CHANOPRIVSNEEDED, MESSAGE_UNKICK_ERR_NOSUCHNICK,MESSAGE_CLIENT_MUTED_CHANNEL,
        MESSAGE_UNKICK_ERR_CHANOPRIVSNEEDED, MESSAGE_WHOIS_ERR_NOSUCHNICK, MESSAGE_WHOIS_CHANNEL, MESSAGE_WHOIS_ERR_CHANOPRIVSNEEDED, MESSAGE_INVITED,
        MESSAGE_MODE_CHANNEL, MESSAGE_MODE_ERR_CHANOPRIVSNEEDED, MESSAGE_CHANGE_NICKNAME, MESSAGE_INVITE_CHANNEL,
        MESSAGE_INVITE_ERR_NOSUCHNICK, MESSAGE_INVITE_ERR_CHANOPRIVSNEEDED
    };
    /**
     * @function
     * CHANNEL_conn conn_criar_CHANNEL(int cl_socket)
     * 
     * cl_socket: o id socket cliente
     * -------------------------------
     * 
     * Funcao responsavel por criar um canal e retorná-lo
     * São inicialiazadas os atributos do Canal
     * 
     * return: CHANNEL_conn com atributos inicializados
    */
   
    CHANNEL_conn* conn_criar_CHANNEL(char* name_channel, struct _client* clt);
    void CHANNEL_destroy(CHANNEL_conn* channel);
    void CHANNEL_destroy_all();
    void CHANNEL_list(char* buffer); 
    void CHANNEL_list_participants(CHANNEL_conn* channel, char* buffer);
    void CHANNEL_msg(CHANNEL_conn* channel, struct _client* clt, char* buffer);
    bool CHANNEL_check_privilege(CHANNEL_conn* channel, struct _client* clt);
    int CHANNEL_add_user(CHANNEL_conn* channel,  struct _client* clt); 
    int CHANNEL_remove_user(CHANNEL_conn* channel,  struct _client* clt);
    void CHANNEL_on_change_nickname(struct _client* clt, const char* old_nickname);
    void CHANNEL_join(char* name_channel,  struct _client* clt); 
    void CHANNEL_kick_user(CHANNEL_conn* channel, struct _client* clt, const char* kick_nickname); 
    void CHANNEL_unkick_user(CHANNEL_conn* channel, struct _client* clt, const char* unkick_nickname); 
    bool CHANNEL_send_message(int cl_socket, const char* buffer); 
    void CHANNEL_send_message_all(CHANNEL_conn* channel, const char* buffer);
    void CHANNEL_send_message_one(CHANNEL_conn* channel,  struct _client* clt,const char* buffer);
    int CHANNEL_broadcast(CHANNEL_conn* channel, struct _client* clt, int type, const char* buffer); 
    void CHANNEL_mute_user(CHANNEL_conn* channel, struct _client* clt, const char* mute_nickname);
    void CHANNEL_unmute_user(CHANNEL_conn* channel, struct _client* clt, const char* unmute_nickname);
    void CHANNEL_invite(CHANNEL_conn* channel, struct _client* clt, const char* kick_nickname);
    void CHANNEL_whois(CHANNEL_conn* channel, struct _client* clt, char* whois);
    void CHANNEL_mode(CHANNEL_conn* channel, struct _client* clt, bool is_public); 

#endif