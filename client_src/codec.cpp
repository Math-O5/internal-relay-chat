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

    // COMANDOS GERAIS
    if ( strstr(cmd, "/connect") == cmd ){
        action_code = ACTION_CONNECT;
    
    } else if ( strstr(cmd, "/disconnect") == cmd ) {
        action_code = ACTION_DISCONNECT;
    
    } else if ( strstr(cmd, "/quit") == cmd ) {
        action_code = ACTION_QUIT;
    
    } else if ( strstr(cmd, "/help") == cmd ) {
        action_code = ACTION_HELP;

    } else if ( strstr(cmd, "/ping") == cmd ) {
        action_code = ACTION_PING;
    }

    // COMANDOS QUE ALTERAM ESTADO DO CLIENTE 
    else if ( strstr(cmd, "/nickname") == cmd ) {
        action_code = ACTION_NICK;

    } else if ( strstr(cmd, "/join") == cmd ) {
        action_code = ACTION_JOIN;
    }
    
    // COMANDOS ADMINISTRADOR
    else if ( strstr(cmd, "/mode") == cmd ) {
        action_code = ACTION_MODE;

    } else if ( strstr(cmd, "/invite") == cmd ) {
        action_code = ACTION_INVITE;

    }  else if ( strstr(cmd, "/whois") == cmd ) {
        action_code = ACTION_WHOIS;

    } else if ( strstr(cmd, "/mute") == cmd ) {
        action_code = ACTION_MUTE;
        
    }  else if ( strstr(cmd, "/unmute") == cmd ) {
        action_code = ACTION_UNMUTE;

    } else if ( strstr(cmd, "/kick") == cmd ) {
        action_code = ACTION_KICK;
    } 
    
    // COMANDOS ESPECIAIS SERVIDOR >> CLIENTE
    else if ( strstr(cmd, "/servermsg") == cmd ) {
        action_code = ACTION_SERVERMSG;

    } else if ( strstr(cmd, "/channelmsg") == cmd ) {
        action_code = ACTION_CHANNELMSG;
    }  

    // COMANDO DO CANAL
    else if ( strstr(cmd, "/list") == cmd ) {
        action_code = ACTION_LIST;

    } else if (cmd[0] != '/') {             // Obs: (cmd[0] != '/') impede de enviar 
        action_code = ACTION_MESSAGE;       //      um comando como mensagem sem querer.
    }

    return action_code;
}

// COMANDOS GERAIS
    int cdc_encode_connect(relay_chat* rc, const char* cmd, char* host, char* port){
        if(cmd == NULL || host == NULL || port == NULL){
            return 1;
        }

        char temp_host[120]; 
        char temp_port[120];

        int nro_itens = sscanf(cmd, "/connect %119s %119s", temp_host, temp_port);

        if(nro_itens >= 1) {
            strcpy(host, temp_host);
        } else {
            strcpy(host, DEFAULT_SERVER_HOST);
        }

        if(nro_itens >= 2) {
            strcpy(port, temp_port);
        } else {
            strcpy(port, DEFAULT_SERVER_PORT);
        }

        return 0;
    }

// COMANDOS QUE ALTERAM ESTADO DO CLIENTE 
    char* cdc_encode_nickname(relay_chat* rc, const char* cmd){
        if(cmd == NULL){
            return NULL;
        }

        char nick[NICKNAME_MAXLENGHT+1];
        int nro_itens = sscanf(cmd, "/nickname %30s", nick);

        if(nro_itens != 1 || !is_valid_nickname(nick)){
            return NULL;
        }

        char* nick_message = (char*) calloc(MAX_MESSAGE_LENGHT, sizeof(char));
        sprintf(nick_message, "/nickname %s\n", nick);
        
        return nick_message;
    }

    char* cdc_encode_join(relay_chat* rc, const char* cmd){
        if(cmd == NULL){
            return NULL;
        }

        char channel[CHANNEL_NAME_MAXLENGHT+1];
        int nro_itens = sscanf(cmd, "/join %30s", channel); //Obs: 30 == CHANNEL_NAME_MAXLENGHT

        if(nro_itens != 1 || !is_valid_channel_name(channel)){
            return NULL;
        }

        char* join_message = (char*) calloc(MAX_MESSAGE_LENGHT, sizeof(char));
        sprintf(join_message, "/join %s\n", channel);
        
        return join_message;
    }

// COMANDOS DO CANAL
    char** cdc_encode_client_message(relay_chat* rc, const char* raw_str, int raw_str_len){
        if(raw_str == NULL || raw_str_len <= 0){
            return NULL;
        }

        /** 1ª Etapa - Calculando quantos pacotes serão necessários
         *  
         * Formato de uma mensagem:
         * /msg <nickname> :<mensagem>\n
         * 
         * overhead padrão: 5 + 1 + (#/@)strlen(nickname) + 2 + 1 = 9 + strlen(nickname)
         * 
         * 
         * Obs: (MAX_MESSAGE_LENGHT-1) é necessário porque temos que descontar o espaço 
         * consumido pelo \n que é adicionado ao fim de cada mensagem do protocolo.
         */
        
        int message_overhead = 9 + rc->nick_len;
        int qtd_mensagens  =  raw_str_len / (MAX_MESSAGE_LENGHT - message_overhead); 
        qtd_mensagens     += (raw_str_len % (MAX_MESSAGE_LENGHT - message_overhead) > 0) ? 1 : 0;

        /**
         * 2ª Gerando os blocos e populando-os
         */
        
        int raw_it = 0;

        char** mensagem = (char**) calloc(qtd_mensagens+1, sizeof(char*));
        for(int i = 0; i < qtd_mensagens; i++){
            // Cria a mensagem
            mensagem[i] = (char*) calloc(MAX_MESSAGE_LENGHT+1, sizeof(char));

            // Colocando o comando, simbolo do usuario e nickname
            char symbol = (rc->is_admin) ? CHANNEL_ADMIN_SIMBOL : CHANNEL_USER_SIMBOL;
            sprintf(mensagem[i], "/msg %c%s :", symbol, rc->nickname);

            // int msg_it = 0;
            int msg_it = 8 + rc->nick_len;
            while(raw_it < raw_str_len && msg_it < (MAX_MESSAGE_LENGHT - 1))
                mensagem[i][msg_it++] = raw_str[raw_it++];
            mensagem[i][msg_it] = '\n';
        }

        return mensagem;
    }

// COMANDOS ADMINISTRADOR
    char* cdc_encode_mode(relay_chat* rc, const char* cmd){
        if(cmd == NULL){
            return NULL;
        }

        char mode[15]; 
        int nro_itens = sscanf(cmd, "/mode %15s", mode); //Obs: 30 == CHANNEL_NAME_MAXLENGHT

        if(nro_itens != 1 || (strcmp(mode,"+i") != 0 && strcmp(mode,"-i") != 0)){
            return NULL;
        }

        char* mode_message = (char*) calloc(MAX_MESSAGE_LENGHT, sizeof(char));
        sprintf(mode_message, "/mode %s\n", mode);
        
        return mode_message;
    }
    
    // todas as funções de administrador abaixo possui o mesmo padrão, então 
    // encapsulei todas em uma função genérica de uso local.
    char* _cdc_encode_admin_pattern(relay_chat* rc, const char* cmd, const char* action){
        if(cmd == NULL){
            return NULL;
        }

        char nick[NICKNAME_MAXLENGHT+1];
        int  nro_itens = sscanf(cmd, "%*s %30s ", nick);

        if(nro_itens != 1 || !is_valid_nickname(nick)){
            return NULL;
        }

        char* nick_message = (char*) calloc(MAX_MESSAGE_LENGHT, sizeof(char));
        sprintf(nick_message, "/%s %s\n", action, nick);
        
        return nick_message;
    }

    char* cdc_encode_invite(relay_chat* rc, const char* cmd){
        return _cdc_encode_admin_pattern(rc, cmd, "invite");
    }

    char* cdc_encode_whois(relay_chat* rc, const char* cmd){
       return _cdc_encode_admin_pattern(rc, cmd, "whois");
    }

    char* cdc_encode_mute(relay_chat* rc, const char* cmd){
        return _cdc_encode_admin_pattern(rc, cmd, "mute");
    }

    char* cdc_encode_unmute(relay_chat* rc, const char* cmd){
        return _cdc_encode_admin_pattern(rc, cmd, "unmute");
    }

    char* cdc_encode_kick(relay_chat* rc, const char* cmd){
        return _cdc_encode_admin_pattern(rc, cmd, "kick");
    }

    char* cdc_encode_unkick(relay_chat* rc, const char* cmd){
        return _cdc_encode_admin_pattern(rc, cmd, "unkick");
    }

int is_valid_channel_name(char* name){
    return 1;
}

int is_valid_nickname(char* name){
    return 1;
}