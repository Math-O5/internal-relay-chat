 /**
  * #include "codec.h"
  * -----------------
  * 
  * Biblioteca responsável por fazer a transcrição entre as strings manipuladas
  * pelas Threads de Input com as Threads de Conexão, servindo de Codec entre
  * o padrão de exibição com o padrão definido pelo protocolo utilizado entre
  * o Cliente <-> Servidor.
  * 
  * As funções de ENCODING são responsáveis por:
  * | -- interpretar as mensagens recebidas pelo usuário.
  * | -- processar o input baseado no tipo de operação desejado.
  * | -- devolver os parâmetros do comando de maneira organizada.
  * | -- garantir que as mensagens respeitam o protocolo estabelecido
  * |    tanto no formato quando no tamanho máximo permitido.
  * 
  * @TODO
  * As funções de DECODING ainda não foram implementadas para esta versão
  * 
  * Obs: Não cabe à codec.h fazer verificações de conexão, pois para ela só 
  *      importa a manipulação de strings para envio ao servidor ou apresentação.
  *      ao usuário via terminal. 
  */
#ifndef CODEC_H
    #define CODEC_H 1459

    #include "chat.h"     /* DEFAULT_SERVER_HOST e DEFAULT_SERVER_PORT */
    
    /**
     * Constantes e Macros
     * -------------------
     * 
     * ACTION_*****
     * Valores utilizados para identificar a ação que o usuário deseja
     * realizar durante cada input. A descrição de cada ação está descrita
     * de forma detalhada na Thread de Input.
     * 
     * Para a codec.h basta ser capaz de mapear quais ACTIONS existem
     * no programa.
     *  
    */
      // COMANDOS GERAIS
      #define ACTION_NONE               0
      #define ACTION_CONNECT          300
      #define ACTION_DISCONNECT       301
      #define ACTION_QUIT             302
      #define ACTION_HELP             303
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

      // MENSAGEMS SERVER -> CLIENT
      #define ACTION_SERVERMSG       700
      #define ACTION_CHANNELMSG      701


      #define INVALID_PROTOCOL       0
      #define SUCCESS               200
      
      #define ERR_NOSUCHNICK        401
      #define ERR_ERRONEUSNICKNAME  432
      #define ERR_NICKNAMEINUSE     433
      
      #define ERR_INVITEONLYCHAN    473
      #define ERR_BANNEDFROMCHAN    474

      #define ERR_CHANOPRIVSNEEDED  482









    /**
     * ENCONDER FUNCTIONS
     */

      /**
       * @function
       * int cdc_detectar_act(const char* cmd);
       * -------------------------------
       * 
       * Utilizada para detectar se alguma ACTION_* foi disparada ao analisar
       * a string input de comando recebida por parâmetro.
       * 
       * Obs: Comandos obrigatóriamente são iniciados com '/' e a mesma
       * deve estar localizada logo no primeiro BYTE de cmd, caso contrário a mesma
       * será interpretada como uma ACTION_MESSAGE por DEFAULT.
       * 
       * cmd: ponteiro para a string à ser analisada como input de comando
       * 
       * return: algum dos códigos ACTION_* definindo qual era a ação pretendida.
       */
        int cdc_detectar_act(const char* cmd);


      /**
       * @function
       * int cdc_encode_connect(const char* cmd, char* host, char* port);
       * -------------------------------
       * 
       * Recebe e processa a string cmd referente à ACTION_CONNECT enviada pelo
       * usuário. Tem como função definir quais valores devem ser salvos em
       * host e port baseado no formato do input do usuário.
       * 
       * Caso 1º: /connect
       * | -- Nenhum valor foi passado, então é atribuído os valores definidos
       * |    em DEFAULT_SERVER_HOST e DEFAULT_SERVER_PORT para os parâmetros.
       * 
       * Caso 2º: /connect <HOST_IP> <PORT>
       * | -- Salva em host e port os dois próximos parâmetros enviados juntos
       * |    à /connect (Obs: não valida se os valores válidos, pois isso será
       * |    verificado durante o processo de conexão).
       * 
       * cmd: ponteiro para a string à ser analisada como input de comando
       * 
       * host: aponta para a string em que será salvo o HOST
       * 
       * port: aponta para a string em que será salvo o PORT
       * 
       * return: 1 caso algum dos parâmetros não tenha sido enviado corretamente.
       *         0 caso tudo ocorra nos conformes.
       */
        int cdc_encode_connect(relay_chat* rc, const char* cmd, char* host, char* port);

      char* cdc_encode_nickname(relay_chat* rc, const char* cmd);

      char* cdc_encode_join(relay_chat* rc, const char* cmd);

      /**
       * @function
       * char** encode_message(&rc, raw_str, raw_str_len)
       * -------------------------------
       * 
       * Utilizada na ACTION_MESSAGE para converter a string desejada para o 
       * formato definido no protocolo RFC 1459. Caso a mensagem ultrapasse o limite
       * de caracteres definido pelo protocolo a mesma é separada em multiplas 
       * mensagens distintas.
       * 
       * raw_str: string que será codificada.
       * 
       * raw_str_len: tamanho de raw_str
       * 
       * return: array de strings que representam as mensagens codificadas a partir de
       *         raw. O ponteiro NULL indica o fim do array e deve ser desalocado após 
       *         seu uso.
       */
        char** cdc_encode_client_message(relay_chat* rc, const char* raw_str, int raw_str_len);

      char* cdc_encode_mode(relay_chat* rc, const char* cmd);
      char* cdc_encode_invite(relay_chat* rc, const char* cmd);
      char* cdc_encode_whois(relay_chat* rc, const char* cmd);
      char* cdc_encode_mute(relay_chat* rc, const char* cmd);
      char* cdc_encode_unmute(relay_chat* rc, const char* cmd);
      char* cdc_encode_kick(relay_chat* rc, const char* cmd);
      char* cdc_encode_unkick(relay_chat* rc, const char* cmd);

      int is_valid_channel_name(char* name);
      int is_valid_nickname(char* name);


#endif 