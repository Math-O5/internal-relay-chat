#include "decodec.h"
#include "chat.h"
#include <stdlib.h>
#include <string>
#include <string.h>

int dcd_detectar_act(const char* cmd){

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
    int dcd_encode_connect(relay_chat* rc, const char* cmd, char* host, char* port){
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
    char* dcd_encode_nickname(relay_chat* rc, const char* cmd){
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

    char* dcd_encode_join(relay_chat* rc, const char* cmd){
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
    char** dcd_encode_client_message(relay_chat* rc, const char* raw_str, int raw_str_len){
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
    char* dcd_encode_mode(relay_chat* rc, const char* cmd){
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
    char* _dcd_encode_admin_pattern(relay_chat* rc, const char* cmd, const char* action){
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

    char* dcd_encode_invite(relay_chat* rc, const char* cmd){
        return _dcd_encode_admin_pattern(rc, cmd, "invite");
    }

    char* dcd_encode_whois(relay_chat* rc, const char* cmd){
       return _dcd_encode_admin_pattern(rc, cmd, "whois");
    }

    char* dcd_encode_mute(relay_chat* rc, const char* cmd){
        return _dcd_encode_admin_pattern(rc, cmd, "mute");
    }

    char* dcd_encode_unmute(relay_chat* rc, const char* cmd){
        return _dcd_encode_admin_pattern(rc, cmd, "unmute");
    }

    char* dcd_encode_kick(relay_chat* rc, const char* cmd){
        return _dcd_encode_admin_pattern(rc, cmd, "kick");
    }

    char* dcd_encode_unkick(relay_chat* rc, const char* cmd){
        return _dcd_encode_admin_pattern(rc, cmd, "unkick");
    }


// COMANDOS QUE ALTERAM ESTADO DO CLIENTE
    int dcd_decode_nickname(relay_chat* rc, const char* cmd, char* nickname){
        if(rc == NULL || cmd == NULL || nickname == NULL){
            return INVALID_PROTOCOL;
        }

        // Preparando as variáveis
        char temp_nick[NICKNAME_MAXLENGHT+1];
        char response[60];
        int  nro_itens;
        
        // Decodificando o comando
        nro_itens = sscanf(cmd, "/nickname %59s %30s", response, temp_nick);
        if(nro_itens != 2 || !is_valid_nickname(temp_nick)){
            return INVALID_PROTOCOL;
        }

        // Salvando o nickname na variavel auxiliar de retorno
        strcpy(nickname, temp_nick);

        // SUCCESS -> NICKNAME DEFINIDO COM SUCESSO
        if(strcmp(response,"SUCCESS") == 0){
            return SUCCESS;

        // ERR_NICKNAMEINUSE -> NICKNAME JÁ ESTÁ EM USO
        } else if(strcmp(response,"ERR_NICKNAMEINUSE") == 0){
            return ERR_NICKNAMEINUSE;

        // ERR_ERRONEUSNICKNAME -> NICKNAME INVÁLIDO
        } else if(strcmp(response,"ERR_ERRONEUSNICKNAME") == 0){
            return ERR_ERRONEUSNICKNAME;
        }

        return INVALID_PROTOCOL;
    }

    int dcd_decode_join(relay_chat* rc, const char* cmd, char* channel, bool* is_admin){
        if(rc == NULL || cmd == NULL || channel == NULL){
            return INVALID_PROTOCOL;
        }

        // Preparando as variáveis
        char temp_channel[CHANNEL_NAME_MAXLENGHT+1];
        char response[60];
        char params[MAX_MESSAGE_LENGHT+1];
        int  nro_itens;
        
        // Decodificando o comando
        nro_itens = sscanf(cmd, "/join %59s %30s %30s", response, temp_channel, params);
        if(nro_itens < 1){
            return INVALID_PROTOCOL;
        }

        // Salvando o nickname na variavel auxiliar de retorno
        strcpy(channel, temp_channel);

        // SUCCESS -> NICKNAME DEFINIDO COM SUCESSO
        if(strcmp(response,"SUCCESS") == 0 && nro_itens == 3){

            if( strcmp(params, "user") == 0 ){
                (*is_admin) = false;
            } else if( strcmp(params, "admin") == 0 ){
                (*is_admin) = true;
            } else {
                return INVALID_PROTOCOL;
            }

            return SUCCESS;

        // ERR_INVITEONLYCHAN -> USUARIO NAO FOI CONVIDADO PARA O CANAL
        } else if(strcmp(response,"ERR_INVITEONLYCHAN") == 0){
            return ERR_INVITEONLYCHAN;

        // ERR_BANNEDFROMCHAN -> USUARIO BANIDO DESTE CANAL
        } else if(strcmp(response,"ERR_BANNEDFROMCHAN") == 0){
            return ERR_BANNEDFROMCHAN;
        }

        return INVALID_PROTOCOL;
    }


// COMANDOS DO CANAL
    int dcd_decode_client_message(relay_chat* rc, const char* cmd, char* nickname, char* content) {
        if(rc == NULL || cmd == NULL || nickname == NULL || content == NULL){
            return INVALID_PROTOCOL;
        }

        // Preparando as variáveis
        char temp_nickname[CHANNEL_NAME_MAXLENGHT+1];
        char temp_content[MAX_MESSAGE_LENGHT+1];
        int  nro_itens;
        
        // Decodificando o comando
        nro_itens = sscanf(cmd, "/msg %30s :%500[^\n]", temp_nickname, temp_content);
        if(nro_itens != 2){
            return INVALID_PROTOCOL;
        }

        // Salvando o nickname na variavel auxiliar de retorno
        strcpy(nickname, temp_nickname);
        strcpy(content, temp_content);

        return SUCCESS;
    }

    int dcd_decode_list(relay_chat* rc, const char* cmd, char* channels) {
        if(rc == NULL || cmd == NULL || channels == NULL){
            return INVALID_PROTOCOL;
        }

        // Preparando as variáveis
        char temp_channels[CHANNEL_NAME_MAXLENGHT+1];
        int  nro_itens;
        
        // Decodificando o comando
        nro_itens = sscanf(cmd, "/list %500[^\n]", temp_channels);
        if(nro_itens != 1){
            return INVALID_PROTOCOL;
        }

        strcpy(channels, temp_channels);
        return SUCCESS;
    }


// COMANDOS ADMINISTRADOR
    int dcd_decode_mode(relay_chat* rc, const char* cmd) {
        if(rc == NULL || cmd == NULL){
            return INVALID_PROTOCOL;
        }

        // Preparando as variáveis
        char response[60];
        int  nro_itens;
        
        // Decodificando o comando
        nro_itens = sscanf(cmd, "/mode %s", response );
        
        if(nro_itens != 1){
            return INVALID_PROTOCOL;
        }

        // SUCCESS -> NICKNAME DEFINIDO COM SUCESSO
        if(strcmp(response,"SUCCESS") == 0){
            return SUCCESS;

        // ERR_CHANOPRIVSNEEDED -> NÃO É OPERADOR DO CANAL
        } else if(strcmp(response,"ERR_CHANOPRIVSNEEDED") == 0){
            return ERR_CHANOPRIVSNEEDED;
        }
        
        return INVALID_PROTOCOL;
    }

    int dcd_decode_whois(relay_chat* rc, const char* cmd, char* nickname, char* ip_info) {
        if(rc == NULL || cmd == NULL || nickname == NULL || ip_info == NULL){
            return INVALID_PROTOCOL;
        }

        // Preparando as variáveis
        char response[60];
        char temp_nickname[NICKNAME_MAXLENGHT+1], temp_ip_info[60];
        int  nro_itens;
        
        // Decodificando o comando
        nro_itens = sscanf(cmd, "/whois %s %s %s", response, temp_nickname, temp_ip_info);
        
        if(nro_itens < 1){
            return INVALID_PROTOCOL;
        }

        strcpy(nickname, temp_nickname);
        strcpy(ip_info, temp_ip_info);

        // SUCCESS -> NICKNAME DEFINIDO COM SUCESSO
        if(strcmp(response,"SUCCESS") == 0 && nro_itens == 3){
            return SUCCESS;

        // ERR_NOSUCHNICK -> NINGUEM POSSUI O NICKNAME ESPECIFICADO
        } else if(strcmp(response,"ERR_NOSUCHNICK") == 0){
            return ERR_NOSUCHNICK;

        // ERR_CHANOPRIVSNEEDED -> NÃO É OPERADOR DO CANAL
        } else if(strcmp(response,"ERR_CHANOPRIVSNEEDED") == 0){
            return ERR_CHANOPRIVSNEEDED;
        }
        
        return INVALID_PROTOCOL;
    }

    int _dcd_decode_admin_pattern(relay_chat* rc, const char* cmd, char* nickname) {
        if(rc == NULL || cmd == NULL || nickname == NULL){
            return INVALID_PROTOCOL;
        }

        // Preparando as variáveis
        char response[60];
        char temp_nickname[NICKNAME_MAXLENGHT+1];
        int  nro_itens;
        
        // Decodificando o comando
        nro_itens = sscanf(cmd, "/%*s %s %s", response, temp_nickname );
        
        if(nro_itens < 1){
            return INVALID_PROTOCOL;
        }

        strcpy(nickname, temp_nickname);

        // SUCCESS -> NICKNAME DEFINIDO COM SUCESSO
        if(strcmp(response,"SUCCESS") == 0 && nro_itens == 2){
            return SUCCESS;

        // ERR_NOSUCHNICK -> NINGUEM POSSUI O NICKNAME ESPECIFICADO
        } else if(strcmp(response,"ERR_NOSUCHNICK") == 0){
            return ERR_NOSUCHNICK;

        // ERR_CHANOPRIVSNEEDED -> NÃO É OPERADOR DO CANAL
        } else if(strcmp(response,"ERR_CHANOPRIVSNEEDED") == 0){
            return ERR_CHANOPRIVSNEEDED;

        }
        
        return INVALID_PROTOCOL;
    }

    int dcd_decode_invite(relay_chat* rc, const char* cmd, char* nickname) {
        return  _dcd_decode_admin_pattern(rc, cmd, nickname);
    }

    int dcd_decode_mute(relay_chat* rc, const char* cmd, char* nickname) {
        return  _dcd_decode_admin_pattern(rc, cmd, nickname);
    }

    int dcd_decode_unmute(relay_chat* rc, const char* cmd, char* nickname) {
        return  _dcd_decode_admin_pattern(rc, cmd, nickname);
    }

    int dcd_decode_kick(relay_chat* rc, const char* cmd, char* nickname) {
        return  _dcd_decode_admin_pattern(rc, cmd, nickname);
    }

    int dcd_decode_unkick(relay_chat* rc, const char* cmd, char* nickname) {
        return  _dcd_decode_admin_pattern(rc, cmd, nickname);
    }


// MENSAGEMS SERVER -> CLIENT
    int _dcd_decode_X_message(relay_chat* rc, const char* cmd,  char* content) {
        if(rc == NULL || cmd == NULL || content == NULL){
            return INVALID_PROTOCOL;
        }

        // Preparando as variáveis
        char temp_content[MAX_MESSAGE_LENGHT+1];
        int  nro_itens;
        
        // Decodificando o comando
        nro_itens = sscanf(cmd, "/%*s :%500[^\n]", temp_content);
        if(nro_itens != 1){
            return INVALID_PROTOCOL;
        }

        // Salvando o nickname na variavel auxiliar de retorno
        strcpy(content, temp_content);

        return SUCCESS;
    }
    
    int dcd_decode_server_message(relay_chat* rc, const char* cmd,  char* content) {
       return _dcd_decode_X_message( rc, cmd, content);
    }

    int dcd_decode_channel_message(relay_chat* rc, const char* cmd,  char* content) {
        return _dcd_decode_X_message( rc, cmd, content);
    }

// FUNÇÕES DE VALIDAÇÃO
    int is_valid_channel_name(char* name){
        return 1;
    }

    int is_valid_nickname(char* name){
        return 1;
    }