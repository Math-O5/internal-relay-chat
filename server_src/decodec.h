/**
  * #include "decodec.h"
  * -----------------
  * 
  * Biblioteca responsável por fazer o decoding da informacao recebida
  * pelo cliente
  * 
  * OBS.: No momento, nao existe a validacao dos comandos recebidos,
  * pois acredita-se que estao sendo tratados diretamente no cliente
  *  
  */
#ifndef DECODEC_H
    #define DECODEC_H 1459
    
    /**
     * Constantes e Macros
     * -------------------
     * 
     * ACTION_*****
     * Valores utilizados para identificar a ação que o usuário deseja
     * realizar
     * 
     *  
    */
      // COMANDOS GERAIS
      #define ACTION_NONE               0
      #define ACTION_DISCONNECT       301
      #define ACTION_QUIT             302
      #define ACTION_PING             304

      // COMANDOS QUE ALTERAM ESTADO DO CLIENTE 
      #define ACTION_NICK            401
      #define ACTION_JOIN            402

      // COMANDOS DO CANAL
      #define ACTION_MESSAGE         503
      #define ACTION_LIST            504

      // COMANDOS ADMINISTRADOR
      #define ACTION_MODE            601     
      #define ACTION_INVITE          602
      #define ACTION_WHOIS           603
      #define ACTION_MUTE            604
      #define ACTION_UNMUTE          605
      #define ACTION_KICK            606
      #define ACTION_UNKICK          607

      // CASOS DE SUCESSO E ERRO
      #define VALID_PROTOCOL          1
      #define INVALID_PROTOCOL       0
      
      #define ERR_NOSUCHNICK        401
      #define ERR_ERRONEUSNICKNAME  432
      #define ERR_NICKNAMEINUSE     433
      
      #define ERR_INVITEONLYCHAN    473
      #define ERR_BANNEDFROMCHAN    474

      #define ERR_CHANOPRIVSNEEDED  482

      // MODE (INVITE)
      #define INVITE_ONLY_ON        1
      #define INVITE_ONLY_OFF       0

    /**
     * DETECT ACTION FUNCTION
    */

      /**
       * @function
       * int detect_action(const char* cmd);
       * -----------------------------------
       * 
       * Funcao que retorna a acao/comando que o cliente
       * deseja executar
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       * 
       * Retorno:
       * --------
       *     int :
       *         ACTION_DISCONNECT       301
       *         ACTION_QUIT             302
       *         ACTION_PING             304
       *         ACTION_NICK             401
       *         ACTION_JOIN             402
       *         ACTION_MESSAGE          503
       *         ACTION_LIST             504
       *         ACTION_MODE             601  
       *         ACTION_INVITE           602
       *         ACTION_WHOIS            603
       *         ACTION_MUTE             604
       *         ACTION_UNMUTE           605
       *         ACTION_KICK             606
       *         ACTION_KICK             606
       *         ACTION_NONE              0
      */
      int detect_action(const char* cmd);

    /**
     * DECODER FUNCTIONS
    */

      // *** Nickname ***
      /**
       * @function
       * int decode_nickname(const char* cmd, char* nickname);
       * ------------------------------------------------------
       * 
       * Retorna via parametro (nickname) o nome do cliente
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     nickname : char*
       *         nome do cliente
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_nickname(const char* cmd, char* nickname);

      // *** Join ***
      /**
       * @function
       * int decode_join(const char* cmd, char* channel);
       * -------------------------------------------------
       * 
       * Retorna via parametro (channel) o nome do canal em que o cliente
       * quer entrar
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     channel : char*
       *         nome do canal
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_join(const char* cmd, char* channel);

      // *** Mode ***
      /**
       * @function
       * int decode_mode(const char* cmd, bool* invite_mode);
       * -----------------------------------------------------
       * 
       * Retorna via parametro (invite_mode) o modo do canal (entrada por convite ou nao)
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     invite_mode : bool*
       *         INVITE_ONLY_ON  (1) -> entrada no canal somente por convite
       *         INVITE_ONLY_OFF (0) -> entrada livre no canal 
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_mode(const char* cmd, bool* invite_mode);

      // *** Invite ***
      /**
       * @function
       * int decode_invite(const char* cmd, char* invite_nickname);
       * -----------------------------------------------------------
       * 
       * Retorna via parametro (invite_nickname) o nome do cliente a ser convidado
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     invite_nickname : char*
       *         nome do cliente a ser convidado
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_invite(const char* cmd, char* invite_nickname);

      // *** Whois ***
      /**
       * @function
       * int decode_whois(const char* cmd, char* whois_nickname);
       * ---------------------------------------------------------
       * 
       * Retorna via parametro (whois_nickname) o nickname do cliente o qual deseja
       * saber o IP
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     whois_nickname : char*
       *         nickame do cliente o qual sera pego o IP
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_whois(const char* cmd, char* whois_nickname);

      // *** Mute ***
      /**
       * @function
       * int decode_mute(const char* cmd, char* mute_nickname);
       * -------------------------------------------------------
       * 
       * Retorna via parametro (mute_nickname) o nickname do cliente o qual sera
       * mutado
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     mute_nickname : char*
       *         nickname do cliente o qual sera mutado
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_mute(const char* cmd, char* mute_nickname);

      // *** Unmute ***
      /**
       * @function
       * int decode_unmute(const char* cmd, char* unmute_nickname);
       * -----------------------------------------------------------
       * 
       * Retorna via parametro (unmute_nickname) o nickname do cliente o qual sera
       * desmutado
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     unmute_nickname : char*
       *         nickname do cliente o qual sera desmutado
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_unmute(const char* cmd, char* unmute_nickname);

      // *** Kick ***
      /**
       * @function
       * int decode_kick(const char* cmd, char* kick_nickname);
       * -------------------------------------------------------
       * 
       * Retorna via parametro (kick_nickname) o nickname do cliente o qual sera
       * kikado do canal
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     kick_nickname : char*
       *         nickname do cliente o qual sera kikado do canal
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_kick(const char* cmd, char* kick_nickname);

      // *** Unkick ***
      /**
       * @function
       * int decode_kick(const char* cmd, char* unkick_nickname);
       * -------------------------------------------------------
       * 
       * Retorna via parametro (unkick_nickname) o nickname do cliente o qual sera
       * deskikado do canal
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     unkick_nickname : char*
       *         nickname do cliente o qual sera deskikado do canal
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_unkick(const char* cmd, char* unkick_nickname);

      // *** Msg ***
      /**
       * @function
       * int decode_msg(const char* cmd, char* role, char* nickname, char* message_content);
       * --------------------------------------------------------------------------------------
       * 
       * Retorna via parametro o role (role) e o nickname (nickname) do cliente, e tambem a 
       * mensagem (message_content) que ele enviou
       * 
       * Parametros:
       * -----------
       *     cmd : const char*
       *         comando (inteiro)
       *     role : char*
       *         role do cliente (@ administrador | # usuario comum)
       *     nickname : char*
       *         nickname do cliente que enviou a mensagem
       *     message_content : char*
       *         mensagem que o cliente enviou    
       * 
       * Retorno:
       * --------
       *     SUCCESS           : Foi possivel fazer o decode do comando 
       *     INVALID_PROTOCOL  : Nao foi possivel fazer o decode do comando devida a sua formatacao
      */
      int decode_msg(const char* cmd, char* role, char* nickname, char* message_content);

#endif 