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
  * As funções de DENCODING são responsáveis por:
  * | -- interpretar as mensagens do protocolo de acordo com a ação.
  * | -- extrair e validar os dados recebidos do servidor.
  * | -- retornar os dados de maneira organizada e semântica.
  * 
  * Obs: Não cabe à codec.h fazer verificações de conexão, pois para ela só 
  *      importa a manipulação de strings para envio ao servidor ou a extração
  *      dos dados das mensagens recebidas.
  *
  * Obs 2: A biblioteca não altera nenhum estado do chat, apenas realiza o 
  *        processamento das strings de comando. 
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
     * SUCCESS
     * Código que indica que uma operação foi realizada com sucesso no servidor.
     * 
     * RPL_NAMREPLY
     * Código especial enviado após um /join que lista os membros de um servidor.
     *
     * ERR_*
     * Códigos de erros enviados pelo servidor e definidos na RFC.
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
      #define RPL_NAMREPLY          353
      
      #define ERR_NOSUCHNICK        401
      #define ERR_NOSUCHCHANNEL     403
      #define ERR_TOOMANYCHANNELS   405
      #define ERR_ERRONEUSNICKNAME  432
      #define ERR_NICKNAMEINUSE     433
      
      #define ERR_CHANNELISFULL     471
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

      /**
       * @function
       * char* cdc_encode_nickname(relay_chat* rc, const char* cmd);
       * -------------------------------
       * 
       * Recebe e processa a string cmd referente à ACTION_NICKNAME enviada pelo
       * usuário. Tem como função extrair do comando a string de nickname inserida
       * pelo usuário e formatá-la no padrão do protocolo.
       *
       * Se tudo estiver correto retorna um ponteiro para a mensagem formatada e
       * alocada na memória no formato "/nickname <nickname>\n"
       * 
       * return: (char*) apontando para a mensagem alocada.
       *         NULL em caso de nickname ou comando inválido.
       *
       * Obs: char* deve ser desalocado pelo usuário posteriormente.
       */
        char* cdc_encode_nickname(relay_chat* rc, const char* cmd);

      /**
       * @function
       * char* cdc_encode_nickname(relay_chat* rc, const char* cmd);
       * -------------------------------
       * 
       * Recebe e processa a string cmd referente à ACTION_JOIN enviada pelo
       * usuário. Tem como função extrair do comando o nome do canal inserido
       * pelo usuário e formatá-lo no padrão do protocolo.
       *
       * Se tudo estiver correto retorna um ponteiro para a mensagem formatada e
       * alocada na memória no formato "/join <channel>\n"
       * 
       * return: (char*) apontando para a mensagem alocada.
       *         NULL em caso de parâmetros ou comando inválido.
       *
       * Obs: char* deve ser desalocado pelo usuário posteriormente.
       */
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

      /**
       * @function
       * char* cdc_encode_mode(relay_chat* rc, const char* cmd);
       * -------------------------------
       * Recebe e processa a string cmd referente à ACTION_MODE enviada pelo
       * usuário. Tem como função extrair e vaidar do comando os parâmetros
       * inseridos pelo usuário.
       *
       * Se tudo estiver correto retorna um ponteiro para a mensagem formatada e
       * alocada na memória no formato "/mode [+i|-i]\n"
       * 
       * return: (char*) apontando para a mensagem alocada.
       *         NULL em caso de parâmetros ou comando inválido.
       *
       * Obs: char* deve ser desalocado pelo usuário posteriormente.
       */
        char* cdc_encode_mode(relay_chat* rc, const char* cmd);

      /**
       * @function
       * char* cdc_encode_invite(relay_chat* rc, const char* cmd);
       * char* cdc_encode_whois(relay_chat* rc, const char* cmd);
       * char* cdc_encode_mute(relay_chat* rc, const char* cmd);
       * char* cdc_encode_unmute(relay_chat* rc, const char* cmd);
       * char* cdc_encode_kick(relay_chat* rc, const char* cmd);
       * char* cdc_encode_unkick(relay_chat* rc, const char* cmd);
       * -------------------------------
       * Todas as funções acima processam uma string com um fluxo genérico 
       * em comum e portanto podem ser explicadas conjuntamente.
       * -------------------------------
       * Recebe e processa a string cmd referente à ACTION_X enviada pelo
       * usuário. Tem como função extrair do comando o nickname inserido pelo
       * usuário, validá-lo e em seguida formatar uma nova string no padrão
       * do protocolo.
       *
       * Se tudo estiver correto retorna um ponteiro para a mensagem formatada e
       * alocada na memória no formato "/X <nickname>\n"
       * 
       * return: (char*) apontando para a mensagem alocada.
       *         NULL em caso de nickname ou comando inválido.
       *
       * Obs: char* deve ser desalocado pelo usuário posteriormente.
       */
        char* cdc_encode_invite(relay_chat* rc, const char* cmd);
        char* cdc_encode_whois(relay_chat* rc, const char* cmd);
        char* cdc_encode_mute(relay_chat* rc, const char* cmd);
        char* cdc_encode_unmute(relay_chat* rc, const char* cmd);
        char* cdc_encode_kick(relay_chat* rc, const char* cmd);
        char* cdc_encode_unkick(relay_chat* rc, const char* cmd);

      /**
       * @function
       * int cdc_decode_nickname(relay_chat* rc, const char* cmd, char* nickname);
       * -------------------------------
       * Processa uma mensagem/comando referente à ACTION_NICKNAME recebida pelo 
       * servidor a fim de extrair e validar os dados recebidos.
       * 
       * Caso o nickname recebido seja válido o valor é copiado em (*nickname) para
       * ser retornado ao usuário.
       * 
       *
       * return: INVALID_PROTOCOL  - parâmetros ou nickname inválidos
       *         SUCCESS           - nickname definido com sucesso no servidor.
       *         ERR_NICKNAMEINUSE - nickname válido porém em uso por outro cliente.
       *         ERR_ERRONEUSNICKNAME - nickname inválido segundo o servidor.
       */
        int cdc_decode_nickname(relay_chat* rc, const char* cmd, char* nickname);
      
      /**
       * @function
       *  int cdc_decode_join(relay_chat* rc, const char* cmd, char* channel, bool* is_admin, char* channel_members);
       * -------------------------------
       * Processa uma mensagem/comando referente à ACTION_JOIN recebida pelo 
       * servidor a fim de extrair e validar os dados recebidos.
       * 
       * Caso os parâmetros estejam corretos os mesmos serão salvos nos parâmetros
       * (char* channel, bool* is_admin, char* channel_members).
       * 
       *
       * return: INVALID_PROTOCOL   - parâmetros ou dados inválidos
       *         SUCCESS            - conexão ao canal feita com sucesso. 
       *         ERR_INVITEONLYCHAN - o canal existe porém necessita de convites.
       *         ERR_BANNEDFROMCHAN - o canal existe porém o usuário foi proibido de acessar.
       *         ERR_TOOMANYCHANNELS - o servidor esgotou os slots de canais.
       *         ERR_CHANNELISFULL   - o canal já atingiu o máximo de participantes.
       */
        int cdc_decode_join(relay_chat* rc, const char* cmd, char* channel, bool* is_admin, char* channel_members);

      /**
       * @function
       * int cdc_decode_client_message(relay_chat* rc, const char* cmd, char* nickname, char* content);
       * -------------------------------
       * Processa uma mensagem/comando referente à ACTION_MSG recebida pelo 
       * servidor a fim de extrair e validar os dados recebidos.
       * 
       * Caso os parâmetros estejam corretos os mesmos serão salvos nos parâmetros
       * (char* nickname, char* content), senão o conteúdo é ignorado.
       * 
       *
       * return: INVALID_PROTOCOL   - parâmetros ou dados inválidos
       *         SUCCESS            - formato correto e parâmetros salvos.
       */
      int cdc_decode_client_message(relay_chat* rc, const char* cmd, char* nickname, char* content);
      
      
      /**
       * @function
       * int cdc_decode_list(relay_chat* rc, const char* cmd, char* channels);
       * -------------------------------
       * Processa uma mensagem/comando referente à ACTION_LIST recebida pelo 
       * servidor a fim de extrair e validar os dados recebidos.
       * 
       * Caso os parâmetros estejam corretos os mesmos serão salvos nos parâmetros
       * (char* channels), senão o conteúdo é ignorado.
       * 
       *
       * return: INVALID_PROTOCOL   - parâmetros ou dados inválidos
       *         SUCCESS            - formato correto e parâmetros salvos.
       */
        int cdc_decode_list(relay_chat* rc, const char* cmd, char* channels);

     /**
       * @function
       * int cdc_decode_mode(relay_chat* rc, const char* cmd);
       * -------------------------------
       * Processa uma mensagem/comando referente à ACTION_MODE, retornando 
       * se o modo do canal foi alterado com sucesso segundo o servidor.
       * 
       * return: INVALID_PROTOCOL     - parâmetros ou dados inválidos
       *         SUCCESS              - formato correto e parâmetros salvos.
       *         ERR_CHANOPRIVSNEEDED - o usuário não possui privilégio de administrador do canal.
       */
        int cdc_decode_mode(relay_chat* rc, const char* cmd);

      /**
       * @function
       * int cdc_decode_whois(relay_chat* rc, const char* cmd, char* nickname, char* ip_info);
       * -------------------------------
       * Processa uma mensagem/comando referente à ACTION_WHOIS recebida do 
       * servidor e extrai os parâmetros recebidos.
       *
       * Caso os parâmetros estejam corretos os mesmos serão salvos nos parâmetros
       * char* nickname, char* ip_info), senão o conteúdo é ignorado.
       * 
       * return: INVALID_PROTOCOL     - parâmetros ou dados inválidos
       *         SUCCESS              - formato correto e parâmetros salvos.
       *         ERR_CHANOPRIVSNEEDED - o usuário não possui privilégio de administrador do canal.
       */
        int cdc_decode_whois(relay_chat* rc, const char* cmd, char* nickname, char* ip_info);

      /**
       * @function
       * int cdc_decode_invite(relay_chat* rc, const char* cmd, char* nickname);
       * int cdc_decode_mute(relay_chat* rc, const char* cmd, char* nickname);
       * int cdc_decode_unmute(relay_chat* rc, const char* cmd, char* nickname);
       * int cdc_decode_kick(relay_chat* rc, const char* cmd, char* nickname);
       * int cdc_decode_unkick(relay_chat* rc, const char* cmd, char* nickname);
       * -------------------------------
       * Todas as funções acima processam um comando com um fluxo genérico 
       * em comum e portanto podem ser explicadas conjuntamente.
       * -------------------------------
       * Processa uma mensagem/comando referente à ACTION_X recebida do 
       * servidor e extrai os parâmetros recebidos.
       *
       * Caso os parâmetros estejam corretos os mesmos serão salvos nos parâmetros
       * (char* nickname), senão o conteúdo é ignorado.
       * 
       * return: INVALID_PROTOCOL     - parâmetros ou dados inválidos
       *         SUCCESS              - formato correto e parâmetros salvos.
       *         ERR_CHANOPRIVSNEEDED - o usuário não possui privilégio de administrador do canal.
       *         ERR_NOSUCHNICK       - o nickname informado não consta no canal ou no servidor.
       */
        int cdc_decode_invite(relay_chat* rc, const char* cmd, char* nickname);
        int cdc_decode_mute(relay_chat* rc, const char* cmd, char* nickname);
        int cdc_decode_unmute(relay_chat* rc, const char* cmd, char* nickname);
        int cdc_decode_kick(relay_chat* rc, const char* cmd, char* nickname);
        int cdc_decode_unkick(relay_chat* rc, const char* cmd, char* nickname);

      /**
       * @function
       * int cdc_decode_server_message(relay_chat* rc, const char* cmd,  char* content);
       * int cdc_decode_channel_message(relay_chat* rc, const char* cmd,  char* content);
       * -------------------------------
       * Todas as funções acima processam um comando com um fluxo genérico 
       * em comum e portanto podem ser explicadas conjuntamente.
       * -------------------------------
       * Processa uma mensagem/comando referente à ACTION_X recebida do 
       * servidor e extrai os parâmetros recebidos.
       *
       * Caso os parâmetros estejam corretos os mesmos serão salvos nos parâmetros
       * (char* content), senão o conteúdo é ignorado.
       * 
       * return: INVALID_PROTOCOL     - parâmetros ou dados inválidos
       *         SUCCESS              - formato correto e parâmetros salvos.
       */
        int cdc_decode_server_message(relay_chat* rc, const char* cmd,  char* content);
        int cdc_decode_channel_message(relay_chat* rc, const char* cmd,  char* content);

      /**
       * @function
       * int is_valid_channel_name(char* name);
       * -------------------------------
       * Verifica se o parametro name está válido segundo o formato estabelecido
       * pelo protocolo implementado.
       *
       * nome de canal valido: 
       * - 1º caracter precisa ser '&' .
       * - apenas caractéres imprimíveis.
       * - não pode haver virgulas ou espaços em branco em meio ao nome.
       *
       * returne: true  - nome de canal válido
       *          false - nome de canal inválido
       */
        int is_valid_channel_name(char* name);

      /**
       * @function
       * int is_valid_channel_name(char* name);
       * -------------------------------
       * Verifica se o parametro name está válido segundo o formato estabelecido
       * pelo protocolo implementado.
       *
       * nickname valido: 
       * - caracteres alfabéticas [a-z e A-Z]
       * - números [0-9]
       - - traços '-' e undersocores '_'
       * 
       * returne: true  - a string é um nickname válido
       *          false - nickname invalido.  
       */
        int is_valid_nickname(char* name);


#endif 