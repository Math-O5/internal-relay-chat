#include "channel.h"

// nickname_channel and channel pointer
map<const char*, CHANNEL_conn*, cmp_str> channels;
map<const char*, CHANNEL_conn*, cmp_str>::iterator it_channel;
map<int, char*>::iterator it_arrived;

char emptychar[] = "";

enum MESSAGE {
    MESSAGE_CLIENT, MESSAGE_NEW_JOIN_CHANNEL, MESSAGE_JOIN_CHANNEL, MESSAGE_NEW_ADMIN_CHANNEL,
    MESSAGE_TOPIC, MESSAGE_QUIT_CHANNEL, MESSAGE_ERR_INVITEONLYCHAN, MESSAGE_ERR_BANNEDFROMCHAN,
    MESSAGE_KICK_CHANNEL, MESSAGE_UNKICK_CHANNEL, MESSAGE_KICK_ERR_NOSUCHNICK, MESSAGE_KICK_ERR_CHANOPRIVSNEEDED,
    MESSAGE_MUTE_CHANNEL, MESSAGE_UNMUTE_CHANNEL, MESSAGE_MUTE_ERR_NOSUCHNICK, MESSAGE_MUTE_ERR_CHANOPRIVSNEEDED,
    MESSAGE_UNMUTE_ERR_NOSUCHNICK, MESSAGE_UNMUTE_ERR_CHANOPRIVSNEEDED, MESSAGE_UNKICK_ERR_NOSUCHNICK, MESSAGE_UNKICK_ERR_CHANOPRIVSNEEDED,
    MESSAGE_WHOIS_ERR_NOSUCHNICK, MESSAGE_WHOIS_CHANNEL, MESSAGE_WHOIS_ERR_CHANOPRIVSNEEDED,
    MESSAGE_MODE_CHANNEL, MESSAGE_MODE_ERR_CHANOPRIVSNEEDED, MESSAGE_CHANGE_NICKNAME,
};

void map_insert_key_char(map<const char*, int, cmp_str> pointer, const char* str, int value) {
    char* strCopy = (char*)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(strCopy, str);
    pointer.insert(pair<char*, int>(strCopy, value));
}

void map_insert_key_int(map<int, char*> pointer, int value, const char* str) {
    char* strCopy = (char*)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(strCopy, str);
    pointer.insert(pair<int, char*>(value, strCopy));
}

/**
 * @function conn_criar_CHANNEL
 * 
 * Cria canal com as propriedades iniciais necessarias.
 * 
 * @param { char* } nickname_channel : nome do canal em que ação acontece
 * @param { client[] } clt : client que será removido.
 * @permision: self user | admin 
 * @return { SUCESS | FAIL }
 * 
 * Atualiza a referencia do cliente para o canal.
 * 
 */
CHANNEL_conn* conn_criar_CHANNEL(char* nickname_channel, client* clt) 
{        
    int size_participants = 1; // há apenas um usuario: o admin;

    // Verifica se o canal não existe.
    if(channels.find(nickname_channel) == channels.end()) {
        
        CHANNEL_conn* newChannel = (CHANNEL_conn*) malloc(sizeof(CHANNEL_conn));
        newChannel->nickname_admin = (char*) malloc(sizeof(char)*MAX_SIZE_NAME);
        newChannel->nickname_channel = (char*) malloc(sizeof(char)*MAX_SIZE_NAME);
        map_insert_key_char(newChannel->participants, clt->nickname, clt->cl_socket);
        map_insert_key_int(newChannel->arrived, 1, clt->nickname);
        strcpy(newChannel->nickname_admin, clt->nickname);
        strcpy(newChannel->nickname_channel, nickname_channel);
        newChannel->is_public = true;
        channels[nickname_channel] = newChannel;

        clt->channel = newChannel;

        return newChannel;
    }
    return NULL;
}

/**
 * @function conn_destruit_CHANNEL
 * 
 * Excluir o canal (limpa a memória)
 * @param { CHANNEL_conn* } channel : referencia do canal em que ação acontece
 * @permision: admin 
 * 
 */
void CHANNEL_destroy(CHANNEL_conn* channel) 
{
    channels.erase(channel->nickname_channel);
    free(channel);
}

/**
 * @function conn_destruit_CHANNELS
 * 
 * Exclui TODOS os canais
 * @permision: server 
 * 
 */
void CHANNEL_destroy_all() 
{
    for(auto channel : channels) {
        free(channel.second);
    }
    channels.clear();
}

/**
 * @function CHANNEL_remove_user
 * 
 * Exclui o usuário do canal e o proibe de entrar novamente.
 * @param { char* } nickname_channel : nome do canal em que ação acontece
 * @param { client[] } clt : client que será removido.
 * @permision: self user | admin 
 * @return { SUCESS | FAIL }
 * 
 */
int CHANNEL_remove_user(CHANNEL_conn* channel, struct _client* clt) 
{
    if(channel == NULL) return FAIL;

    // limpa a referencia do canal do cliente.
    clt->channel = NULL;
    
    // adiciona o participante e sua respectiva ordem de chegada
    channel->participants.erase(clt->nickname);

    // Remove da lista de ordem de chegada
    for(it_arrived = channel->arrived.begin(); it_arrived != channel->arrived.end(); ++it_channel) 
    {
        if(strcmp(it_arrived->second, clt->nickname)) 
        {
            channel->arrived.erase(it_arrived);
        }
    }

    // se há participantes no canal
    if(!channel->participants.empty()) 
    {
        if(strcmp(channel->nickname_admin, clt->nickname)) {
            char* newAdmin = channel->arrived.begin()->second;
            clt = clt_get_by_nickname(newAdmin);
            CHANNEL_broadcast(channel, clt, MESSAGE_NEW_ADMIN_CHANNEL, emptychar);
        }                
    }
    // se não tem nenhum participante no cai, ele deve ser excluido 
    else
    {
        CHANNEL_destroy(channel);
    }

    return SUCCESS;
}

/**
 * @function CHANNEL_remove_user
 * 
 * Exclui o usuário do canal e o proibe de entrar novamente.
 * @param { char* } nickname_channel : nome do canal em que ação acontece
 * @param { client[] } clt : client que será removido.
 * @permision: self user | admin 
 * @return { SUCESS | FAIL }
 * 
 */
int CHANNEL_add_user(CHANNEL_conn* channel, client* clt) 
{
    if(channel == NULL) return FAIL;

    //extrae o valor da posição de chegada do ultimo participante
    int last = channel->arrived.rbegin()->first + 1;
    printf("************posicao %d **********\n", last);

    // Check if the user is banned
    if(channel->notAllowedParticipants.find(clt->nickname) != channel->notAllowedParticipants.end()) 
    {
        return MESSAGE_ERR_BANNEDFROMCHAN;
    }

    // TODO: check invite permission 

    // adiciona o participante e sua respectiva ordem de chegada
    channel->participants[clt->nickname] = clt->cl_socket;  
    channel->arrived[last] = clt->nickname;

    clt->channel = channel;

    return SUCCESS;
}

void CHANNEL_list(client* clt) 
{
    char buffer[BUFFER_SIZE];
    char espaco_virgula[5];
    strcpy(espaco_virgula, ", ");
    
    for(auto channel : channels) 
    {
        strcat(buffer, channel.first);
        strcat(buffer, espaco_virgula);
    }

    clt_send_message(clt->cl_socket, buffer);
    return;
}

/**
 * @function CHANNEL_on_change_nickname
 * @param { client* } é a referencia para os dados do cliente.
 * @param { char* } é o nome anterior
 */
void CHANNEL_on_change_nickname(client* clt, const char* old_nickname) {
    
    if(clt->channel == NULL) 
        return;
    
    // TODO: Quando a chave para o ipv4
    CHANNEL_broadcast(clt->channel, clt, MESSAGE_CHANGE_NICKNAME, old_nickname);
}

/**
 * @function CHANNEL_remove_user
 * 
 * Exclui o usuário do canal e o proibe de entrar novamente.
 * @param { char* } nickname_channel : nome do canal em que ação acontece
 * @param { client[] } clt : client que será removido.
 * @permision: self user | admin 
 * @return { SUCESS | FAIL }
 * 
 */
void CHANNEL_join(char* nickname_channel, client* clt) 
{    
    CHANNEL_conn* channel;

    // Se o usuário já estiver em um canal, ele é retirado dele.
    if(clt->channel != NULL) 
    {
        it_channel = channels.find(channel->nickname_channel);
        CHANNEL_remove_user(clt->channel, clt);
        CHANNEL_broadcast(it_channel->second, clt, MESSAGE_QUIT_CHANNEL, emptychar);
    }

    it_channel = channels.find(nickname_channel);

    // // Se o canal não existe, então será criado e o criador é admin
    if(it_channel == channels.end()) 
    {
        channel = conn_criar_CHANNEL(nickname_channel, clt);
        CHANNEL_broadcast(channel, clt, MESSAGE_NEW_JOIN_CHANNEL, emptychar);
    } 
    else 
    {
    //     int response = CHANNEL_add_user(it_channel->second, clt);
        printf("oops!");
        fflush(stdout);
        
    //     switch (response)
    //     {
    //         case SUCCESS:
    //             CHANNEL_broadcast(it_channel->second, clt, MESSAGE_JOIN_CHANNEL, emptychar);
    //             break;
    //         case MESSAGE_ERR_BANNEDFROMCHAN:
    //             CHANNEL_broadcast(it_channel->second, clt, MESSAGE_ERR_BANNEDFROMCHAN, emptychar);
    //             break;
    //         // case MESSAGE_ERR_INVITEONLYCHAN:
    //         //     CHANNEL_broadcast(it_channel, clt, MESSAGE_MESSAGE_ERR_INVITEONLYCHAN, "");
    //         //     break;
    //         default:
    //             break;
    //     }
    }
}

void CHANNEL_invite(CHANNEL_conn* channel, client* clt, const char* kick_nickname) 
{

    return;
}

/**
 * @function CHANNEL_kick_user
 * 
 * Exclui o usuário do canal e o proibe de entrar novamente.
 * @param { char* } nickname_channel : nome do canal em que ação acontece
 * @param { client[] } clt : client que será banido.
 * @permision: admin 
 * @return { SUCESS | FAIL }
 * 
 */
void CHANNEL_kick_user(CHANNEL_conn* channel, client* clt, const char* kick_nickname) 
{   
    if(channel == NULL) return;
    
    if(channel->participants[kick_nickname] == 0) 
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_KICK_ERR_NOSUCHNICK, emptychar);
    }

    // apenas admin
    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {   
        // Se o canal existe, o usuario é adicionado da lista de kick. Obs. sets não armazena valores duplicados.
        channel->notAllowedParticipants.insert(kick_nickname);
        
        client* ban = clt_get_by_nickname(kick_nickname);
        
        // exclui o usuario do canal
        CHANNEL_remove_user(channel, ban);        
        CHANNEL_broadcast(channel, clt, MESSAGE_KICK_CHANNEL, kick_nickname);

    } else {
        CHANNEL_broadcast(channel, clt, MESSAGE_KICK_ERR_CHANOPRIVSNEEDED, kick_nickname);
    }
}

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { char* } nickname_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
void CHANNEL_unkick_user(CHANNEL_conn* channel, client* clt, const char* unkick_nickname) 
{   
    if(channel == NULL) return;
    
    if(channel->participants[unkick_nickname] != 0) 
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_UNKICK_ERR_NOSUCHNICK, emptychar);
    }
    
    // apenas admin
    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {
        // Se o canal existe, o usuario é retirado da lista de kick.
        channel->notAllowedParticipants.erase(clt->nickname);
        CHANNEL_broadcast(channel, clt, MESSAGE_UNKICK_CHANNEL, unkick_nickname);
    } else {
        CHANNEL_broadcast(channel, clt, MESSAGE_UNKICK_ERR_CHANOPRIVSNEEDED, unkick_nickname);
    }
}

/**
 * @function CHANNEL_mute_user
 * 
 * Silencia o usuário do canal e o proibe de enviar mensagem.
 * @param { char* } nickname_channel : nome do canal em que ação acontece
 * @param { client[] } clt : client que será banido.
 * @permision: admin 
 * 
 */
void CHANNEL_mute_user(CHANNEL_conn* channel, client* clt, const char* mute_nickname) 
{   
    if(channel == NULL) return;
    
    // Se o usuario não está no canal.
    if(channel->participants[mute_nickname] == 0) 
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_MUTE_ERR_NOSUCHNICK, emptychar);
        return;
    }

    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {   
        // Se o canal existe, o usuario é adicionado da lista de kick. Obs. sets não armazena valores duplicados.
        channel->mutedParticipants.insert(mute_nickname);              
        CHANNEL_broadcast(channel, clt, MESSAGE_MUTE_CHANNEL, mute_nickname);
    } else 
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_MUTE_ERR_CHANOPRIVSNEEDED, mute_nickname);
    }
}

/**
 * @function CHANNEL_unmute_user
 * 
 * Silencia o usuário do canal e o proibe de enviar mensagem.
 * @param { char* } nickname_channel : nome do canal em que ação acontece
 * @param { client[] } clt : client que será banido.
 * @permision: admin 
 * 
 */
void CHANNEL_unmute_user(CHANNEL_conn* channel, client* clt, const char* unmute_nickname) 
{   
    if(channel == NULL) return;
    
    if(channel->participants[unmute_nickname] == 0)                   // Se o usuario não está no canal.
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_UNMUTE_ERR_NOSUCHNICK, emptychar);
        return;
    }

    if(!strcmp(clt->nickname, channel->nickname_admin))  
    {   
        channel->mutedParticipants.erase(unmute_nickname);            // Se o canal existe, o usuario é apagado da lista de kick. Obs. sets não armazena valores duplicados.
        CHANNEL_broadcast(channel, clt, MESSAGE_UNMUTE_CHANNEL, unmute_nickname);
    } else
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_UNMUTE_ERR_CHANOPRIVSNEEDED, unmute_nickname);
    }
}

void CHANNEL_whois(CHANNEL_conn* channel, client* clt, char* whois) 
{
    if(channel == NULL) return;
    
    if(channel->participants[whois] == 0)                   // Se o usuario não está no canal.
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_WHOIS_ERR_NOSUCHNICK, emptychar);
        return;
    }

    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {   
        const char espace[] = " ";
        
        channel->mutedParticipants.erase(whois);            // Se o canal existe, o usuario é apagado da lista de kick. Obs. sets não armazena valores duplicados.
        
        client* clt = clt_get_by_nickname(whois);
        strcat(whois, espace);
        strcat(whois, clt->ip_address);

        CHANNEL_broadcast(channel, clt, MESSAGE_WHOIS_CHANNEL, whois);
    } else
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_WHOIS_ERR_CHANOPRIVSNEEDED, whois);
    }
}   

void CHANNEL_mode(CHANNEL_conn* channel, client* clt, bool is_public) {
    if(channel == NULL) return;

    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {   
        char privacity[] = "publico";
        
        if(!is_public) 
            strcpy(privacity, "privado");
        
        channel->is_public = is_public;


        CHANNEL_broadcast(channel, clt, MESSAGE_MODE_CHANNEL, privacity);
    } else
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_MODE_ERR_CHANOPRIVSNEEDED, emptychar);
    }   
}

/** Messagens **/
/** **/
bool CHANNEL_send_message(int cl_socket, const char* buffer) 
{
    int try_send = 0;

    // Tenta enviar a mensagem até receber confirmação. Se após 
    while(send(cl_socket, buffer, strlen(buffer), 0) < 0 && try_send < 6) 
    {
        try_send += 1;
    }
    return (try_send == 6) ? false : true;
}

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { char* } nickname_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
void CHANNEL_send_message_all(CHANNEL_conn* channel, const char* buffer)
{
    // Envio da mensagem aos cliente em broadcast 
    for(auto client : channel->participants) 
    {
        // TODO: split buffer

        // Tenta enviar a mensagem
        if(clt_send_message(client.second, buffer)) 
        {
            //msg_send_cliente(channel->nickname_channel.c_str(), client.first);
        }
        // Se o usuario não está respondendo é desconectado do canal 
        else 
        {
            // client* clt = clt_get_by_nickname(client.first);
            // CHANNEL_remove_user(channel, clt);
            // msg_client_no_response(client->first);
        } 
    }
}

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { char* } nickname_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
void CHANNEL_send_message_one(CHANNEL_conn* channel, client* clt, const char* buffer)
{
    // Tenta enviar a mensagem
    if(CHANNEL_send_message(clt->cl_socket , buffer)) 
    {
        //msg_send_cliente(channel->nickname_channel, clt->nickname);
    }
    // Se o usuario não está respondendo é desconectado do canal 
    else 
    {
        //msg_client_no_response(clt->nickname);
        CHANNEL_remove_user(channel, clt);
    } 
}

void CHANNEL_recv_message(CHANNEL_conn* channel, client* clt, int cmd, const char* buffer) {

}

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { char* } nickname_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
int CHANNEL_broadcast(CHANNEL_conn* channel, struct _client* clt, int type, const char* buffer) 
{
    char msg_buffer[BUFFER_SIZE];
    char role;

    // Configuracao da mensagem a ser enviada (adiciona que enviou a mensagem no conteudo)
    switch (type) 
    {
        case MESSAGE_CLIENT:    // Formatação da mensagem do cliente.
            if(channel->nickname_admin == clt->nickname)    
                role = '@';
            else role = '#';

            // sprintf(msg_buffer, "/msg %c%s : %s\n", role, clt->nickname, buffer);
            break;

        case MESSAGE_CHANGE_NICKNAME:
            if(!strcmp(clt->nickname, channel->nickname_admin)) 
                role = '@';
            else role = '#';

            sprintf(msg_buffer, "/channelmsg o %c%s : %s\n", role, clt->nickname, buffer);  
            CHANNEL_send_message_all(channel, msg_buffer);
            break;

        case MESSAGE_NEW_JOIN_CHANNEL:          // Mensagem para quem criou o canal.
            sprintf(msg_buffer, "/join SUCCESS %s role:admin.\n", channel->nickname_channel);
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            sprintf(msg_buffer, "/join RPL_NAMREPLY %s +%s", channel->nickname_channel, clt->nickname);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_JOIN_CHANNEL:          // Mensagem padrão dos usuários que acabaram de entrar no canal.
            if(!strcmp(clt->nickname, channel->nickname_admin))  
                role = '@';
            else role = '#';   

            sprintf(msg_buffer, "/join SUCCESS %s role:user.\n", channel->nickname_channel);
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            // a lista ordenada pela ordem de chegada, sem considerar o admin.
            sprintf(msg_buffer, "/join RPL_NAMREPLY %s +%s\n", channel->nickname_channel, clt->nickname);

            // TODO: avoid overflow buffer
            it_arrived = ++channel->arrived.begin();
            while(it_arrived != channel->arrived.end()) {
                strcpy(msg_buffer, " @");
                strcpy(msg_buffer, it_arrived->second);
                ++it_arrived;
            }
            
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            // mensagem para todos no canal
            // sprintf(msg_buffer, "/channelmsg : %c%s conectou-se.\n", role, clt->nickname);
            CHANNEL_send_message_all(channel, msg_buffer);

            break;

        case MESSAGE_NEW_ADMIN_CHANNEL:         // O novo admin do canal é
            if(!strcmp(clt->nickname, channel->nickname_admin)) 
                role = '@';
            else role = '#';   

            sprintf(msg_buffer, "/join SUCCESS %s role:admin.\n", channel->nickname_channel);
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            // sprintf(msg_buffer, "/channelmsg : @%s é o novo administrador.\n", clt->nickname);
            CHANNEL_send_message_all(channel, msg_buffer);
            break;

        case MESSAGE_TOPIC:         // Mostre todos comando do usuario
            break;

        case MESSAGE_QUIT_CHANNEL:          // Usuario desconectou-se do canal
            if(!strcmp(clt->nickname, channel->nickname_admin))    
                role = '@';
            else role = '#';  

            // sprintf(msg_buffer, "/quit %c%s : %s\n", role, clt->nickname, "desconectou-se.");
            CHANNEL_send_message_all(channel, msg_buffer);
            break;

        case MESSAGE_ERR_INVITEONLYCHAN:            // Error: usuario não foi convidado para entrar nesse canal.
            // sprintf(msg_buffer, "/join ERR_INVITEONLYCHAN\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_ERR_BANNEDFROMCHAN:            // Error: usuario não foi banido para entrar nesse canal.
            // sprintf(msg_buffer, "/join ERR_BANNEDFROMCHAN\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_KICK_CHANNEL:
            // sprintf(msg_buffer, "/channelmsg %s %s\n", buffer, "foi banido pelo administrador.");
            CHANNEL_send_message_all(channel, msg_buffer);

            // sprintf(msg_buffer, "/kick SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_UNKICK_CHANNEL:
            // sprintf(msg_buffer, "/kick SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_MUTE_CHANNEL:
            // sprintf(msg_buffer, "/channelmsg %s %s\n", buffer, "foi silenciado pelo administrador.");
            CHANNEL_send_message_all(channel, msg_buffer);

            // sprintf(msg_buffer, "/mute SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_UNMUTE_CHANNEL:
            // sprintf(msg_buffer, "/unmute SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            break;

        case MESSAGE_MUTE_ERR_NOSUCHNICK:
            // sprintf(msg_buffer, "/mute ERR_NOSUCHNICK\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_MUTE_ERR_CHANOPRIVSNEEDED:
            // sprintf(msg_buffer, "/mute ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_UNMUTE_ERR_NOSUCHNICK:
            // sprintf(msg_buffer, "/unmute ERR_NOSUCHNICK\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_UNMUTE_ERR_CHANOPRIVSNEEDED:
            // sprintf(msg_buffer, "/unmute ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_KICK_ERR_NOSUCHNICK:
            // sprintf(msg_buffer, "/kick ERR_NOSUCHNICK\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_KICK_ERR_CHANOPRIVSNEEDED:
            // sprintf(msg_buffer, "/kick ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

         case MESSAGE_WHOIS_CHANNEL:
            // sprintf(msg_buffer, "/whois SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_WHOIS_ERR_NOSUCHNICK:
            // sprintf(msg_buffer, "/whois ERR_NOSUCHNICK\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_WHOIS_ERR_CHANOPRIVSNEEDED:
            // sprintf(msg_buffer, "/whois ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

         case MESSAGE_MODE_CHANNEL:

            // sprintf(msg_buffer, "/mode SUCCESS\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            sprintf(msg_buffer, "/channelmsg canal %s agora é %s\n", channel->nickname_channel, buffer);
            CHANNEL_send_message_all(channel, msg_buffer);
            break;
        
        case MESSAGE_MODE_ERR_CHANOPRIVSNEEDED:
            // sprintf(msg_buffer, "/mode ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        default:
            return FAIL;
    }
    return SUCCESS;
}
