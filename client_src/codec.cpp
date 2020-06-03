#include "codec.h"
#include "chat.h"
#include <stdlib.h>
#include <string>
#include <string.h>


int cdc_detectar_act(const char* cmd){

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

int cdc_encode_connect(const char* cmd, char* host, char* port){
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

// @COmentários em "chat.h"
char** cdc_encode_client_message(const char* raw_str, int raw_str_len){
    if(raw_str == NULL || raw_str_len <= 0){
        return NULL;
    }

    /** 1ª Etapa - Calculando quantos pacotes serão necessários
     *  @TODO: Nas próximas etapas deverá ser levado em conta o prefixo e outros
     *  strings obrigatórias como definido no protocolo. Por enquanto só quebrar as
     *  strings em blocos ja é o suficiente.
     * 
     * 
     * Obs: (MAX_MESSAGE_LENGHT-1) é necessário porque temos que descontar o espaço 
     * consumido pelo \n que é adicionado ao fim de cada mensagem do protocolo.
     */
    
    int qtd_mensagens  = raw_str_len / (MAX_MESSAGE_LENGHT - 1); 
    qtd_mensagens     += (raw_str_len % (MAX_MESSAGE_LENGHT - 1) > 0) ? 1 : 0;

    /**
     * 2ª Gerando os blocos e populando-os
     */
    
    int raw_it = 0;

    char** mensagem = (char**) calloc(qtd_mensagens+1, sizeof(char*));
    for(int i = 0; i < qtd_mensagens; i++){
        mensagem[i] = (char*) calloc(MAX_MESSAGE_LENGHT+1, sizeof(char));

        int msg_it = 0;
        while(raw_it < raw_str_len && msg_it < (MAX_MESSAGE_LENGHT - 1))
            mensagem[i][msg_it++] = raw_str[raw_it++];
        mensagem[i][msg_it] = '\n';
    }

    return mensagem;
}