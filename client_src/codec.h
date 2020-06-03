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



#endif 