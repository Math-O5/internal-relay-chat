#include "codec.h"
#include "chat.h"
#include <stdlib.h>
#include <string>
#include <string.h>


int enc_detectar_act(const char* cmd){

    int action_code = ACTION_NONE;

    /**
     * strstr(cmd, "/COMMAND") == cmd
     * 
     * O comando só é valido se o usuário iniciar a mensagem
     * com /, caso contrário será sempre considerado uma mensagem.
     */

    // Verificando qual ação o input expressa.
    if ( strstr(cmd, "/connect") == cmd ){
        action_code = ACTION_CONNECT;
    
    } else if ( strstr(cmd, "/disconnect") == cmd ) {
        action_code = ACTION_DISCONNECT;
    
    } else if ( strstr(cmd, "/quit") == cmd ) {
        action_code = ACTION_QUIT;
    
    } else {
        action_code = ACTION_MESSAGE;
    }

    return action_code;
}

int enc_encode_connect(const char* cmd, char* host, char* port){
    if(cmd == NULL || host == NULL || port == NULL){
        return 1;
    }

    char temp_host[120]; 
    char temp_port[120];

    int nro_itens = sscanf(cmd, "/connect %119s %119s", temp_host, temp_port);

    if(nro_itens == 2) {
        strcpy(host, temp_host);
        strcpy(port, temp_port);
    } else {
        strcpy(host, DEFAULT_SERVER_HOST);
        strcpy(port, DEFAULT_SERVER_PORT);
    }

    return 0;
}