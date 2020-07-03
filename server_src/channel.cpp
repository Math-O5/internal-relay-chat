#include "channel.h"

// nickname_channel and channel pointer
map<const char*, CHANNEL_conn*, cmp_str> channels;
map<const char*, CHANNEL_conn*, cmp_str>::iterator it_channel;
map<int, char*>::iterator it_arrived;
map<const char*, int, cmp_str> ::iterator it;

void map_insert_key_char(map<const char*, int, cmp_str>& pointer, const char* str, int value) {
    char* strCopy = (char*)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(strCopy, str);
    pointer.insert(pair<char*, int>(strCopy, value));
}

void map_insert_key_channel(map<const char*, CHANNEL_conn*, cmp_str>& pointer, const char* str, CHANNEL_conn* value) {
    char* strCopy = (char*)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(strCopy, str);
    pointer.insert(pair<char*, CHANNEL_conn*>(strCopy, value));
}

void map_insert_key_int(map<int, char*>& pointer, int value, const char* str) {
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
    char buffer[BUFFER_SIZE];
    // Verifica se o canal não existe.
    if(channels.find(nickname_channel) == channels.end()) {
        map<const char*, int, cmp_str> tmp_participants;
        map<int, char*> tmp_arrived;
        set<const char*, cmp_str> tmp_muted, tmp_notAllow;

        CHANNEL_conn* newChannel = (CHANNEL_conn*) malloc(sizeof(CHANNEL_conn));
        newChannel->nickname_admin = (char*) malloc(sizeof(char)*MAX_SIZE_NAME);
        newChannel->nickname_channel = (char*) malloc(sizeof(char)*MAX_SIZE_NAME);
        
        newChannel->participants = tmp_participants;
        newChannel->arrived = tmp_arrived;
        newChannel->notAllowedParticipants = tmp_notAllow;
        newChannel->mutedParticipants = tmp_muted;

        map_insert_key_char(newChannel->participants, clt->nickname, clt->cl_socket);
        map_insert_key_int(newChannel->arrived, 1, clt->nickname);
        map_insert_key_channel(channels, nickname_channel, newChannel);

        strcpy(newChannel->nickname_admin, clt->nickname);
        strcpy(newChannel->nickname_channel, nickname_channel);

        it =  newChannel->participants.find(clt->nickname);
  
        newChannel->is_public = true;
        
        // reference of channel on client
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

bool CHANNEL_check_privilege(CHANNEL_conn* channel, client* clt) {
    if(channel == NULL || clt == NULL) return false;
    if(!strcmp(channel->nickname_admin, clt->nickname))
        return true;
    return false;
}

/**
 * @function conn_destruit_CHANNELS
 * 
 * Exclui TODOS os canais
 * @permision: server 
 * 
 */
void CHANNEL_list(char* buffer) 
{
    char tmp_buffer[BUFFER_SIZE];
    memset(buffer, 0 , sizeof(char)*BUFFER_SIZE);
    memset(tmp_buffer, 0 , sizeof(char)*BUFFER_SIZE);
    
    for(it_channel = channels.begin(); it_channel != channels.end(); ++it_channel) {
        printf("aff %s\n", it_channel->second->nickname_channel);
        strcat(tmp_buffer, it_channel->first);
        strcat(tmp_buffer, ", ");
    }
    
    if(strlen(tmp_buffer) <= 0)
        sprintf(buffer, "/channels : Não há canais. Digite /join <nome_do_canal>\n");
    else
        sprintf(buffer, "/channels %s\n", tmp_buffer);
}

void CHANNEL_list_participants(struct _client* clt, char* buffer) 
{
    if(clt->channel == NULL)
    {
        sprintf(buffer, "/servermsg : Você não está em nenhum canal. Digite /join <nome_do_canal> entrar em um.\n");
        return;
    }

    // TODO: avoid overflow buffer
    it_arrived = clt->channel->arrived.begin();
    strcat(buffer, " #");
    strcat(buffer, it_arrived->second);
    strcat(buffer, ", ");
    ++it_arrived;
    
    while(it_arrived != clt->channel->arrived.end()) {
        strcat(buffer, "@");
        strcat(buffer, it_arrived->second);
        strcat(buffer, ", ");
        ++it_arrived;
    }

    // memset(buffer, 0 ,sizeof(buffer));
    // for(auto cli : clt->channel->participants) {
    //     strcat(buffer, cli.first);
    //     strcat(buffer, ", ");
    // }   
    return;
}

void CHANNEL_msg(CHANNEL_conn* channel, client* clt, char* buffer) 
{
    if(channel == NULL || clt == NULL) return;

    if(clt->channel->mutedParticipants.find(clt->nickname) == clt->channel->mutedParticipants.end())
    {
        msg_client_channel(clt->cl_id, clt->nickname, clt->channel->nickname_channel);
        CHANNEL_broadcast(clt->channel, clt, MESSAGE_CLIENT, buffer);
    } else 
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_CLIENT_MUTED_CHANNEL, "");
    }
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
int CHANNEL_remove_user(CHANNEL_conn* channel, client* clt) 
{
    if(channel == NULL) return FAIL;

    // adiciona o participante e sua respectiva ordem de chegada
    channel->participants.erase(clt->nickname);

    // limpa a referencia do canal do cliente.
    clt->channel = NULL;
    
    // Remove da lista de ordem de chegada
    for(it_arrived = channel->arrived.begin(); it_arrived != channel->arrived.end(); ++it_arrived) 
    {
        if(!strcmp(it_arrived->second, clt->nickname)) 
        {
            channel->arrived.erase(it_arrived);
            break;
        }
    }

    // se há participantes no canal
    if(!channel->participants.empty()) 
    {
        if(!strcmp(channel->nickname_admin, clt->nickname)) 
        {
            char* newAdmin = channel->arrived.begin()->second;
            clt = clt_get_by_nickname(newAdmin);
            strcpy(channel->nickname_admin, clt->nickname);
            CHANNEL_broadcast(channel, clt, MESSAGE_NEW_ADMIN_CHANNEL, "");
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

    for(auto i : channel->notAllowedParticipants) {
        printf("value: %s\n", i);
    }

    // Check if the user is banned
    if(channel->notAllowedParticipants.find(clt->nickname) != channel->notAllowedParticipants.end()) 
    {
        return ERROR_USER_BANNED;
    }

    // TODO: check invite permission 

    // adiciona o participante e sua respectiva ordem de chegada
    map_insert_key_char(channel->participants, clt->nickname, clt->cl_socket);
    map_insert_key_int(channel->arrived, last, clt->nickname);

    clt->channel = channel;

    return SUCCESS;
}

void CHANNEL_list(struct _client* clt) 
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
    
    // atualiza o nome do usuario dentro do canal
    clt->channel->participants.erase(old_nickname);
    map_insert_key_char(clt->channel->participants, clt->nickname, clt->cl_socket);

    // Remove da lista de ordem de chegada
    for(it_arrived = clt->channel->arrived.begin(); it_arrived != clt->channel->arrived.end(); ++it_arrived) 
    {
        if(!strcmp(it_arrived->second, old_nickname)) 
        {
            strcpy(it_arrived->second, clt->nickname); 
            break;
        }
    }

    if(!strcmp(clt->channel->nickname_admin, old_nickname)) {
        strcpy(clt->channel->nickname_admin, clt->nickname);
    }

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
            printf("huuuuu\n");

        it_channel = channels.find(clt->channel->nickname_channel);
            printf("juuuu\n");

        CHANNEL_remove_user(clt->channel, clt);
            printf("kii\n");

        CHANNEL_broadcast(it_channel->second, clt, MESSAGE_QUIT_CHANNEL, "");
    }

    printf("liiii\n");
    it_channel = channels.find(nickname_channel);

    // // Se o canal não existe, então será criado e o criador é admin
    if(it_channel == channels.end()) 
    {
        channel = conn_criar_CHANNEL(nickname_channel, clt);
        CHANNEL_broadcast(channel, clt, MESSAGE_NEW_JOIN_CHANNEL, "");
    } 
    else 
    {
            printf("niuuuuuu");
        int response = CHANNEL_add_user(it_channel->second, clt);
        
        switch (response)
        {
            case SUCCESS:
                CHANNEL_broadcast(it_channel->second, clt, MESSAGE_JOIN_CHANNEL, "");
                break;
            case ERROR_USER_BANNED:
                CHANNEL_broadcast(it_channel->second, clt, MESSAGE_ERR_BANNEDFROMCHAN, "");
                break;
            case MESSAGE_ERR_INVITEONLYCHAN:
                CHANNEL_broadcast(it_channel->second, clt, MESSAGE_ERR_INVITEONLYCHAN, "");
                break;
            default:
                break;
        }
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
    client* ban = clt_get_by_nickname(kick_nickname);
    
    if(channel == NULL) return;
    
    if(ban == NULL || channel->participants.find(kick_nickname) == channel->participants.end()) {
        CHANNEL_broadcast(channel, clt, MESSAGE_KICK_ERR_NOSUCHNICK, kick_nickname);
        return;
    }


    // apenas admin
    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {   

        // Se o canal existe, o usuario é adicionado da lista de kick. Obs. sets não armazena valores duplicados.
        channel->notAllowedParticipants.insert(ban->nickname);
        
        // printf("size %d\n", channel->notAllowedParticipants.size());
        // for(auto i : channel->notAllowedParticipants) {
        //     printf("value: %s\n", i);
        // }
        
        // exclui o usuario do canal
        CHANNEL_remove_user(channel, ban);
        clt_send_message(ban->cl_socket, "/servermsg : O administrador baniu do canal.\n");        
        CHANNEL_broadcast(channel, clt, MESSAGE_KICK_CHANNEL, kick_nickname);
    } else {
        CHANNEL_broadcast(channel, clt, MESSAGE_KICK_ERR_CHANOPRIVSNEEDED, kick_nickname);
    }
}

void CHANNEL_list_ban(CHANNEL_conn* channel, char* buffer) 
{    
    memset(buffer, 0, sizeof(buffer));
    for(auto i : channel->notAllowedParticipants) {
        strcat(buffer, i);
        strcat(buffer, ", ");
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
    client* ban = clt_get_by_nickname(unkick_nickname);
    
    if(ban == NULL || channel->participants[unkick_nickname] != 0) 
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_UNKICK_ERR_NOSUCHNICK, unkick_nickname);
        return;
    }
    
    // apenas admin
    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {
        // Se o canal existe, o usuario é retirado da lista de kick.
        channel->notAllowedParticipants.erase(unkick_nickname);
        CHANNEL_broadcast(channel, clt, MESSAGE_UNKICK_CHANNEL, unkick_nickname);
     
        if(ban != NULL) 
        {
            clt_send_message(ban->cl_socket, "/servermsg : O administrador liberou seu acesso ao canal.\n");   
        }
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

    client* mute_client = clt_get_by_nickname(mute_nickname);
    
    // Se o usuario não está no canal.
    if(mute_client == NULL || channel->participants[mute_nickname] == 0) 
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_MUTE_ERR_NOSUCHNICK, mute_nickname);
        return;
    }

    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {   
        // Se o canal existe, o usuario é adicionado da lista de kick. Obs. sets não armazena valores duplicados.
        channel->mutedParticipants.insert(mute_client->nickname);              
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
        CHANNEL_broadcast(channel, clt, MESSAGE_UNMUTE_ERR_NOSUCHNICK, unmute_nickname);
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
        CHANNEL_broadcast(channel, clt, MESSAGE_WHOIS_ERR_NOSUCHNICK, whois);
        return;
    }

    if(!strcmp(clt->nickname, channel->nickname_admin)) 
    {           
        channel->mutedParticipants.erase(whois);            // Se o canal existe, o usuario é apagado da lista de kick. Obs. sets não armazena valores duplicados.
        
        client* clt = clt_get_by_nickname(whois);
        strcat(whois, " ");
        strcat(whois, clt->ip_address);

        CHANNEL_broadcast(channel, clt, MESSAGE_WHOIS_CHANNEL, whois);
    } else
    {
        CHANNEL_broadcast(channel, clt, MESSAGE_WHOIS_ERR_CHANOPRIVSNEEDED, whois);
    }
}   

void CHANNEL_mode(CHANNEL_conn* channel, client* clt, bool is_public) 
{
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
        CHANNEL_broadcast(channel, clt, MESSAGE_MODE_ERR_CHANOPRIVSNEEDED, "");
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
    else 
    {
        // Se o usuario não está respondendo é desconectado do canal 
        msg_client_no_response_channel(clt->cl_id, clt->nickname, channel->nickname_channel);
        CHANNEL_remove_user(channel, clt);
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
int CHANNEL_broadcast(CHANNEL_conn* channel, client* clt, int type, const char* buffer) 
{
    char msg_buffer[BUFFER_SIZE];
    char role;

    // Configuracao da mensagem a ser enviada (adiciona que enviou a mensagem no conteudo)
    switch (type) 
    {
        case MESSAGE_CLIENT:    // Formatação da mensagem do cliente.
            if(!strcmp(channel->nickname_admin, clt->nickname))    
                role = '#';
            else role = '@';

            sprintf(msg_buffer, "/msg %c%s : %s\n", role, clt->nickname, buffer);
            CHANNEL_send_message_all(channel, msg_buffer);
            break;

        case MESSAGE_CHANGE_NICKNAME:
            if(!strcmp(clt->nickname, channel->nickname_admin)) 
                role = '#';
            else role = '@';

            sprintf(msg_buffer, "/channelmsg : O usuario %c%s agora chama-se %c%s!\n", role, buffer, role, clt->nickname);  
            CHANNEL_send_message_all(channel, msg_buffer);
            break;

        case MESSAGE_NEW_JOIN_CHANNEL:          // Mensagem para quem criou o canal.
            sprintf(msg_buffer, "/join SUCCESS %s role:admin\n", channel->nickname_channel);
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            sprintf(msg_buffer, "/join RPL_NAMREPLY %s #%s\n", channel->nickname_channel, clt->nickname);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_JOIN_CHANNEL:          // Mensagem padrão dos usuários que acabaram de entrar no canal.
            if(!strcmp(clt->nickname, channel->nickname_admin))  
                role = '#';
            else role = '@';   

            sprintf(msg_buffer, "/join SUCCESS %s role:user\n", channel->nickname_channel);
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            // a lista ordenada pela ordem de chegada, sem considerar o admin.
            sprintf(msg_buffer, "/join RPL_NAMREPLY %s", channel->nickname_channel);
            CHANNEL_list_participants(clt, msg_buffer);
            strcat(msg_buffer, "\n\0");
            printf("Lets see... %s\n", msg_buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            // mensagem para todos no canal
            sprintf(msg_buffer, "/channelmsg : %c%s conectou-se no canal %s.\n", role, clt->nickname, channel->nickname_channel);
            CHANNEL_send_message_all(channel, msg_buffer);

            break;

        case MESSAGE_NEW_ADMIN_CHANNEL:         // O novo admin do canal é
            if(!strcmp(clt->nickname, channel->nickname_admin)) 
                role = '#';
            else role = '@';   

            sprintf(msg_buffer, "/join SUCCESS %s role:admin\n", channel->nickname_channel);
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            sprintf(msg_buffer, "/channelmsg : #%s é o novo administrador.\n", clt->nickname);
            CHANNEL_send_message_all(channel, msg_buffer);
            break;

        case MESSAGE_TOPIC:         // Mostre todos comando do usuario
            break;

        case MESSAGE_QUIT_CHANNEL:          // Usuario desconectou-se do canal
            if(!strcmp(clt->nickname, channel->nickname_admin))    
                role = '#';
            else role = '@';  

            sprintf(msg_buffer, "/channelmsg : %c%s desconectou-se do canal %s\n", role, clt->nickname, channel->nickname_channel);
            CHANNEL_send_message_all(channel, msg_buffer);
            break;

        case MESSAGE_ERR_INVITEONLYCHAN:            // Error: usuario não foi convidado para entrar nesse canal.
            sprintf(msg_buffer, "/join ERR_INVITEONLYCHAN\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_ERR_BANNEDFROMCHAN:            // Error: usuario não foi banido para entrar nesse canal.
            sprintf(msg_buffer, "/join ERR_BANNEDFROMCHAN\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_KICK_CHANNEL:
            sprintf(msg_buffer, "/channelmsg : %s foi banido pelo administrador.\n", buffer);
            CHANNEL_send_message_all(channel, msg_buffer);

            sprintf(msg_buffer, "/kick SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_UNKICK_CHANNEL:
            sprintf(msg_buffer, "/unkick SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_CLIENT_MUTED_CHANNEL:
            sprintf(msg_buffer, "/channelmsg : Você foi silenciado pelo administrador.\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        case MESSAGE_MUTE_CHANNEL:
            sprintf(msg_buffer, "/channelmsg : %s foi silenciado pelo administrador.\n", buffer);
            CHANNEL_send_message_all(channel, msg_buffer);

            sprintf(msg_buffer, "/mute SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_UNMUTE_CHANNEL:
            sprintf(msg_buffer, "/channelmsg : %s  pode conversar novamente.\n", buffer);
            CHANNEL_send_message_all(channel, msg_buffer);
            
            sprintf(msg_buffer, "/unmute SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_MUTE_ERR_NOSUCHNICK:
            sprintf(msg_buffer, "/mute ERR_NOSUCHNICK %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_MUTE_ERR_CHANOPRIVSNEEDED:
            sprintf(msg_buffer, "/mute ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_UNMUTE_ERR_NOSUCHNICK:
            sprintf(msg_buffer, "/unmute ERR_NOSUCHNICK %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_UNMUTE_ERR_CHANOPRIVSNEEDED:
            sprintf(msg_buffer, "/unmute ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_KICK_ERR_NOSUCHNICK:
            sprintf(msg_buffer, "/kick ERR_NOSUCHNICK %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_KICK_ERR_CHANOPRIVSNEEDED:
            sprintf(msg_buffer, "/kick ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

         case MESSAGE_WHOIS_CHANNEL:
            sprintf(msg_buffer, "/whois SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_WHOIS_ERR_NOSUCHNICK:
            sprintf(msg_buffer, "/whois ERR_NOSUCHNICK %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_WHOIS_ERR_CHANOPRIVSNEEDED:
            sprintf(msg_buffer, "/whois ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

         case MESSAGE_MODE_CHANNEL:

            sprintf(msg_buffer, "/mode SUCCESS\n");
            CHANNEL_send_message_one(channel, clt, msg_buffer);

            sprintf(msg_buffer, "/channelmsg canal %s agora é %s.\n", channel->nickname_channel, buffer);
            CHANNEL_send_message_all(channel, msg_buffer);
            break;
        
        case MESSAGE_MODE_ERR_CHANOPRIVSNEEDED:
            sprintf(msg_buffer, "/mode ERR_CHANOPRIVSNEEDED");
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;

        case MESSAGE_INVITE_CHANNEL:
            sprintf(msg_buffer, "/invite SUCCESS %s\n", buffer);
            CHANNEL_send_message_one(channel, clt, msg_buffer);
            break;
        
        case MESSAGE_INVITED:
            sprintf(msg_buffer, 
                    "/servermsg : Você foi convidado para o canal %s, para entrar use o comando: /join %s.\n",
                    buffer, buffer);
            clt_send_message(clt->cl_socket, msg_buffer);
            break;
        
        default:
            return FAIL;
    }
    return SUCCESS;
}
