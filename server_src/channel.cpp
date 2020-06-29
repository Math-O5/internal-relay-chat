#include "channel.h"

map<string, CHANNEL_conn*> channels;
map<string, CHANNEL_conn*>::iterator it_channel;
map<int, string>::iterator it_arrived;

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

int conn_destruit_CHANNEL(string name_channel) {  
     it_channel = channels.find(name_channel);

    // verifica se o canal existe
    if(it_channel != channels.end()) {
        free(it_channel);
        return SUCCESS;
    }   
    return FAIL;
}

/**
 * @function 
 * Adiciona um usuario ao canal. 
 *
 */
int CHANNEL_add_user(string name_channel, client* clt) {
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
    it_channel = channels.find(name_channel);

    // verifica se o canal existe
    if(it_channel != channels.end()) {

        // adiciona o participante e sua respectiva ordem de chegada
        it_channel->participants.erase(clt->nickname);

        // Remove da lista de ordem de chegada
        for(auto arrived :  it_channel->arrived) {
            if(arrived->second == clt->nickname) {
                 it_channel->arrived.erase(arrived);
                 break;
            }
        }

        return SUCCESS;
    }
    return FAIL;
}
    

int CHANNEL_kick_user(string name_channel, client* clt) {
    
    it_channel = channels.find(name_channel);
    
    // Se o canal existe, o usuario é adicionado da lista de kick.
    if(it_channel != channels.end() && !it_channel->notAllowedParticipants[clt->nickname]) {
       it_channel->notAllowedParticipants.insert(clt->nickname);
    }

    // exclui o usuario do canal
    return CHANNEL_remove_user(name_channel, clt);        
}

int CHANNEL_unkick_user(string name_channel, client* clt) {
    it_channel = channels.find(name_channel);
    
    // Se o canal existe, o usuario é removido da lista de kick.
    if(it_channel != channels.end()) {
       it_channel->notAllowedParticipants.erase(clt->nickname);
    }

    // exclui o usuario do canal
    return SUCCESS;
}

int CHANNEL_broadcast(client* clt);
int CHANNEL_commands();