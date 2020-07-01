#include "decodec.h"
#include <stdlib.h>
#include <string>
#include <string.h>

// @Comentários em "decodec.h"
int detect_action(const char* cmd){

    int action_code = ACTION_NONE;

    /**
     * strstr(cmd, "/COMMAND") == cmd
     * 
     * O comando só é valido se o usuário iniciar a mensagem
     * com /, caso contrário será sempre considerado uma mensagem.
     */

    // COMANDOS GERAIS
    if ( strstr(cmd, "/disconnect") == cmd ) {
        action_code = ACTION_DISCONNECT;
    
    } else if ( strstr(cmd, "/quit") == cmd ) {
        action_code = ACTION_QUIT;
    
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

    } else if ( strstr(cmd, "/unkick") == cmd ) {
        action_code = ACTION_UNKICK;
    } 
    
    // COMANDO DO CANAL
    else if ( strstr(cmd, "/list") == cmd ) {
        action_code = ACTION_LIST;

    } else if (strstr(cmd, "/msg") == cmd || cmd[0] != '/') {             // Obs: (cmd[0] != '/') impede de enviar 
        action_code = ACTION_MESSAGE;                                      // um comando como mensagem sem querer.
    }

    return action_code;
}

// @Comentários em "decodec.h"
void decode_nickname(const char* cmd, char* nickname){

    sscanf(cmd, "%*s %50s", nickname); 
}

// @Comentários em "decodec.h"
void decode_join(const char* cmd, char* channel){

    sscanf(cmd, "%*s %s", channel);
}

// @Comentários em "decodec.h"
void decode_mode(const char* cmd, bool* invite_mode){

    char mode_status[5];

    sscanf(cmd, "%*s %s", mode_status);

    (*invite_mode) = strcmp(mode_status, "+i") == 0 ? INVITE_ONLY_ON : INVITE_ONLY_OFF;
}

// @Comentários em "decodec.h"
void decode_invite(const char* cmd, char* invite_nickname){

    sscanf(cmd, "%*s %50s", invite_nickname);
}

// @Comentários em "decodec.h"
void decode_whois(const char* cmd, char* whois_nickname){

    sscanf(cmd, "%*s %50s", whois_nickname);
}

// @Comentários em "decodec.h"
void decode_mute(const char* cmd, char* mute_nickname){

    sscanf(cmd, "%*s %s", mute_nickname);
}

// @Comentários em "decodec.h"
void decode_unmute(const char* cmd, char* unmute_nickname){

    sscanf(cmd, "%*s %s", unmute_nickname);
}

// @Comentários em "decodec.h"
void decode_kick(const char* cmd, char* kick_nickname){

    sscanf(cmd, "%*s %s", kick_nickname);  
}

// @Comentários em "decodec.h"
void decode_unkick(const char* cmd, char* unkick_nickname){

    sscanf(cmd, "%*s %s", unkick_nickname);  
}

// @Comentários em "decodec.h"
void decode_msg(const char* cmd, char* role, char* nickname, char* message_content){

    sscanf(cmd, "%*s %c%s : %[^\n]", &(*role), nickname, message_content);
}