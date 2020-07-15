#ifndef MESSAGE_H
    #define MESSAGE_H 2020

    /**
     * Esta Biblioteca é responsável por todas as mensagens de log do servidor
     * Cada mensagem tem o seguinte formato:
     * [tipo de log] [data e horário] - conteúdo da mensagem
     * 
     * Tipo de log
     * -----------
     * [x] - Mensagens de erro ou de desconexão de um cliente
     *     Ex.: [x] - [21/07/2000 - 16:30:00] O cliente 10 não responde...
     * [+] - Mensagens de interação que obtiveram sucesso (novas conexões, servidor inicializado, ...)
     *     Ex.: [+] - [21/07/2000 - 16:30:00] Numero maximo de conexoes permitidas: 10
     * [-] - Mensagem de finalização do servidor
     *     Ex.: [-] - [21/07/2000 - 16:30:00] Servidor finalizado com sucesso.
     * [ ] - Sem log
     * 
     * Data e horário
     * --------------
     * Data e horário em que ocorreu o evento
     *     Ex.: 21/07/2000 - 16:30:00
     * 
     * Conteúdo da mensagem
     * --------------------
     * O conteúdo da mensagem vai ser um evento ocorrido ou a própria mensagem que determinado cliente enviou
     *     Exs.: [+] - [21/07/2000 - 16:30:00] Foi enviado pong para o cliente 10.
     *           [ ]
     *           [+] - [21/07/2000 - 16:30:00] <CLIENTE 12>: Olá !!!
    */

    /**
     * Macros e constantes
     * -------------------
     * 
     * CONNECT
     * Valor que indica que o servidor foi inicializado com sucesso
    */
        #define CONNECT 0
    
    /**
     * Cores
    */
        #define COLORL_GRAY     "\033[0;30m"
        #define COLOR_RED       "\033[0;31m"
        #define COLOR_GREEN     "\033[0;32m"
        #define COLOR_YELLOW    "\033[0;33m"
        #define COLOR_BLUE      "\033[0;34m"
        #define COLOR_MAGENTA   "\033[0;35m"
        #define COLOR_CYAN      "\033[0;36m"
        #define COLOR_WHITE     "\033[0;37m"

        #define COLORB_GRAY      "\033[1;30m"
        #define COLORB_RED       "\033[1;31m"
        #define COLORB_GREEN     "\033[1;32m"
        #define COLORB_YELLOW    "\033[1;33m"
        #define COLORB_BLUE      "\033[1;34m"
        #define COLORB_MAGENTA   "\033[1;35m"
        #define COLORB_CYAN      "\033[1;36m"
        #define COLORB_WHITE     "\033[1;37m"

        #define COLOR_RESET      "\033[0m"

        #define PREFIX_NORMAL       "  \033[1;37m[-]"
        #define PREFIX_EMPTY_LINE   "  \033[1;37m[ ]\n"

        #define PREFIX_SUCCESS      "  \033[1;32m[+]"
        #define PREFIX_ERROR        "  \033[1;31m[x]"
    
    /**
     * Bibliotecas importadas
     * ----------------------
    */
        #include <stdio.h>
        #include <stdlib.h> /* system */
        #include <time.h>
        #include <netinet/in.h> /* struct sockaddr_in */

        #include "clients.h"

    using namespace std;
    /**
     * @function
     * 
     * void time_log(char* t_log, int t_log_size)
     * ------------------------------------------
     * 
     * Retorna via parâmetro (char* t_log), a data e horário atual. O tamanho da string retonarda
     * é indicado por t_log_size. A data e o horário atual foi retirado da função localtime() da biblioteca
     * <time.h>. Esta informação será utilizada para o log de cada evento.
     * 
     * Formato da string: "dd/mm/aaaa - hh:mm:ss" 
    */
    void time_log(char* t_log, int t_log_size);

    /**
     * @function
     * 
     * void msg_inicio(int status)
     * ---------------------------
     * 
     * Mensagem de inicialização do servidor. Se o status == CONNECT (0), o servidor foi inicializado
     * com sucesso, e se o status != CONNECT o servidor não foi inicializado..
    */
    void msg_inicio(int status);

    /**
     * @function
     * 
     * void msg_info_server(int port, int sv_socket, int max_conn, int cl_count)
     * -------------------------------------------------------------------------
     * 
     * Mensagem com as informações do servidor: porta, socket do servidor conectado, máximo de conexões
     * e quantidade de clientes conectados
    */
    void msg_info_server(int port, int sv_socket, int max_conn, int cl_count);

    /**
     * @function
     * 
     * void msg_info_client(int id, int socket, struct sockaddr_in cl_addr)
     * --------------------------------------------------------------------
     * 
     * Mensagem com as informaçẽos do cliente: id, socket co cliente conectado, host e porta
    */
    void msg_info_client(int id, int socket, struct sockaddr_in cl_addr);

    /**
     * @function
     * 
     * void msg_recv_cliente(int id, char* buffer)
     * -------------------------------------------
     * 
     * Mensagem que o cliente enviou para o servidor, de acordo com id do cliente
    */
    void msg_recv_cliente(int id, char* buffer);

    /**
     * @function
     * 
     * void msg_send_cliente(int id_emissor, int id_receptor)
     * ------------------------------------------------------
     * 
     * Mensagem entregue ao cliente: cliente que enviou a mensagem (id_emissor) para o cliente
     * que recebeu (id_receptor)
    */
    void msg_send_cliente(int id_emissor, int id_receptor);

    /**
     * @function
     * 
     * void msg_cliente_conexao()
     * --------------------------
     * 
     * Mensagem de nova conexão no servidor
    */
    void msg_cliente_conexao();

    /**
     * @function
     * 
     * void msg_client_no_response(int id)
     * -----------------------------------
     * 
     * Mensagem para indicar que não houve resposta do cliente (int id)...
    */
    void msg_client_no_response(int id);

    /**
     * @function
     * 
     * void msg_cliente_desconexao(int id)
     * -----------------------------------
     * 
     * Mensagem de desconexão do cliente (int id)
    */
    void msg_cliente_desconexao(int id);

    /**
     * @function
     * 
     * void msg_max_client(struct sockaddr_in cl_addr)
     * -----------------------------------------------
     * 
     * Mensagem de erro, que indica que o servidor já está no limite de clientes conectados.
     * A mensagem também mostra as informações od cliente que tentou se conectar (host e porta)
    */
    void msg_max_client(struct sockaddr_in cl_addr);
    
    /**
     * @function
     * 
     * void msg_info_ping(int id)
     * --------------------------
     * 
     * Mensagem que o cliente (int id) enviou o comando ping para o servidor
    */
    void msg_info_ping(int id);

    /**
     * @function
     * 
     * void msg_info_pong(int id)
     * --------------------------
     * 
     * Mensagem avisando que a reposta do comando /ping (pong) foi entregue ao cliente (int id)
    */
    void msg_info_pong(int id);
    
    /**
     * @function
     * 
     * void msg_nickname_cliente()
     * --------------------------
     * 
     * Mensagem atualização do nome cliente
    */
    void msg_nickname_cliente(int id, const char* newNickname, char* oldNickname);
    void msg_join_channel(struct _client* clt);

    /**
     * @function
     * 
     * void msg_encerrar_server()
     * --------------------------
     * 
     * Mensagem de finalização do servidor
    */
    void msg_encerrar_server();

    // ############################################################ PARTE 3 ############################################################

// ******************************* MENSAGENS GERAIS *******************************

    /**
     * @function
     * 
     * void msg(char type, char* msg)
     * ------------------------------
     * 
     * Mensagem generica definida pelo parametro 'msg' e o seu
     * tipo (+ , - , x , ...) eh defindo pelo parametro 'type'  
     *
    */
    void msg(char type, char* msg);
    void msg_request_client_channel(int id, char* nickname);
    void msg_client_channel(const char* nickname, const char* channel);
    void msg_channel_client(const char* nickname, const char* channel);
    void msg_client_no_response_channel(int id, char* nickname, char* channel);

// ******************************* LIST *******************************

    /**
     * @function
     * 
     * void msg_list(char* nickname)
     * -----------------------------
     * 
     * Mensagem indicando que foram listados o nome dos canais
     * para o usuario que solicitou 
     * 
    */
    void msg_list(int id, char* nickname);

    void msg_request_list(int id, char* nickname);

// ******************************* JOIN *******************************

    /**
     * @function
     * 
     * void msg_create_channel(int id, char* nickname, char* channel_name);
     * -----------------------------------------------------------
     * 
     * Mensagem que indica que o canal foi criado
     * 
    */
    void msg_create_channel(int id, char* nickname, char* channel_name);

    void msg_request_create_channel(int id, char* nickname, char* channel_name);

    void msg_error_create_channel(int id, char* nickname);

    /**
     * @function
     * 
     * void msg_join_channel(int id, char* nickname, char* channel_name)
     * ---------------------------------------------------------
     * 
     * Mensagem que notifica que o usuario entrou no canal
     * 
    */
    void msg_join_channel(int id, char* nickname, char* channel_name);

    /**
     * @function
     * 
     * void msg_exit_channel(char* nickname, char* channel_name)
     * ---------------------------------------------------------
     * 
     * Mensagem que avisa que o usuario saiu do canal 
     * 
    */
    void msg_exit_channel(char* nickname, char* channel_name);

    /**
     * @function
     * 
     * void msg_destroy_channel(char* channel_name)
     * --------------------------------------------
     * 
     * Mensagem de desativacao do canal
     * 
    */
    void msg_destroy_channel(char* channel_name);

// ******************************* NICKNAME *******************************

     /**
     * @function
     * 
     * void msg_create_nickname(int id, char* nickname)
     * ------------------------------------------------
     * 
     * Mensagem de criacao do nickname pelo cliente
     * 
    */
    void msg_create_nickname(int id, char* nickname);

     /**
     * @function
     * 
     * void msg_change_nickname(char* nickname, char* oldNickname)
     * ------------------------------------------------
     * 
     * Mensagem indicando que o nickname do usuario foi alterado
     * 
    */
    void msg_change_nickname(int id, char* old_nickname, char* new_nickname);

    void msg_request_nickname(int id, char* new_nickname);

    void msg_error_nicknameinuse(int id);

    void msg_error_nickname(int id);

// ******************************* KICK *******************************

    void msg_request_kick(int id, char* nickname);
    
     /**
     * @function
     * 
     * void msg_kick(int id, char* nickname_adm, char* nickname_user)
     * --------------------------------------------------------------
     * 
     * Mensagem avisando que o usuario foi expulso do canal
     * 
    */
    void msg_kick(int id, char* nickname_adm, char* nickname_user);

    void msg_error_kick_no_channel(int id, char* nickname);

    void msg_error_kick_denied(int id, char* nickname);

    void msg_error_kick_no_nick(int id, char* nickname);

// ******************************* UNKICK *******************************
     
    void msg_request_unkick(int id, char* nickname);

     /**
     * @function
     * 
     * void msg_unkick(char* nickname_adm, char* nickname_user, char* channel_name)
     * ----------------------------------------------------------------------------
     * 
     * Mensagem avisando que o usuario voltou a ser permitido participar do canal
     * 
    */
    void msg_unkick(int id, char* nickname_adm, char* nickname_user);

    void msg_error_unkick_no_channel(int id, char* nickname);

    void msg_error_unkick_denied(int id, char* nickname);

    void msg_error_unkick_no_nick(int id, char* nickname);

// ******************************* MUTE *******************************

    void msg_request_mute(int id, char* nickname);

    /**
     * @function
     * 
     * void msg_mute(int id, char* nickname_adm, char* nickname_user)
     * --------------------------------------------------------------
     * 
     * Mensagem indicando que o usuario foi mutado
     * 
    */
    void msg_mute(int id, char* nickname_adm, char* nickname_user);

    void msg_error_mute_no_channel(int id, char* nickname);

    void msg_error_mute_denied(int id, char* nickname);

    void msg_error_mute_no_nick(int id, char* nickname);

// ******************************* UNMUTE *******************************

    void msg_request_unmute(int id, char* nickname);

    /**
     * @function
     * 
     * void msg_unmute(int id, char* nickname_adm, char* nickname_user)
     * ----------------------------------------------------------------
     * 
     * Mensagem indicando que o usuario foi desmutado
     * 
    */
    void msg_unmute(int id, char* nickname_adm, char* nickname_user);

    void msg_error_unmute_no_channel(int id, char* nickname);

    void msg_error_unmute_denied(int id, char* nickname);

    void msg_error_unmute_no_nick(int id, char* nickname);

// ******************************** WHOIS *******************************

    void msg_request_whois(int id, char* nickname);

    /**
     * @function
     * 
     * msg_whois(int id, char* nickname_adm, char* nickname_user_ip)
     * -------------------------------------------------------------
     * 
     * Mensagem que mostra o IP do usuario, o qual foi solicitado pelo administrador do canal
     * 
    */
    void msg_whois(int id, char* nickname_adm, char* nickname_user_ip);

    void msg_error_whois_no_channel(int id, char* nickname);

    void msg_error_whois_not_adm(int id, char* nickname);

    void msg_error_whois_no_nick(int id, char* nickname);

// ******************************** MODE ********************************

    void msg_request_mode(int id, char* nickname);

    /**
     * @function
     * 
     * void msg_invite_only_on(int id, char* nickname_adm, char* channel_name)
     * ---------------------------------------------------------------
     * 
     * Mensagem avisando que o modo do canal foi alterado para apenas convidados
     * 
    */
    void msg_invite_only_on(int id, char* nickname_adm, char* channel_name);

    /**
     * @function
     * 
     * void msg_invite_only_off(int id, char* nickname_adm, char* channel_name)
     * ----------------------------------------------------------------
     * 
     * Mensagem avisando que o modo do canal foi alterado para qualquer usuario possa
     * participar
     * 
    */
    void msg_invite_only_off(int id, char* nickname_adm, char* channel_name);

    void msg_error_mode_no_channel(int id, char* nickname);

    void msg_error_mode_not_adm(int id, char* nickname, char* channel_name);

// ******************************** INVITE ********************************

    void msg_request_invite(int id, char* nickname);

    /**
     * @function
     * 
     * void msg_invite(int id, char* nickname_adm, char* nickname_user)
     * ----------------------------------------------------------------
     * 
     * Mensagem indicando que tal usuario convidou outro para o canal
     * 
    */
    void msg_invite(int id, char* nickname_adm, char* nickname_user);

    void msg_error_invite_no_channel(int id, char* nickname);

    void msg_error_invite_denied(int id, char* nickname);

    void msg_error_invite_no_nick(int id, char* nickname);

// ******************************** PING ********************************

    void msg_request_ping(int id, char* nickname);

    void msg_pong(int id, char* nickname);

// ******************************** INVALID PROTOCOL ********************************

    void msg_invalid_protocol(int id);

#endif