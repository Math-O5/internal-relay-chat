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
    
    #define MAX_SIZE_NAME 50
    #define MAX_CLIENTS 20
    #define CHANNEL_NAME_MAXLENGHT 50

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
     * O canal é um area de broadcast. Um usuário envia a mensagem e todos outros no canal
     * podem vê-la.
     * O canal pode ser público ou privado, e tem um administrador.
     * O admin é o user que criou o canal, ou 
     * 
     * atributos do canal
     * --------------------
     * 
    { const char* } nickname_channel                 : é o nome deste canal.
    { char* } nickname_admin                         : é o nome do administrador.
    { bool  } bool is_public                         : indica se o canal é público ou privado.
    { map<const char*, int, cmp_str> }  participants : mapeia  <nickname_cliente, cl_socket> 
    { map<int, char*> }  arrived                     : mapeia  <position arrived, nickname_cliente>
    { set<const char*, cmp_str> } notAllowedParticipants :               // lista dos <nickname> dos clientes banidos.
                                    mutedParticipants,                   // lista dos <nickname> dos clientes silenciados.
                                    invitesParticipants;                 // lista dos <nickname> dios clientes convidados.
     */
    typedef struct _CHANNEL_conn
    {
        char* nickname_channel;
        char* nickname_admin;
        bool is_public;
        map<const char*, int, cmp_str> participants;     // nickname_cliente, cl_socket 
        map<int, char*> arrived;                         // position arrived, nickname_cliente

        set<const char*, cmp_str>   notAllowedParticipants,              // <nickname> dos clientes banidos.
                                    mutedParticipants,                   // <nickname> dos clientes silenciados.
                                    invitesParticipants;                 // <nickname> dios clientes convidados.
    } CHANNEL_conn;


    enum MESSAGE {
        MESSAGE_CLIENT, MESSAGE_NEW_JOIN_CHANNEL, MESSAGE_JOIN_CHANNEL, MESSAGE_NO_JOIN_CHANNEL, MESSAGE_NEW_ADMIN_CHANNEL,
        MESSAGE_TOPIC, MESSAGE_QUIT_CHANNEL, MESSAGE_ERR_INVITEONLYCHAN, MESSAGE_ERR_BANNEDFROMCHAN, 
        MESSAGE_KICK_CHANNEL, MESSAGE_UNKICK_CHANNEL, MESSAGE_KICK_ERR_NOSUCHNICK, MESSAGE_KICK_ERR_CHANOPRIVSNEEDED,
        MESSAGE_MUTE_CHANNEL, MESSAGE_UNMUTE_CHANNEL, MESSAGE_MUTE_ERR_NOSUCHNICK, MESSAGE_MUTE_ERR_CHANOPRIVSNEEDED,
        MESSAGE_UNMUTE_ERR_NOSUCHNICK, MESSAGE_UNMUTE_ERR_CHANOPRIVSNEEDED, MESSAGE_UNKICK_ERR_NOSUCHNICK,MESSAGE_CLIENT_MUTED_CHANNEL,
        MESSAGE_UNKICK_ERR_CHANOPRIVSNEEDED, MESSAGE_WHOIS_ERR_NOSUCHNICK, MESSAGE_WHOIS_CHANNEL, MESSAGE_WHOIS_ERR_CHANOPRIVSNEEDED, MESSAGE_INVITED,
        MESSAGE_MODE_CHANNEL, MESSAGE_MODE_ERR_CHANOPRIVSNEEDED, MESSAGE_CHANGE_NICKNAME, MESSAGE_INVITE_CHANNEL,
        MESSAGE_INVITE_ERR_NOSUCHNICK, MESSAGE_INVITE_ERR_CHANOPRIVSNEEDED
    };

    /**
     * @function conn_criar_CHANNEL
     * 
     * Cria canal com as propriedades iniciais necessarias.
     * 
     * @param { char* } nickname_channel : nome do canal em que ação acontece
     * @param { client[] } clt : client que será removido.
     * @permision: self user | admin 
     * @return { SUCESS | FAIL }
     * 
     * Atualiza a referencia do cliente para o canal.
     * 
     */
    CHANNEL_conn* conn_criar_CHANNEL(char* name_channel, struct _client* clt);

    /**
     * @function conn_destruit_CHANNEL
     * 
     * Excluir o canal (limpa a memória)
     * @param { CHANNEL_conn* } channel : referencia do canal em que ação acontece
     * @permision: admin 
     * 
     */
    void CHANNEL_destroy(CHANNEL_conn* channel);

    /**
     * @function conn_destruit_CHANNELS
     * 
     * Exclui TODOS os canais (limpa a memória).
     * 
     * Essa função é chamada pelo "connection.cpp" ao fim de execução.
     * @permision: only server 
     * 
     */
    void CHANNEL_destroy_all();

    /**
     * @function CHANNEL_list
     * 
     * Lista todos os canais e envia a resposta para o usuário pelo buffer.
     * 
     * @param { buffer* }  clt  :   cliente que está enviando o comando.
     * @permision: all users.
     * 
     * Exemplo:
        Client >> /list\n
        
        buffer[] = "/channels <name1>, <name2>, <name3>,...\n"
    * 
    */
    void CHANNEL_list(char* buffer); 

    void CHANNEL_list_participants(CHANNEL_conn* channel, char* buffer);

    /**
     * @function CHANNEL_list
     * 
     * Lista todos os canais e envia a resposta para o usuário.
     * 
     * @param { client[] }      clt      :   cliente que está enviando o comando.
     * @permision: all users.
     * 
     * Exemplo:
        Client >> /list\n
        Server << /channels <name1>, <name2>, <name3>,...\n
    * 
    */
    void CHANNEL_list(struct _client* clt);

    /**
     * @function CHANNEL_list
     * 
     * Repassa a mensagem para todo o canal, porem verifica as permissões do
     * usupario no canal. Se ele estiver "muted", ele não pode falar e receberá uma mensagem de
     * erro.
     * 
     * @param { CHANNEL_conn* } channel           :   Referencia do canal que será especionado.
     * @param { client[] }      clt               :   Cliente que está enviando o comando.
     * @param { char* }         buffer            :   Mensagem do usuário.
     * @permision: all users.   
     * 
     * Exemplo:
        Client >> /msg (#/@)<nick_name> :<message_content>\n
        Server << /msg (#/@)<nick_name> :<message_content>\n

        Falha 
        Server << /servermsg : MESSAGE_CLIENT_MUTED_CHANNEL
    * 
    */
    void CHANNEL_msg(CHANNEL_conn* channel, struct _client* clt, char* buffer);

    /**
     * @function CHANNEL_check_privilege
     * 
     * Verifica as permissões do client no channel.
     * 
     * @param { CHANNEL_conn* } channel : nome do canal em que ação acontece
     * @param { struct _client } clt    : client que será analisado.
     * @permision: all 
     * @return { SUCESS | FAIL } Se o user é admin ou não.
     * 
     */
    bool CHANNEL_check_privilege(CHANNEL_conn* channel, struct _client* clt);
    
    /**
     * @function CHANNEL_remove_user
     * 
     * Exclui o usuário do canal e o proibe de entrar novamente.
     * @param { char* } nickname_channel : nome do canal em que ação acontece
     * @param { client[] } clt : client que será removido.
     * @permision: self user | admin 
     * @return { SUCESS | FAIL }
     * 
     */
    int CHANNEL_add_user(CHANNEL_conn* channel,  struct _client* clt); 

    /**
     * @function CHANNEL_remove_user
     * 
     * Exclui o usuário do canal e o proibe de entrar novamente.
     * @param { char* } nickname_channel : nome do canal em que ação acontece
     * @param { client[] } clt : client que será removido.
     * @permision: self user | admin 
     * @return { SUCESS | FAIL }
     * 
     */
    int CHANNEL_remove_user(CHANNEL_conn* channel,  struct _client* clt);
    
    /**
     * @function CHANNEL_on_change_nickname
     * 
     * Faz as alterações necessarias ao longo do servidor quando um cliente muda de nome.
     * Se um cliente foi banido e muda o nome, o nome dele igualmente é alterado na lista de banidos.
     * Se ele é administrador de algum canal, o nome do administrador tambem muda.
     * 
     * @param { client* } clt        : é a referencia para os dados do cliente.
     * @param { const char* } old_nickname : é o nome anterior
     * @permission: CASCADE, sempre que o nome de qualquer usuário for alterado.
     */
    void CHANNEL_on_change_nickname(struct _client* clt, const char* old_nickname);

    /**
     * @function CHANNEL_join
     * 
     * Entra no canal especificado. Se o usuário não possuir permissão para entrar,
     * é enviado uma mensagem de erro.
     * 
     * @param { char* } nickname_channel :   canal em que o usuário deseja entrar.
     * @param { client[] }      clt      :   cliente que está enviando o comando.
     * @permision: all users.
     * 
     * Exemplo:
        Client >> /join <channel>\n

        Sucesso
        SERVER << /join SUCCESS <channel> <role:user/admin>\n
        SERVER << /join RPL_NAMREPLY <channel> [[#|@]<nick> [[#|@]<nick>]]\n

        Falha
        SERVER << /join ERR_BANNEDFROMCHAN\n
        SERVER << /join ERR_INVITEONLYCHAN\n
    * 
    */
    void CHANNEL_join(char* name_channel,  struct _client* clt); 

    /**
     * @function CHANNEL_kick_user
     * 
     * Exclue o usuário do canal e o proibe de entrar novamente.
     * 
     * @param { CHANNEL_conn* } channel :   referencia do canal que será especionado.
     * @param { client[] }      clt     :   cliente que está enviando o comando.
     * @param { char* } kick_nickname   :   Nome do cliente 
     * @permision: admin only.
     * 
     * Exemplo:
        Client >> /kick <nickname>\n

        SUCESSO
        SERVER << /kick SUCCESS <nickname>\n

        FALHAS
        SERVER << /kick ERR_NOSUCHNICK\n
        SERVER << /kick ERR_CHANOPRIVSNEEDED\n
    * 
    */
    void CHANNEL_kick_user(CHANNEL_conn* channel, struct _client* clt, const char* kick_nickname); 
    
    /**
     * @function CHANNEL_unkick_user
     * 
     * Permite que o usuário possa entrar no canal novamente, caso tenha sido banido.
     * 
     * @param { CHANNEL_conn* } channel :   referencia do canal que será especionado.
     * @param { client[] }      clt     :   cliente que está enviando o comando.
     * @param { char* } unkick_nickname :   Nome do cliente 
     * @permision: admin only.
     * 
     */
    void CHANNEL_unkick_user(CHANNEL_conn* channel, struct _client* clt, const char* unkick_nickname); 
    
    /**
     * @function CHANNEL_send_message
     * 
     * Função que envia a mensagem para um cliente. São feitas 5 tentativas antes 
     * do procedimento ser abortado.
     * 
     * @param { int }          cl_socket :   Id do Socket do cliente conectado.    
     * @param { const char*  } buffer    :   Buffer auxiliar para compor as mensagens.
     * @permision: all on this channel
     * 
     */
    bool CHANNEL_send_message(int cl_socket, const char* buffer); 
    
    /**
     * @function CHANNEL_send_message_all
     * 
     * Envia mensagem para todos os participantes do canal.
     * 
     * @param { CHANNEL_conn* } channel :   referencia do canal que a mensagem vai ser enviada.
     * @param { char* }         buffer  :   Buffer auxiliar para compor as mensagens.
     * @permision: All in this channel 
     * 
     */
    void CHANNEL_send_message_all(CHANNEL_conn* channel, const char* buffer);
    
    /**
     * @function CHANNEL_send_message_one
     * 
     * Envia mensagem pra um cliente especifico. 
     * 
     * @param { CHANNEL_conn* } channel :   referencia do canal que a mensagem vai ser enviada.
     * @param { client[] }      clt     :   cliente que está enviando a mensagem.
     * @param { char* }         buffer  :   Buffer auxiliar para compor as mensagens.
     * @permision: all on this channel
     * 
     */
    void CHANNEL_send_message_one(CHANNEL_conn* channel,  struct _client* clt,const char* buffer);
    
    /**
     * @function CHANNEL_mute_user
     * 
     * Silencia o usuário do canal e o proibe de enviar mensagem.
     * 
     * @param { CHANNEL_conn* } channel :   referencia do canal que será especionado.
     * @param { client[] }      clt     :   cliente que está enviando o comando.
     * @param { char* } mute_nickname   :   Nome do cliente 
     * @permision: admin only.
     * 
     * Exemplo:
        Client >> /mute <nickname>\n

        SUCESSO
        SERVER << /mute SUCCESS <nickname>\n

        FALHAS
        SERVER << /mute ERR_NOSUCHNICK\n
        SERVER << /mute ERR_CHANOPRIVSNEEDED\n
    * 
    */
    void CHANNEL_mute_user(CHANNEL_conn* channel, struct _client* clt, const char* mute_nickname);
    
    /**
     * @function CHANNEL_unmute_user
     * 
     * Devolve a permissõ de fala do usuário.
     * 
     * @param { CHANNEL_conn* } channel           :   referencia do canal que será especionado.
     * @param { client[] }      clt               :   cliente que está enviando o comando.
     * @param { char* }         unmute_nickname   :   Nome do cliente 
     * @permision: admin only.
     * 
     */
    void CHANNEL_unmute_user(CHANNEL_conn* channel, struct _client* clt, const char* unmute_nickname);

    /**
     * @function CHANNEL_invite
     * 
     * Envia um convite para o usuário entrar em um canal.
     * Esse é o único mode de entrar em um canal privado.
     * 
     * @param { CHANNEL_conn* } channel           :   Referencia do canal que será especionado.
     * @param { client[] }      clt               :   Cliente que está enviando o comando.
     * @param { const char* }   invite_nickname   :   Nome do cliente convidado.
     * @permision: admin only.
     * 
     */
    void CHANNEL_invite(CHANNEL_conn* channel, struct _client* clt, const char* kick_nickname);
    
    /**
     * @function CHANNEL_whois
     * 
     * Função que informa o Ipv4 do usuário. O comando é usado pelo admin e pode
     * somente ver o IP dos usuários no canal.
     * 
     * @param { CHANNEL_conn* } channel :   referencia do canal que será especionado.
     * @param { client[] }      clt     :   cliente que está enviando o comando.
     * @param { char* }         whois   :   Nome do cliente 
     * @permision: admin only.
     * 
     * Exemplo:
        Client >> /whois <nickname>\n

        SUCESSO
        SERVER << /whois SUCCESS <nick> <IP>\n

        FALHAS
        SERVER << /whois ERR_NOSUCHNICK\n
        SERVER << /whois ERR_CHANOPRIVSNEEDED\n
    * 
    */
    void CHANNEL_whois(CHANNEL_conn* channel, struct _client* clt, char* whois);
    
    /**
     * @function CHANNEL_mode
     * 
     * Função que muda do estado do canal de público para privado, vice-versa.
     * 
     * @param { CHANNEL_conn* } channel :   referencia do canal que será modificado.
     * @param { client[] }      clt     :   cliente que está enviando o comando.
     * @param { bool }       is_public  :   Variavel que diz se o canal é público ou privado.
     * @permision: admin only.
     * 
     * Exemplo:
        Client >> /mode -i\n (canal público <padrão>)
        Client >> /mode +i\n (apenas convidado)

        SUCESSO
        SERVER << /mode SUCCESS\n

        FALHAS
        SERVER << /mode ERR_CHANOPRIVSNEEDED\n
    *
    */
    void CHANNEL_mode(CHANNEL_conn* channel, struct _client* clt, bool is_public); 
    
    /**
     * @function CHANNEL_broadcast
     * 
     * Envia de mensagem no canal referente a operação feita. 
     *  
     * @param { CHANNEL_conn* } channel :   referencia do canal que a mensagem vai ser enviada.
     * @param { client[] }      clt     :   cliente que está enviando a mensagem.
     * @param { char* }         buffer  :   Buffer auxiliar para compor as mensagens.
     * @permision: all in this channel 
     * 
     */
    void CHANNEL_broadcast(CHANNEL_conn* channel, struct _client* clt, int type, const char* buffer); 

#endif