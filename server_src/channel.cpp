#include "channel.h"

map<string, CHANNEL_conn*> channels;
map<string, CHANNEL_conn*>::iterator it_channel;
    
/**
 * @function Cria um novo canal. O novo canal não pode existir.
 */
int conn_criar_CHANNEL(string name_channel, client* clt) {        
    
    int size_participants = 1; // há apenas um usuario: o admin;

    if(channels.find(name_channel) == channels.end()) {
        
        CHANNEL_conn* newChannel = (CHANNEL_conn*) malloc(sizeof(CHANNEL_conn));
        newChannel->participants[clt->nickname] = clt->cl_socket;
        newChannel->arrived[size_participants] = clt->nickname;
        channels[name_channel] = newChannel;
        
        return SUCCESS;
    }
    return ERROR_CREATE_CHANNEL;
}

/**
 * @function 
 * Adiciona um usuario ao canal. 
 *
 */
CHANNEL_add_user(string name_channel, client* clt) {
    it_channel = channels.find(name_channel);

    // verifica se o canal existe
    if(it_channel != channels.end()) {

        //extrae o valor dor ultimo participante
        int last = it_channel->rbegin()->first + 1;
      
        // Check if the user is banned
        if(it_channel->notAllowedParticipants[clt->nickname]) {
            return ERROR_USER_BANNED;
        }

        // adiciona o participante e sua respectiva ordem de chegada
        it_channel->participants[clt->nickname] = clt->cl_socket;  
        it_channel->arrived[last] = clt->nickname;

        return SUCCESS;
    }
    return FAIL;
}

CHANNEL_remove_user(string name_channel, client* clt) {

}
    

    CHANNEL_kick_user(client* clt);
    CHANNEL_ban_user(client* clt);
    CHANNEL_broadcast(client* clt);
    CHANNEL_commands();