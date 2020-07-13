    #include "clients.h"

    // Array com os structs dos clientes
    client *cl_arr[MAX_CLIENTS];

    bool _is_char(char character) 
    {
        return ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z')) ? true : false;
    }

    bool _is_number(char number) 
    {
        return (number >= '1' && number <= '9') ? true : false; 
    }

    bool _is_seleted_special_char(char special) 
    {
        return (special == '-' || special == '_') ? true : false; 
    }

    bool clt_validate_nickname(char* nickname)
    {
            if(!strcmp(nickname, "invalid")) return false;
            int size = strlen(nickname);

            if(size < 2)
                return false;

            for(int i = 0; i < size; i++){
                if( (!_is_char(nickname[i])) && (!_is_number(nickname[i])) && (!_is_seleted_special_char(nickname[i])) )
                    return false;
            }

            return true;
        }

    /**
     * @functon CHANNEL_is_valid_nickname
     * @param { string } nickname nome a ser checado
     * @return { SUCCESS | ERR_NICKNAMEINUSE | ERR_ERRONEUSNICKNAME }
     * Checa se o nome é valido.
     */
    int clt_is_valid_nickname(char* nickname) 
    {
        if(!clt_validate_nickname(nickname) || !strcmp(nickname, "invalid")) return ERR_ERRONEUSNICKNAME;
        if(clt_get_by_nickname(nickname) != NULL)
            return ERR_NICKNAMEINUSE;
        else return SUCCESS;
    }

    // @Comentários em "clients.h"
    client* clt_criar(struct sockaddr_in address, int socket, int id, int sv_socket){
        client* cl = (client*) malloc(sizeof(client));  
        char tmp_ip[300];
        
        cl->cl_address = address;
        cl->cl_socket = socket;
        cl->cl_id = id;
        cl->sv_socket = sv_socket;
        cl->nickname = (char*) malloc(sizeof(char)*MAX_SIZE_NAME);
        cl->channel = NULL;
        
        cl->ip_address = (char*) malloc(sizeof(char)*300);
        sprintf(cl->ip_address, "%d.%d.%d.%d", 
        (address.sin_addr.s_addr & 0xff),
        (address.sin_addr.s_addr & 0xff00) >> 8,
        (address.sin_addr.s_addr & 0xff0000) >> 16,
        (address.sin_addr.s_addr & 0xff000000) >> 24);
        strcpy(cl->nickname, "invalid");

        return cl;
    }

    // @Comentários em "clients.h"
    int clt_add_queue(client* cl, pthread_mutex_t* mutex){

        pthread_mutex_lock(mutex);

        // Adiciona o cliente no primeiro espaco disponivel
        int index;
        for(index = 0; index < MAX_CLIENTS; index++){
            if(!cl_arr[index]){
                cl_arr[index] = cl;
                break;
            }
        }

        pthread_mutex_unlock(mutex);

        // Numero maximo de clientes ja foi atingido...
        if(index == MAX_CLIENTS)
            return FAIL;

        return SUCCESS;
    }

    // @Comentários em "clients.h"
    void clt_destruir(int id){
        for(int i = 0; i < MAX_CLIENTS; i++) 
        {
            if(cl_arr[i] != NULL && cl_arr[i]->cl_id == id) 
            {
                close(cl_arr[i]->cl_socket);
                cl_arr[i] = NULL;
                free(cl_arr[i]);
                break;
            }
        }
    }

    // @Comentários em "clients.h"
    void clt_destruir_clientes() 
    {
        for(int i = 0; i < MAX_CLIENTS; i++) 
        {
            if(cl_arr[i] != NULL) 
            {
                close(cl_arr[i]->cl_socket);
                free(&(*cl_arr[i]));
            }
        }
    }

    // @Comentários em "clients.h"
    client* clt_remove_queue(int id, pthread_mutex_t* mutex){
        
        pthread_mutex_lock(mutex);  

        // Procura pelo cliente, atraves de seu id, e o retira do array
        client* temp = NULL;
        for(int i = 0; i < MAX_CLIENTS; i++) 
        {
            if(cl_arr[i] && cl_arr[i]->cl_id == id) 
            {
                temp = cl_arr[i];
                cl_arr[i] = NULL;
                break;
            }
        }

        pthread_mutex_unlock(mutex);

        return temp;
    }

    client* clt_get_by_nickname(const char* cli_nickname) 
    {
        for(int i = 0; i < MAX_CLIENTS; i++) 
        {
            if(cl_arr[i] && !strcmp(cl_arr[i]->nickname, cli_nickname)) 
            {
                return cl_arr[i];
            }
        }
        return NULL;
    }

    // @Comentários em "clients.h"
    client* clt_get_by_id(int cli_id) 
    {
        for(int i = 0; i < MAX_CLIENTS; i++) 
        {
            if(cl_arr[i] && cl_arr[i]->cl_id == cli_id) 
            {
                return cl_arr[i];
            }
        }
        return NULL;
    }

    // @Comentários em "clients.h"
    bool clt_send_message(int cl_socket, const char* buffer) 
    {
        int try_send = 0;

        // Tenta enviar a mensagem até receber confirmação. Se após 
        while(send(cl_socket, buffer, strlen(buffer), 0) < 0 && try_send < 6) 
        {
            try_send += 1;
        }
        return (try_send == 6) ? false : true;
    }

    // @Comentários em "clients.h"
    void clt_send_message_all(int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer){

        // Configuracao da mensagem a ser enviada (adiciona que enviou a mensagem no conteudo)
        char msg_buffer[BUFFER_SIZE];
        sprintf(msg_buffer, ">> <CLIENTE %d>: %s\n", id_cur, buffer);

        pthread_mutex_lock(mutex);

        // Envio da mensagem aos clientes
        // Restringir para broadcast 
        for(int i = 0; i < max_conn; i++){
            if(cl_arr[i] != NULL) 
            {
                if(clt_send_message(cl_arr[i]->cl_socket, msg_buffer)) 
                {
                    msg_send_cliente(id_cur, cl_arr[i]->cl_id);
                } else {
                    msg_client_no_response(cl_arr[i]->cl_id);
                    clt_destruir(cl_arr[i]->cl_id);
                } 
            }
        }

        pthread_mutex_unlock(mutex);
    }

    // @Comentários em "clients.h"
    void clt_ping(client* cl) 
    {    
        char log[] = "/pong\n";
        msg_info_ping(cl->cl_id);
        if(clt_send_message(cl->cl_socket, log)) 
        {
            msg_info_pong(cl->cl_id);
        } 
    }

    // TODO: pass it to Channel
    void clt_split_and_send(int id_cur, int max_conn, char* buffer, pthread_mutex_t* mutex) 
    {
        int shift;
        char* pack; 
        shift = 0;
        
        pack = (char*)malloc(sizeof(char)*MAX_MESSAGE_LENGHT);
        
        while((shift = decode_message(buffer, pack, shift)) > 0) 
        {
            //clt_send_message_all(id_cur, max_conn, mutex, pack);
        }
        
        free(pack);
        bzero(buffer, BUFFER_SIZE);
    }
    // @Comentários em "clients.h"

    int clt_read_buffer(client* cl, char* buffer) 
    {
        
        // Comando: /ping
        if(strncmp(buffer, "/ping", 5) == 0) 
        {
            
            char log[] = ">> <SERVER>: pong\n";
            memset(buffer, '\0', BUFFER_SIZE);
            msg_info_ping(cl->cl_id);

            if(clt_send_message(cl->cl_socket, log)) 
            {
                msg_info_pong(cl->cl_id);
            } 
            return PING;

        // Comando: /quit
        } else if(strncmp(buffer, "/quit", 5) == 0) 
        {
            memset(buffer, '\0', BUFFER_SIZE);
            return QUIT;
        } 

        // Mensagem a ser enviada para os outros clientes
        return CONTINUE;
    }

    // @Comentários em "clients.h"
    void clt_run(int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex){
        // Buffer com as mensagens dos clientes 
        char buffer[BUFFER_SIZE];
        
        int response_code = 0;
        char temp_buffer_A[MAX_MESSAGE_LENGHT];
        char temp_buffer_B[MAX_MESSAGE_LENGHT];
        bool temp_bool;
        char temp_buffer_C[2*MAX_MESSAGE_LENGHT];

        memset(buffer, '\0', BUFFER_SIZE);

        // Recupera as informacoes do cliente...
        client* clt = clt_get_by_id(id_cur);

        // Cliente indisponível
        if(clt == NULL) 
        {
            return;
        }

        // Mensagem com as informacoes do cliente
        msg_info_client(id_cur, clt->cl_socket, clt->cl_address);

        while(true)
        {
            memset(buffer, '\0', BUFFER_SIZE);
            memset(temp_buffer_A, '\0', MAX_MESSAGE_LENGHT);

            // Mensagem recebida da usuário!
            // O cliente fica bloqueado enquanto esse evento não acontece.
            if(recv(clt->cl_socket, buffer, MAX_MESSAGE_LENGHT, 0) > 0)         
            {    
                // Se o buffer estiver vazio, espera-se outra mensagem
                if(strlen(buffer) <= 1)
                    continue;

                pthread_mutex_lock(mutex);

                int action_code = detect_action(buffer);
                switch(action_code) 
                {
                    case ACTION_NICK:
                        decode_nickname(buffer, temp_buffer_A);
                        response_code = clt_is_valid_nickname(temp_buffer_A);
                        
                        if (response_code == SUCCESS) 
                        {
                            strcpy(temp_buffer_B, clt->nickname);
                            strcpy(clt->nickname, temp_buffer_A); 
                            
                            sprintf(buffer, "/nickname SUCCESS %s\n", clt->nickname);  
                            clt_send_message(clt->cl_socket,buffer);

                            CHANNEL_on_change_nickname(clt, temp_buffer_B);  
                            msg_nickname_cliente(clt->cl_id, clt->nickname, temp_buffer_B);   
                        } else if(response_code == ERR_NICKNAMEINUSE) 
                        {
                            sprintf(buffer, "/nickname ERR_NICKNAMEINUSE %s\n", temp_buffer_A);  
                            clt_send_message(clt->cl_socket,buffer);
                        } else 
                        {
                            sprintf(buffer, "/nickname ERR_ERRONEUSNICKNAME %s\n", temp_buffer_A);  
                            clt_send_message(clt->cl_socket,buffer);
                        }

                        break;

                    case ACTION_JOIN:
                        if(clt_validate_nickname(clt->nickname) == false) 
                        {
                            sprintf(buffer, "/nickname ERR_NICKNAMEINUSE %s\n", clt->nickname);    
                            clt_send_message(clt->cl_socket,buffer);
                            break;
                        }

                        decode_join(buffer, temp_buffer_A);
                        if(CHANNEL_is_valid_channel_name(temp_buffer_A) == false) 
                        {
                            sprintf(buffer, "/join ERR_BADCHANNELKEY %s\n", clt->nickname);    
                            clt_send_message(clt->cl_socket, buffer);
                            break;
                        }

                        CHANNEL_join(temp_buffer_A, clt);
                        if(clt->channel != NULL)
                            msg_join_channel(clt);
                        break;

                    case ACTION_LIST:
                        memset(buffer, 0 , BUFFER_SIZE);
                        CHANNEL_list(buffer);
                        clt_send_message(clt->cl_socket, buffer);
                        break;

                    case ACTION_MODE:
                    if(clt->channel == NULL) 
                    {
                            clt_send_message(
                                clt->cl_socket, 
                                "/servermsg : Entre em um canal para continuar!\n\t/join <nome_do_canal>\t: para criar/entrar em nome_do_canal.\n\t/list\t: para ver todos os canais.\n"
                            ); 
                            break;
                        }   
                        response_code = decode_mode(buffer, &temp_bool);

                        if(CHANNEL_check_privilege(clt->channel, clt) == false) 
                        {
                            sprintf(temp_buffer_A, "/mode ERR_CHANOPRIVSNEEDED\n");
                            CHANNEL_send_message_one(clt->channel, clt, temp_buffer_A);
                        } else if(response_code == VALID_PROTOCOL) {
                            CHANNEL_mode(clt->channel, clt, temp_bool);
                        } else {
                            clt_send_message(clt->cl_socket, "/servermsg : Invalid protocol.\n");
                        }
                        break;

                    case ACTION_WHOIS:
                        if(clt->channel == NULL) 
                        {
                            clt_send_message(
                                clt->cl_socket, 
                                "/servermsg : Entre em um canal para continuar!\n\t/join <nome_do_canal>\t: para criar/entrar em nome_do_canal.\n\t/list\t: para ver todos os canais.\n"
                            ); 
                            break;
                        }   
                        
                        response_code = decode_whois(buffer, temp_buffer_A);
                        
                        /* Confirma privilégios */ 
                        if(CHANNEL_check_privilege(clt->channel, clt) == false) 
                        {
                            sprintf(temp_buffer_A, "/whois ERR_CHANOPRIVSNEEDED\n");
                            CHANNEL_send_message_one(clt->channel, clt, temp_buffer_A);
                        } else if(response_code == VALID_PROTOCOL)
                        {    
                            CHANNEL_whois(clt->channel, clt, temp_buffer_A);
                            msg_whois(clt->channel->nickname_admin, clt->nickname, clt->channel->nickname_channel, clt->cl_address);
                        } else 
                        {
                            sprintf(temp_buffer_C, "/whois ERR_NOSUCHNICK %s\n", temp_buffer_A);
                            CHANNEL_send_message_one(clt->channel, clt, temp_buffer_C);
                        }


                        break;
                    
                    case ACTION_INVITE:
                    case ACTION_MUTE:
                    case ACTION_UNMUTE:
                    case ACTION_KICK:
                    case ACTION_UNKICK:
                    if(clt->channel == NULL) 
                    {
                            clt_send_message(
                                clt->cl_socket, 
                                "/servermsg : Entre em um canal para continuar!\n\t/join <nome_do_canal>\t: para criar/entrar em nome_do_canal.\n\t/list\t: para ver todos os canais.\n"
                            ); 
                            break;
                        }   

                        if(action_code == ACTION_INVITE)
                            response_code = decode_invite(buffer, temp_buffer_A);
                        else if(action_code == ACTION_MUTE)
                            response_code = decode_mute(buffer, temp_buffer_A);
                        else if(action_code == ACTION_UNMUTE)
                            response_code = decode_unmute(buffer, temp_buffer_A);
                        else if(action_code == ACTION_KICK)
                            response_code = decode_kick(buffer, temp_buffer_A);
                        else if(action_code == ACTION_UNKICK)
                            response_code = decode_unkick(buffer, temp_buffer_A);
                        
                        /* Confirma privilégios */ 
                        if(CHANNEL_check_privilege(clt->channel, clt) == false) 
                        {
                            memset(temp_buffer_B, 0, sizeof(temp_buffer_B));
                            strncpy(temp_buffer_B, buffer, indexOf(buffer, ' '));
                            sprintf(temp_buffer_C, "%s ERR_CHANOPRIVSNEEDED\n", temp_buffer_B);
                            CHANNEL_send_message_one(clt->channel, clt, temp_buffer_C);
                        } else if(response_code == VALID_PROTOCOL)
                        {    
                            if(action_code == ACTION_INVITE)
                                CHANNEL_invite(clt->channel, clt, temp_buffer_A);
                            else if(action_code == ACTION_MUTE)
                                CHANNEL_mute_user(clt->channel, clt, temp_buffer_A);
                            else if(action_code == ACTION_UNMUTE)
                                CHANNEL_unmute_user(clt->channel, clt, temp_buffer_A);
                            else if(action_code == ACTION_KICK)
                                CHANNEL_kick_user(clt->channel, clt, temp_buffer_A);
                            else if(action_code == ACTION_UNKICK)
                                CHANNEL_unkick_user(clt->channel, clt, temp_buffer_A);
                        } else {
                            sprintf(temp_buffer_C, "/%s ERR_NOSUCHNICK %s\n", buffer, temp_buffer_A);
                            CHANNEL_send_message_one(clt->channel, clt, temp_buffer_C);
                        }
                        break;

                    case ACTION_MESSAGE:
                        if(clt->channel == NULL) 
                        {
                            clt_send_message(
                                clt->cl_socket, 
                                "/servermsg : Entre em um canal para continuar!\n\t/join <nome_do_canal>\t: para criar/entrar em nome_do_canal.\n\t/list\t: para ver todos os canais.\n"
                            ); 
                            break;
                        }   

                        response_code = decode_msg(buffer, temp_buffer_A, temp_buffer_B, temp_buffer_C);
                        
                        if(response_code == VALID_PROTOCOL) 
                        {
                        CHANNEL_msg(clt->channel, clt, temp_buffer_C);
                        } else
                        {
                            clt_send_message(clt->cl_socket, "/servermsg : Invalid protocol.\n");   
                        }  
                        break;

                    case ACTION_PING:
                        clt_send_message(clt->cl_socket, "/servermsg : pong\n"); 
                        break;
                }
                pthread_mutex_unlock(mutex);
            }
            // Ocorreu um erro na conexao...
            else{
                if(clt->channel != NULL)
                {
                    msg_exit_channel(clt->nickname, clt->channel->nickname_channel);
                    CHANNEL_broadcast(clt->channel, clt, MESSAGE_QUIT_CHANNEL, "");
                    CHANNEL_remove_user(clt->channel, clt); 
                }
                return;
            }
        }
    }

    // @Comentários em "client.h"
    int decode_message(char* buffer, char* pack, int index) 
    {

        if(buffer == NULL || *buffer == '\n' || *buffer == '\0')
            return 0;
        
        bzero(pack, MAX_MESSAGE_LENGHT);

        int i = 0, j = 0;
        for(i = index; buffer[i] != '\n' && i < MAX_MESSAGE_LENGHT-1; ++i) 
        {
            pack[j++] = buffer[i];
        }

        pack[j] = '\n';
        pack[j+1] = '\0';

        if(buffer[i] != '\n' || i >= BUFFER_SIZE) 
            return 0;
        return i + 1;
    }

    int indexOf(char* str, char charater) 
    {
        int i = 0;
        while(str[i] != 0) 
        {
            if(str[i] == charater)
                return i;
            ++i;
        }
        return -1;
    }
