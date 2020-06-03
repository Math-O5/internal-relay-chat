 /**
  * #include "codec.h"
  * -----------------
  * 
  */
#ifndef CODEC_H
    #define CODEC_H 1459

    #include "chat.h"
    #include <stdlib.h>
    
    #define ACTION_NONE               0
    #define ACTION_CONNECT          900
    #define ACTION_DISCONNECT       901
    #define ACTION_QUIT             902
    #define ACTION_HELP             903

    #define ACTION_MESSAGE         1000
    #define ACTION_PING            1001

    // ENCODER

      // Recebe a string e retorna o código da ação
      int cdc_detectar_act(const char* cmd);

      // Recebe a string de comando connect e verifica se foram passados
      // os valores de HOST e PORTA. Se sim, salva os valores no 2º e 3º
      // parâmetro, senão salva os valores padrões definidos em chat.h
      // Retorna: 1 caso algum parametro esteja errado 
      int cdc_encode_connect(const char* cmd, char* host, char* port);

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
     * rc: referencia ao objeto relay_chat com os estados do chat.
     * 
     * raw_str: string que será codificada.
     * 
     * raw_str_len: tamanho de raw_str
     * 
     * return: array de strings que representam as mensagens codificadas a partir de
     *         raw. O ponteiro NULL indica o fim do array e deve ser desalocado após 
     *         seu uso.
     */
    char** cdc_encode_client_message(const char* raw_str, int raw_str_len);

#endif 