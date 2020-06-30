#include "channel.h"

// name_channel and channel pointer
map<string, CHANNEL_conn*> channels;
map<string, CHANNEL_conn*>::iterator it_channel;
map<int, string>::iterator it_arrived;

CHANNEL_conn* conn_criar_CHANNEL(string name_channel, client* clt);
// void CHANNEL_destroy(CHANNEL_conn* channel);
// int CHANNEL_remove_user(CHANNEL_conn* channel, client* clt);
// int CHANNEL_add_user(CHANNEL_conn* channel, client* clt); 
// void CHANNEL_join(string name_channel, client* clt); 
// int CHANNEL_kick_user(CHANNEL_conn* channel, client* clt, string kick_nickname); 
// int CHANNEL_unkick_user(CHANNEL_conn* channel, client* clt); 
// bool CHANNEL_send_message(int cl_socket, char* buffer); 
// void CHANNEL_send_message_all(CHANNEL_conn* channel, char* buffer);
// void CHANNEL_send_message_one(CHANNEL_conn* channel, client* clt, char* buffer);
// int CHANNEL_broadcast(CHANNEL_conn* channel, client* clt, int type, char* buffer); 

/**
 * @function conn_criar_CHANNEL
 * 
 * Cria canal com as propriedades iniciais necessarias.
 * 
 * @param { string } name_channel : nome do canal em que ação acontece
 * @param { client[] } clt : client que será removido.
 * @permision: self user | admin 
 * @return { SUCESS | FAIL }
 * 
 */
CHANNEL_conn* conn_criar_CHANNEL(string name_channel, client* clt) 
{        
    // int size_participants = 1; // há apenas um usuario: o admin;

    // // Verifica se o canal não existe.
    // if(channels.find(name_channel) == channels.end()) {
        
    //     CHANNEL_conn* newChannel = (CHANNEL_conn*) malloc(sizeof(CHANNEL_conn));
    //     newChannel->participants[clt->nickname] = clt->cl_socket;
    //     newChannel->arrived[size_participants] = clt->nickname;
    //     newChannel->name_admin = clt->nickname;
    //     channels[name_channel] = newChannel;

    //     clt.channel = newChannel;

    //     return newChannel;
    // }
    return NULL;
}

// /**
//  * @function conn_destruit_CHANNEL
//  * 
//  * Excluir o canal (limpa a memória)
//  * @param { CHANNEL_conn* } channel : referencia do canal em que ação acontece
//  * @permision: admin 
//  * 
//  */
// void CHANNEL_destroy(CHANNEL_conn* channel) 
// {
//     channels.erase(channel->name_channel);
//     free(channel);
// }

// /**
//  * @function conn_destruit_CHANNELS
//  * 
//  * Exclui TODOS os canais
//  * @permision: server 
//  * 
//  */
// void CHANNEL_destroy_all() 
// {
//     for(auto channel : channels) {
//         free(channels.second);
//     }
//     channels.clear();
// }

// /**
//  * @function CHANNEL_remove_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será removido.
//  * @permision: self user | admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// int CHANNEL_remove_user(CHANNEL_conn* channel, client* clt) 
// {
//     if(channel == NULL) return FAIL;

//     // limpa a referencia do canal do cliente.
//     clt.channel = NULL;
    
//     // adiciona o participante e sua respectiva ordem de chegada
//     channel->participants.erase(clt->nickname);

//     // Remove da lista de ordem de chegada
//     it_arrived = channel->arrived.find(clt->nickname);
//     if(it_arrived != channel->arrived.end()) 
//     { 
//         channel->arrived.erase(it_arrived);
//     }

//     // se há participantes no canal
//     if(!channel->participants.empty()) 
//     {
//         if(!channel->name_admin == clt->nickname) {
//             int newAdmin = channel->arrived.begin()->second;
//             clt = clt_get_by_nickname(newAdmin);
//             CHANNEL_broadcast(channel, clt, MESSAGE_NEW_ADMIN_CHANNEL, "");
//         }                
//     }
//     // se não tem nenhum participante no cai, ele deve ser excluido 
//     else
//     {
//         CHANNEL_destroy(channel);
//     }

//     return SUCCESS;
// }

// /**
//  * @function CHANNEL_remove_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será removido.
//  * @permision: self user | admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// int CHANNEL_add_user(CHANNEL_conn* channel, client* clt) 
// {
//     if(channel == NULL) return FAIL;

//     //extrae o valor da posição de chegada do ultimo participante
//     int last = channel->rbegin()->first + 1;
//     printf("************posicao %d **********\n", last);

//     // Check if the user is banned
//     if(channel->notAllowedParticipants[clt->nickname]) 
//     {
//         return ERR_BANNEDFROMCHAN;
//     }

//     // TODO: check invite permission 

//     // adiciona o participante e sua respectiva ordem de chegada
//     channel->participants[clt->nickname] = clt->cl_socket;  
//     channel->arrived[last] = clt->nickname;

//     clt->channel = channel;

//     return SUCCESS;
// }

// /**
//  * @function CHANNEL_remove_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será removido.
//  * @permision: self user | admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// void CHANNEL_join(string name_channel, client* clt) 
// {    
//     CHANNEL_conn* channel;

//     // Se o usuário já estiver em um canal, ele é retirado dele.
//     if(clt->channel != NULL) 
//     {
//         it_channel = channels.find(channel->name_channel);
//         CHANNEL_remove_user(clt->channel, clt);
//         CHANNEL_broadcast(it_channel, clt, MESSAGE_QUIT_CHANNEL, "");
//     } else {
//         clt->channel = channel;
//     }

//     it_channel = channels.find(name_channel);

//     // Se o canal não existe, então será criado e o criador é admin
//     if(it_channel == channels.end()) 
//     {
//         channel = conn_criar_CHANNEL(name_channel, clt);
//         CHANNEL_broadcast(channel, clt, MESSAGE_NEW_CHANNEL, "");
//         clt->channel = channel;
//     } else 
//     {
//         int response = CHANNEL_add_user(it_channel, clt);
        
//         switch (response)
//         {
//             case SUCCESS:
//                 CHANNEL_broadcast(it_channel, clt, MESSAGE_JOIN_CHANNEL, "");
//                 break;
//             case ERR_BANNEDFROMCHAN:
//                 CHANNEL_broadcast(it_channel, clt, MESSAGE_ERR_BANNEDFROMCHAN, "");
//                 break;
//             // case ERR_INVITEONLYCHAN:
//             //     CHANNEL_broadcast(it_channel, clt, MESSAGE_ERR_INVITEONLYCHAN, "");
//             //     break;
//             default:
//                 break;
//         }
        
//     }
// }

// int CHANNEL_invite() {

// }
    
// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// int CHANNEL_kick_user(CHANNEL_conn* channel, client* clt, string kick_nickname) 
// {   
//     if(channel == NULL) return FAIL;

//     if(clt_validate_nickname(kick_nickname) && channel->participants[kick_nickname] != NULL) 
//     {
//         CHANNEL_broadcast(channel, clt, ERR_NOSUCHNICK, "");
//         return FAIL;
//     }

//     if(clt->nickname == channel->name_admin) 
//     {
//         // Se o canal existe, o usuario é adicionado da lista de kick.
//         if(!channel->notAllowedParticipants[kick_nickname]) 
//         {
//             channel->notAllowedParticipants.insert(kick_nickname);
//         }

//         client* ban = clt_get_by_nickname(kick_nickname);
        
//         // exclui o usuario do canal
//         CHANNEL_remove_user(channel, ban);        
//         CHANNEL_broadcast(it_cchannelhannel, clt, MESSAGE_KICK_CHANNEL, kick_nickname);

//         return SUCCESS;
//     } else {
//         CHANNEL_broadcast(channel, clt, ERR_CHANOPRIVSNEEDED, kick_nickname)
//         return FAIL;
//     }
// }

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// int CHANNEL_unkick_user(CHANNEL_conn* channel, client* clt) 
// {   
//     if(channel == NULL) return FAIL;
    
//     if(clt_validate_nickname(kick_nickname) && channel->participants[kick_nickname] != NULL) 
//     {
//         CHANNEL_broadcast(channel, clt, ERR_NOSUCHNICK, "");
//         return FAIL;
//     }
    
//     // Se o canal existe, o usuario é adicionado da lista de kick.
//     it_channel->notAllowedParticipants.erase(clt->nickname);

//     CHANNEL_broadcast(it_channel, clt, MESSAGE_UNKICK_CHANNEL, "");

//     return SUCCESS;
// }

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// int CHANNEL_commands();

// /** Messagens **/

// /** **/
// bool CHANNEL_send_message(int cl_socket, char* buffer) 
// {
//     int try_send = 0;

//     // Tenta enviar a mensagem até receber confirmação. Se após 
//     while(send(cl_socket, buffer, strlen(buffer), 0) < 0 && try_send < 6) 
//     {
//         try_send += 1;
//     }
//     return (try_send == 6) ? false : true;
// }

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// void CHANNEL_send_message_all(CHANNEL_conn* channel, char* buffer)
// {
//     // Envio da mensagem aos cliente em broadcast 
//     for(auto client : channel->participants) 
//     {
//         // TODO: split buffer

//         // Tenta enviar a mensagem
//         if(CHANNEL_send_message(client->second, buffer)) 
//         {
//             msg_send_cliente( clt->nickname, client->first);
//         }
//         // Se o usuario não está respondendo é desconectado do canal 
//         else 
//         {
//             msg_client_no_response(client->first);
//             CHANNEL_remove_user(channel, client->first);
//         } 
//     }
// }

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// void CHANNEL_send_message_one(CHANNEL_conn* channel, client* clt, char* buffer)
// {
//     // Tenta enviar a mensagem
//     if(CHANNEL_send_message(clt->cl_socket , buffer)) 
//     {
//         msg_send_cliente(clt->nickname, client->first);
//     }
//     // Se o usuario não está respondendo é desconectado do canal 
//     else 
//     {
//         msg_client_no_response(clt->nickname);
//         CHANNEL_remove_user(channel, clt);
//     } 
// }

// /**
//  * @function CHANNEL_kick_user
//  * 
//  * Exclui o usuário do canal e o proibe de entrar novamente.
//  * @param { string } name_channel : nome do canal em que ação acontece
//  * @param { client[] } clt : client que será banido.
//  * @permision: admin 
//  * @return { SUCESS | FAIL }
//  * 
//  */
// int CHANNEL_broadcast(CHANNEL_conn* channel, client* clt, int type, char* buffer) 
// {
//     char msg_buffer[BUFFER_SIZE];
    
//     // Configuracao da mensagem a ser enviada (adiciona que enviou a mensagem no conteudo)
//     switch (type) 
//     {
//         // Formatação da mensagem do cliente.
//         case MESSAGE_CLIENT:
//             char role = (channel->name_admin == clt->nickname)? '@' : '+';
//             sprintf(msg_buffer, "CLIENTE >> /msg %c%s : %s\n", role, clt->nickname, buffer);
//             break;

//         // Mensagem para quem criou o canal.
//         case MESSAGE_NEW_JOIN_CHANNEL:
//             sprintf(msg_buffer, "SERVER << /join SUCCESS %s role:admin.\n", channel->name_channel);
//             CHANNEL_send_message_one(channel, clt, msg_buffer);

//             sprintf(msg_buffer, "SERVER << /join RPL_NAMREPLY %s +%s", channel->name_channel, clt->nickname);
//             CHANNEL_send_message_one(channel, clt, msg_buffer);
//             break;

//         // Mensagem padrão dos usuários que acabaram de entrar no canal.
//         case MESSAGE_JOIN_CHANNEL:
//             char role = (channel->name_admin == clt->nickname)? '@' : '+';         
          
//             sprintf(msg_buffer, "SERVER << /join SUCCESS %s role:user.\n", channel->name_channel);
//             CHANNEL_send_message_one(channel, clt, msg_buffer);

//             // a lista ordenada pela ordem de chegada, sem considerar o admin.
//             sprintf(msg_buffer, "SERVER << /join RPL_NAMREPLY %s +%s\n", channel->name_channel, clt->nickname);

//             // TODO: avoid overflow buffer
//             it_channel = ++channel->arrived.begin();
//             while(it_channel != channel->arrived.end()) {
//                 strcpy(msg_buffer, " @");
//                 strcpy(msg_buffer, it_channel->second);
//                 ++it_channel;
//             }
            
//             CHANNEL_send_message_one(channel, clt, msg_buffer);

//             // mensagem para todos no canal
//             sprintf(msg_buffer, "SERVER << /channelmsg : %c%s conectou-se.\n", role, clt->nickname);
//             CHANNEL_send_message_all(channel, msg_buffer);

//             break;

//         // O novo admin do canal é
//         case MESSAGE_NEW_ADMIN_CHANNEL:
//             sprintf(msg_buffer, "SERVER << /channelmsg : @%s is the new admin.\n", clt->nickname);
//             CHANNEL_send_message_all(channel, msg_buffer);
//             break;

//         // Mostre todos comando do usuario
//         case MESSAGE_TOPIC:
//             break;

//         // Usuario desconectou-se do canal
//         case MESSAGE_QUIT_CHANNEL:
//             char role = (channel->name_admin == clt->nickname)? '@' : '+';
//             sprintf(msg_buffer, "SERVER >> /quit %c%s : %s\n", role, clt->nickname, "desconectou-se.");
//             CHANNEL_send_message_all(channel, msg_buffer);
//             break;

//         // Error: usuario não foi convidado para entrar nesse canal.
//         case MESSAGE_ERR_INVITEONLYCHAN:
//             sprintf(msg_buffer, "SERVER << /join ERR_INVITEONLYCHAN\n");
//             CHANNEL_send_message_one(channel, msg_buffer);
//             break;

//         // Error: usuario não foi banido para entrar nesse canal.
//         case MESSAGE_ERR_BANNEDFROMCHAN:
//             sprintf(msg_buffer, "SERVER << /join ERR_BANNEDFROMCHAN\n");
//             CHANNEL_send_message_one(channel, msg_buffer);
//             break;

//         case MESSAGE_KICK_CHANNEL:
//             sprintf(msg_buffer, "SERVER >> /channelmsg %s %s\n", buffer, "foi banido pelo administrador.");
//             CHANNEL_send_message_all(channel, msg_buffer);

//             sprintf(msg_buffer, "SERVER << /kick SUCCESS %s\n", buffer);
//             CHANNEL_send_message_one(channel, clt, msg_buffer);
//             break;

//         case MESSAGE_UNKICK_CHANNEL:
//             sprintf(msg_buffer, "SERVER << /kick SUCCESS %s\n", buffer);
//             CHANNEL_send_message_one(channel, clt, msg_buffer);

//             break;

//         case ERR_NOSUCHNICK:
//             sprintf(msg_buffer, "SERVER >> /kick ERR_NOSUCHNICK\n");
//             CHANNEL_send_message_all(channel, msg_buffer);
//             break;
        
//         case ERR_CHANOPRIVSNEEDED:
//             sprintf(msg_buffer, "SERVER >> /kick ERR_CHANOPRIVSNEEDED");
//             CHANNEL_send_message_all(channel, msg_buffer);
//             break;

//         default:
//             return FAIL;
//     }

//     // Enviar a mensagem para todo mundo
//     //CHANNEL_send_message_all(channel, clt, msg_buffer);
//     return SUCCESS;
// }
