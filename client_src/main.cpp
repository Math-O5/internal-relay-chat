/**
 * Internet Relay Chat - Client Application
 * ----------------------------------------
 * 
 * Aplicação utilizada para prática e aprendizado de protocolos, sockets
 * e threads utilizando o ambiente POSIX.
 * 
 * documentação: a esquemática geral do projeto está definida no README.md
 *               incluso na raíz do cliente, já as especificações de cada
 *               struct e função declarada se encontra respectivos arquivos.
 *               
 * data: 27/abril/2020 
 * 
 * autores: Gabriel Van Loon    (@GabrielVanLoon)
 *          Mathias Fernandes   (@Math-O5)
 *          João Nagasava       (@JNagasava)
 * 
 * Universidade de São Paulo - Campus São Carlos
 * Instituto de Ciências Matemáticas e de Computação.
 */

#include <iostream>
#include <stdlib.h>
#include <string.h>     /* memset, strcmp */
#include <pthread.h>    /* pthread_create, pthread_mutex_t */
#include <unistd.h>     /* sleep */
#include <signal.h>     /* signal */

#include "terminal.h"   /* terminal_control */
#include "chat.h"       /* relay_chat */
#include "codec.h"    /* encoder/decoder */

using namespace std;

/**
 * Constantes e Macros
 * -------------------
 * 
 * TRUE e FALSE
 * Macros apenas para tornar valores booleanos mais semânticos.
 * 
 * ACTION_***** (ver: codec.h)
 * Valores utilizados para identificar a ação que o usuário deseja
 * realizar durante cada input. A descrição de cada ação está melhor 
 * definida na devida thread de input.
 */
    #define TRUE  1
    #define FALSE 0 

/**
 * Variáveis Globais
 * -----------------
 * 
 * Para o bom funcionamento do projeto é necessário a instância de dois
 * objetos: um relay_chat responsável por manter os estados da aplicação
 * e um terminal_control responsável por garantir uma boa experiência de 
 * I/O com o usuário.
 * 
 * Mais detalhes dessas structs podem ser encontrados em "chat.h" e "terminal.h"
 * 
 * Obs: terminal_backup é utilizada para salvar as configurações do terminal do usuário
 *      antes do programa começar a alterar as configurações. Assim, é possível retornar
 *      tudo ao normal logo ao fim da execução do programa.
 */   
    relay_chat          chat;
    terminal_control    terminal;
    struct termios      terminal_backup;

/**
 * Threads, Mutexes e Conditionals
 * -------------------------------
 * 
 * Essa aplicação consta com 4 threads distintas que garantem que o fluxo de 
 * mensagens entre o cliente e o servidor ocorram de maneira orgânica da melhor
 * maneira possível.
 * 
 * A descrição do funcionamento dessas threads pode ser encontrada no diagrama 
 * encontrado no README.md e também nas documentações das mesmas (as funções 
 * utilizadas são identificadas pela label @thread aqui e em "chat.h").
 */
    pthread_t send_msg_thread;
    pthread_t recv_msg_thread;

    pthread_t in_terminal_thread;
    pthread_t out_terminal_thread;

    pthread_mutex_t send_mutex     = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t recv_mutex     = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t state_mutex    = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_t terminal_mutex = PTHREAD_MUTEX_INITIALIZER;
    
    // Otimização das threads - condições para acordar do sleep
    pthread_cond_t  cond_recv_waiting = PTHREAD_COND_INITIALIZER;
    pthread_cond_t  cond_send_waiting = PTHREAD_COND_INITIALIZER;

/**
 * Declaração de funções
 * ---------------------
 * 
 * As funções aqui declaradas se encontram abaixo da main, assim 
 * como suas devidas especificações.
 */
    void* input_terminal_handler(void*);
    void* output_terminal_handler(void*);
    void  sigint_handler(int signum);

/**
 * @main
 * int main(int argc, char *argv[])
 * --------------------------------
 * 
 * A main neste programa possui o simples papel de inicializar as variáveis,
 * iniciar a execução das threads e garantir que tudo conseguiu executar de maneira
 * adequada. Sendo mais especifico o papel dela é:
 * 
 * 0º   Iniciar MUTEXES e CONDITIONALS.
 * 1º   Instânciar o relay_chat e o terminal_control.
 * (*)  Abrir a CONEXÂO com o servidor padrão.
 * 2º   Iniciar Threads de Conexão (send_msg_thread, recv_msg_thread).
 * 3º   Iniciar Threads de I/O (in_terminal_thread, out_terminal_thread).
 * 
 * Após as devidas configurações, a main entra em modo sleep e aguarda até que a thread
 * de input, que representa a instância de iteração direta com o usuário, seja finalizada.
 * 
 * As variáveis são então destruidas, a conexão, caso exista, é fechada e o programa 
 * é finalizado.
 * 
 * (*) Em versões posteriores, a conexão deverá ser feita sob comando do usuário e essa etapa
 *     deverá ser removida dos afazeres da main().
 */
    int main(int argc, char *argv[]){
        
        // O programa ignora SIGINT
        signal(SIGINT, sigint_handler); 
        
        // Fazendo Backup das configurações do terminal do usuário
        tcgetattr(0, &terminal_backup);

        system("clear");
        
        /**
         * Iniciando as variáveis globais necessárias;
         */ 
            printf(PREFIX_NORMAL); printf("Inicializando variáveis do programa....\n"); 
            
            send_mutex     = PTHREAD_MUTEX_INITIALIZER;
            recv_mutex     = PTHREAD_MUTEX_INITIALIZER;
            state_mutex    = PTHREAD_MUTEX_INITIALIZER;

            terminal_mutex = PTHREAD_MUTEX_INITIALIZER;

            cond_recv_waiting = PTHREAD_COND_INITIALIZER;
            cond_send_waiting = PTHREAD_COND_INITIALIZER;

            chat = criar_relay_chat(&send_mutex, &recv_mutex, &cond_send_waiting, &cond_recv_waiting);
            chat.state_mutex = &state_mutex;        
            terminal.terminal_mutex = &terminal_mutex;
            
            printf(PREFIX_SUCCESS); printf("variáveis iniciadas com sucesso.\n"); 


        /**
         * Iniciando as Threads de I/O
         */ 
            printf(PREFIX_NORMAL); printf("Iniciando THREADS  de I/O...\n"); 
            
            if(pthread_create(&out_terminal_thread, NULL, &output_terminal_handler, NULL) != 0){
                destruir_relay_chat(&chat);
                printf(PREFIX_ERROR); printf("falha ao inicializar o 'Output Terminal Handler'.\n");
                exit(EXIT_FAILURE);
            }

            if(pthread_create(&in_terminal_thread, NULL, &input_terminal_handler, NULL) != 0) {
                destruir_relay_chat(&chat);
                printf(PREFIX_ERROR); printf("falha ao inicializar o 'Input Terminal Handler'.\n");
                exit(EXIT_FAILURE);
            }

            printf(PREFIX_SUCCESS); printf("threads de I/O iniciadas.\n"); 

        // O programa é finalizado quando o usuário
        // finaliza o input por meio de um /quit ou EOF
        pthread_join(in_terminal_thread, NULL);

        // Liberando a variável chat
        destruir_relay_chat(&chat); 

        // Resetando as configurações do terminal do usuário       
        tcsetattr(0, TCSANOW, &terminal_backup);
        printf("\e[m\n"); // Resetando a cor do terminal

        exit(EXIT_SUCCESS);
    }

/**
 * @thread 
 * void* input_terminal_handler(void*)
 * -----------------------------------
 * 
 * Thread responsável por verificar constantemente se o usuário deseja realizar
 * algum input no console. Para isso é utilizado os recursos oferecidos pelo 
 * terminal_control em combinação com a sua função terminal_input_iteration().
 * 
 * Caso algum input seja realizado, a intenção do usuário é interpretada e 
 * identficada por meio de uns dos códigos de ação definidas pelas constantes
 * ACTION_*.
 * 
 * Por consequencia, cada ação determina uma sequência de passos específicas a
 * ser seguida e executada pela thread.
 * 
 * Caso o input do usuário gere algo que deva ser enviado ao servidor, uma requisição
 * ao lock de chat.send_mutex é realizada e os dados são inseridos no Buffer de Envio
 * (chat.send_buffer).
 * 
 * Além disso, um sinal é enviado para a thread 'Send Messages Handler' caso a mesma
 * esteja dormindo devido ao buffer vazio.
 * 
 * action_code: identificador do tipo de ação pretendida pelo usuário
 * 
 * repeat_loop: a thread é executada enquanto a flag se encontra como verdadeira.
 * 
 */ 
    void* input_terminal_handler(void*) {

        echo_disable(&terminal);

        int action_code;
        int repeat_loop = 1;
        char** messages_list = NULL;
        char*  message = NULL;

        while(repeat_loop){
            
            action_code = ACTION_NONE;

            /**
             * Verificando se o usuário realizou algum input. Em caso afirmativo
             * verifica o tipo de ação pretendida por meio do CODEC
             */
            if(terminal_input_iteration(&terminal)){
                action_code = cdc_detectar_act(terminal.input);
            }
            
            /**
             * Definindo qual sequência de ações tomar baseada na ação  
             * pretendida no input do usuário.
             */
            switch (action_code) {
                
                /**
                 * ACTION_CONNECT (/connect)
                 * --------------------
                * Responsável por abrir a conexão do client com um servidor IRC.
                * Existem duas possíveis opções de comando que ativam a ACTION_CONNECT
                * 
                * /connect >> Conecta no endereço e porta padrão definido em chat.h
                * /connect <host> <port> >> Conecta no endereço e porta definida pelo
                *                           próprio usuario.
                * 
                * Caso a conexão seja estabelecida com sucesso, as Threads do Servidor 
                * são iniciadas.
                 */
                case ACTION_CONNECT:
                    
                    // 0º Verifica se já não está conectado em algo
                    if(chat.connection_status == CONNECTION_OPEN) {
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("falha ao conectar, já existe uma conexão ativa.\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }

                    // 1ª Interpretando a mensagem /connect do usuário
                    cdc_encode_connect(&chat, terminal.input, chat.sserver , chat.sport);

                    // 2º Abrindo as Conexões e Threads
                    pthread_mutex_lock(terminal.terminal_mutex);
                        printf(PREFIX_NORMAL);
                        printf("Abrindo conexão com o servidor %s:%s...\n", chat.sserver, chat.sport); 
                    pthread_mutex_unlock(terminal.terminal_mutex);


                    if( abrir_conexao(&chat) != 0 ){
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("não foi possível abrir conexão com o servidor.\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    } 

                    // printf("[+]   | --- Iniciando Threads de Conexão...\n"); 
                    if(pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, &chat) != 0){
                        fechar_conexao(&chat);
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("falha ao inicializar o 'Receive Messages Handler'.\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }
                    if(pthread_create(&send_msg_thread, NULL, &send_msg_handler, &chat) != 0){
                        fechar_conexao(&chat);
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("falha ao inicializar o 'Send Messages Handler'\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }

                    // Deu tudo certo
                    pthread_mutex_lock(terminal.terminal_mutex);
                        printf(PREFIX_SUCCESS);
                        printf("conexão com o servidor %s:%s estabelecida!\n", chat.sserver, chat.sport);
                    pthread_mutex_unlock(terminal.terminal_mutex); 
                    break;

                /**
                 * ACTION_QUIT (/quit)
                 * --------------------
                 * O usuário pretende finalizar a aplicação. Neste caso basta que
                 * a flag seja iterrompida para a thread ser finalizada e a main iniciar
                 * então os processos de encerramento do programa.
                 * 
                 * Obs: Não precisa de Break, pois vai ter que fazer as mesmas etapas 
                 *      do ACTION_DISCONNECT
                 * 
                 * 
                 * ACTION_DISCONNECT (/disconnect)
                 * --------------------
                * É utilizada para caso o cliente queira desconectar do servidor sem necessáriamente
                * fechar o programa. Neste caso, basta fechar a conexão, as threads de servidor e
                * também resetar os buffers.
                 */
                case ACTION_QUIT:
                    repeat_loop = 0;

                case ACTION_DISCONNECT:

                    // 0º Verifica se já não está desconectado
                    if(action_code == ACTION_DISCONNECT && chat.connection_status == CONNECTION_CLOSED){
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("não há nenhuma conexão aberta para ser fechada.\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }

                    // 1º Fechando a conexão
                    fechar_conexao(&chat);

                    // 2º Acorda a Thread sendo para ela morrer
                    pthread_mutex_lock(chat.send_mutex);
                        pthread_cond_signal(chat.cond_send_waiting);
                    pthread_mutex_unlock(chat.send_mutex); 
                    
                    // 3º Cancela as threads
                    pthread_cancel(recv_msg_thread);
                    pthread_cancel(send_msg_thread);

                    // 4º Resetando os buffers
                    // Obs: Threads já morreram mas ainda assim preciso dar 
                    // o lock só para via das dúvidas.
                    pthread_mutex_lock(chat.recv_mutex);
                        chat.recv_buff_size = 0;
                    pthread_mutex_unlock(chat.recv_mutex);

                    pthread_mutex_lock(chat.send_mutex);
                        chat.send_buff_size = 0;
                    pthread_mutex_unlock(chat.send_mutex);
                    
                    pthread_mutex_lock(terminal.terminal_mutex);
                        if(action_code == ACTION_QUIT){ 
                            printf(PREFIX_NORMAL);
                            printf("Até a próxima =D\n");
                        } else {
                            printf(PREFIX_SUCCESS);
                            printf("conexão com o servidor finalizada!\n");
                        }
                    pthread_mutex_unlock(terminal.terminal_mutex);
                    break;

                /**
                 * ACTION_MESSAGE (/msg)
                 * ---------------------
                 * O usuário pretende enviar uma mensagem ao canal em que se encontra.
                 * Neste caso a string deve ser processada e então copiada ao buffer de
                 * envio do chat (chat.send_buff).
                 */
                case ACTION_MESSAGE:

                    // 0º Verifica se já não está desconectado
                    if(chat.connection_status == CONNECTION_CLOSED){
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("ainda não há nenhuma conexão estabelecida.\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }

                    if(!chat.has_nick){
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("você ainda não registrou seu %snickname.\n", COLORB_RED);
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }

                    if(!chat.has_channel){
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("você ainda não entrou em nenhum %scanal.\n", COLORB_RED);
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }

                    pthread_mutex_lock(chat.send_mutex);
                        pthread_cond_signal(chat.cond_send_waiting); 

                        // 1º Codificando a mensagem em 1 ou mais mensagens no formato
                        //    definido pelo protocolo.
                        messages_list = cdc_encode_client_message(&chat, terminal.input, strlen(terminal.input));

                        // 2º Copia as mensagens uma à uma para o send_buffer do chat.
                        //    e vai atualizando o tamanho do buffer.
                        if(messages_list != NULL){
                            int it = 0;
                            while(messages_list[it]   != NULL){
                                if(chat.send_buff_size <= 0){
                                    strcpy(chat.send_buff, messages_list[it]);
                                    chat.send_buff_size = strlen(messages_list[it]);
                                } else{
                                    strcat(chat.send_buff, messages_list[it]);
                                    chat.send_buff_size += strlen(messages_list[it]);
                                }
                                free(messages_list[it++]);
                            }
                            free(messages_list);
                        }

                    pthread_mutex_unlock(chat.send_mutex);
                    break;
                
                case ACTION_PING:
                case ACTION_LIST:

                    // 0º Verifica se já não está desconectado
                    if(chat.connection_status == CONNECTION_CLOSED){
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("ainda não há nenhuma conexão estabelecida.\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }

                    pthread_mutex_lock(chat.send_mutex);
                        pthread_cond_signal(chat.cond_send_waiting);

                            if(action_code == ACTION_PING){
                                strcat(chat.send_buff, "/ping\n");
                                chat.send_buff_size += 6;
                            } else if(action_code == ACTION_LIST){
                                strcat(chat.send_buff, "/list\n");
                                chat.send_buff_size += 6;
                            }

                    pthread_mutex_unlock(chat.send_mutex);
                    break;

                case ACTION_NICK:
                case ACTION_JOIN:
                
                case ACTION_MODE:
                case ACTION_INVITE:
                case ACTION_WHOIS:
                case ACTION_MUTE:
                case ACTION_UNMUTE:
                case ACTION_KICK:
                case ACTION_UNKICK:

                    // 0º Verifica se já não está desconectado
                    if(chat.connection_status == CONNECTION_CLOSED){
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("ainda não há nenhuma conexão estabelecida.\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    }
                    
                    // 1º Decodifica o input e verifica se está tudo ok
                    if(action_code == ACTION_NICK)
                        message = cdc_encode_nickname(&chat, terminal.input);
                    else if(action_code == ACTION_JOIN)
                        message = cdc_encode_join(&chat, terminal.input);

                    if(action_code == ACTION_MODE)
                        message = cdc_encode_mode(&chat, terminal.input);
                    else if(action_code == ACTION_INVITE)
                        message = cdc_encode_invite(&chat, terminal.input);
                    else if(action_code == ACTION_WHOIS)
                        message = cdc_encode_whois(&chat, terminal.input);
                    else if(action_code == ACTION_MUTE)
                        message = cdc_encode_mute(&chat, terminal.input);
                    else if(action_code == ACTION_UNMUTE)
                        message = cdc_encode_unmute(&chat, terminal.input);
                    else if(action_code == ACTION_KICK)
                        message = cdc_encode_kick(&chat, terminal.input);
                    else if(action_code == ACTION_UNKICK)
                        message = cdc_encode_unkick(&chat, terminal.input);

                    if(message == NULL){
                        pthread_mutex_lock(terminal.terminal_mutex);
                            printf(PREFIX_ERROR);
                            printf("falha ao executar comando. Verifique se os parâmetros foram inseridos corretamente!\n");
                        pthread_mutex_unlock(terminal.terminal_mutex);
                        break;
                    } 

                    pthread_mutex_lock(chat.send_mutex);
                        pthread_cond_signal(chat.cond_send_waiting);

                        strcat(chat.send_buff, message);
                        chat.send_buff_size += strlen(message);
                        
                        free(message); 
                        message = NULL;
                        
                    pthread_mutex_unlock(chat.send_mutex);
                    break;

                case ACTION_HELP:
                    pthread_mutex_lock(terminal.terminal_mutex);
                        msg_help(&terminal);



                    pthread_mutex_unlock(terminal.terminal_mutex);
            }
        }
        
        echo_enable(&terminal);
        return NULL;
    }

/**
 * @thread 
 * void* output_terminal_handler(void*)
 * -----------------------------------
 * 
 * Thread responsável por verificar se alguma nova mensagem foi recebida pela thread
 * 'Receive Messages Handler' e inserida no chat.recv_buffer.
 * 
 * Em caso afirmativo, o lock de  (chat.recv_mutex) é solicitado e as mensagens são processadas 
 * e formatadas para serem exibidas corretamente no terminal. Essas mensagens não são exibidas
 * diretamente mas sim salvas no buffer de output do terminal_control.
 * 
 * Após verificar e processar as mensagens, a thread solicita o lock de uso do terminal (para
 * garantir que nenhum output será exibido durante uma inserção do usuário) e descarrega tudo que
 * estava aguardando para ser exibido ao usuário.
 * 
 * Caso o buffer de mensagens recebidas pelo servidor esteja vazio, um wait é executado e a thread
 * dorme até que a thread 'Receive Messages Handler' envie um sinal alertando que novos dados
 * foram inseridos no buffer.
 * 
 */ 
    void* output_terminal_handler(void*){
        
        /**
         * Mensagem de boas vindas default exibidas ao iniciar o programa.
         */
        pthread_mutex_lock(terminal.terminal_mutex);
            msg_inicio(&terminal);
        pthread_mutex_unlock(terminal.terminal_mutex);

        int repeat_loop = 1;
        int buffer_it   = 0, message_it = 0;
        int action_code = ACTION_NONE;
        char message[MAX_MESSAGE_LENGHT+1];

        int response_code = 0;
        char temp_buffer_A[MAX_MESSAGE_LENGHT];
        char temp_buffer_B[MAX_MESSAGE_LENGHT];
        bool temp_bool;
        char temp_buffer_C[2*MAX_MESSAGE_LENGHT];

        
        while(repeat_loop){
            
            // 1º - Verifica se existe algo no buffer de received.
            //      Se sim, faz os seguintes processos:
            //      - Remove mensagem na frente do buffer recv.
            //      - Decodifica a mensagem e atualiza os estados.
            //      - Salva os outputs no buffer de output.
            pthread_mutex_lock(chat.recv_mutex);
                
                // Caso o buffer esteja vazio, a thread dorme até que exista algo 
                // novo para ser processado pela mesma.
                if(chat.recv_buff_size <= 0){ 
                    pthread_cond_wait(chat.cond_recv_waiting, chat.recv_mutex); 
                }

                buffer_it = 0;
                while(buffer_it < chat.recv_buff_size && chat.connection_status == CONNECTION_OPEN){
                    
                    memset(message, 0, sizeof(message));
                    message_it = 0;

                    do{
                        message[message_it++] = chat.recv_buff[buffer_it];
                    } while(chat.recv_buff[buffer_it++] != '\n' && message_it < MAX_MESSAGE_LENGHT);

                    action_code = cdc_detectar_act(message);
                    switch(action_code){

                        case ACTION_NICK:
                            response_code = cdc_decode_nickname(&chat, message, temp_buffer_A);

                            if(response_code == SUCCESS){  
                                strcpy(chat.nickname, temp_buffer_A);
                                chat.nick_len = strlen(temp_buffer_A);
                                chat.has_nick = true;
                                
                                sprintf(temp_buffer_C, "%snickname %s%s%s definido com sucesso!\n", PREFIX_SUCCESS, COLORB_GREEN, chat.nickname, COLOR_GREEN);
                                
                            } else if( response_code == ERR_NICKNAMEINUSE){
                                sprintf(temp_buffer_C, "%so nickname %s%s%s já está sendo utilizado por outro usuário.\n", PREFIX_ERROR, COLORB_RED, temp_buffer_A, COLOR_RED);
                                
                            } else if( response_code == ERR_ERRONEUSNICKNAME){
                                sprintf(temp_buffer_C, "%so nickname %s%s%s possui um formato inválido.\n", PREFIX_ERROR, COLORB_RED, temp_buffer_A, COLOR_RED);
                            }
                            break;

                        case ACTION_JOIN:
                            response_code = cdc_decode_join(&chat, message, temp_buffer_A, &temp_bool, temp_buffer_B);

                            if(response_code == SUCCESS){
                                strcpy(chat.channel, temp_buffer_A);
                                chat.channel_len = strlen(temp_buffer_A);
                                chat.has_channel = true;
                                chat.is_admin    = temp_bool;

                                if(!chat.is_admin)
                                    sprintf(temp_buffer_C, "%svocê entrou no canal %s%s%s.\n", PREFIX_SUCCESS, COLORB_GREEN, temp_buffer_A, COLOR_GREEN);
                                else
                                    sprintf(temp_buffer_C, "%svocê criou o canal %s%s%s e é seu operador!\n", PREFIX_SUCCESS, COLORB_GREEN, temp_buffer_A, COLOR_GREEN);

                            } else if(response_code == RPL_NAMREPLY) { 
                                sprintf(temp_buffer_C, "%s membros do canal: %s%s\n", PREFIX_CHANNEL, COLORB_BLUE, temp_buffer_B);

                            } else if(response_code == ERR_INVITEONLYCHAN) {
                                sprintf(temp_buffer_C, "%sé necessário um CONVITE para entrar neste canal.\n", PREFIX_ERROR);

                            } else if(response_code == ERR_BANNEDFROMCHAN){
                                sprintf(temp_buffer_C, "%svocê foi banido deste canal.\n", PREFIX_ERROR);

                            } else if(response_code == ERR_TOOMANYCHANNELS){
                                sprintf(temp_buffer_C, "%so servidor atingiu a CAPACIDADE MÁXIMA de canais.\n", PREFIX_ERROR);

                            } else if(response_code == ERR_CHANNELISFULL){
                                sprintf(temp_buffer_C, "%so canal está LOTADO, selecione outro ou tente novamente mais tarde.\n", PREFIX_ERROR);

                            }


                            break;

                        case ACTION_LIST:
                            response_code = cdc_decode_list(&chat, message, temp_buffer_A);

                            if(response_code == SUCCESS){
                                sprintf(temp_buffer_C, "%scanais disponíveis: %s\n", PREFIX_SERVER, temp_buffer_A);
                            }

                            break;

                        case ACTION_MODE:
                            response_code = cdc_decode_mode(&chat, message);
                            
                            if(response_code == SUCCESS){
                                sprintf(temp_buffer_C, "%smodo do canal alterado com sucesso!\n", PREFIX_SUCCESS);

                            } else if(response_code == ERR_CHANOPRIVSNEEDED){
                                sprintf(temp_buffer_C, "%svocê não é o %sadministrador%s deste canal.\n", PREFIX_ERROR, COLORB_RED, COLOR_RED);

                            }

                            break;

                        case ACTION_WHOIS:
                            response_code = cdc_decode_whois(&chat, message, temp_buffer_A, temp_buffer_B);
                            
                            if(response_code == SUCCESS){
                                sprintf(temp_buffer_C, "%sWho is %s? IP: %s\n", PREFIX_SUCCESS, temp_buffer_A, temp_buffer_B);

                            } else if(response_code == ERR_CHANOPRIVSNEEDED){
                                sprintf(temp_buffer_C, "%svocê não é o %sadministrador%s deste canal.\n", PREFIX_ERROR, COLORB_RED, COLOR_RED);
                                
                            } else if(response_code == ERR_NOSUCHNICK){
                                sprintf(temp_buffer_C, "%snão foi possível localizar o usuário %s%s%s no canal.\n", PREFIX_ERROR, COLORB_RED, temp_buffer_A, COLOR_RED);

                            }

                            break;

                        case ACTION_INVITE:
                        case ACTION_MUTE:
                        case ACTION_UNMUTE:
                        case ACTION_KICK:
                        case ACTION_UNKICK:

                            if(action_code == ACTION_INVITE)
                                response_code = cdc_decode_invite(&chat, message, temp_buffer_A);
                            else if(action_code == ACTION_MUTE)
                                response_code = cdc_decode_mute(&chat, message, temp_buffer_A);
                            else if(action_code == ACTION_UNMUTE)
                                response_code = cdc_decode_unmute(&chat, message, temp_buffer_A);
                            else if(action_code == ACTION_KICK)
                                response_code = cdc_decode_kick(&chat, message, temp_buffer_A);
                            else if(action_code == ACTION_UNKICK)
                                response_code = cdc_decode_unkick(&chat, message, temp_buffer_A);
                            
                            if(response_code == SUCCESS){
                                
                                if(action_code == ACTION_INVITE)
                                    sprintf(temp_buffer_C, "%sconvite enviado para o usuário %s.\n", PREFIX_SUCCESS, temp_buffer_A);
                                else if(action_code == ACTION_MUTE)
                                    sprintf(temp_buffer_C, "%so usuário %s foi silenciado.\n", PREFIX_SUCCESS, temp_buffer_A);
                                else if(action_code == ACTION_UNMUTE)
                                    sprintf(temp_buffer_C, "%so usuário %s foi desilenciado.\n", PREFIX_SUCCESS, temp_buffer_A);
                                else if(action_code == ACTION_KICK)
                                    sprintf(temp_buffer_C, "%so usuário %s foi banido do canal.\n", PREFIX_SUCCESS, temp_buffer_A);
                                else if(action_code == ACTION_UNKICK)
                                    sprintf(temp_buffer_C, "%so usuário %s foi perdoado pelo operador do canal.\n", PREFIX_SUCCESS, temp_buffer_A);

                            } else if(response_code == ERR_CHANOPRIVSNEEDED){
                                sprintf(temp_buffer_C, "%svocê não é o %sadministrador%s deste canal.\n", PREFIX_ERROR, COLORB_RED, COLOR_RED);
                                
                            } else if(response_code == ERR_NOSUCHNICK){
                                sprintf(temp_buffer_C, "%snão foi possível localizar o usuário %s%s%s.\n", PREFIX_ERROR, COLORB_RED, temp_buffer_A, COLOR_RED);
                            }
                            break;

                        case ACTION_MESSAGE:
                            response_code = cdc_decode_client_message(&chat, message, temp_buffer_A, temp_buffer_B);
                            
                            if(response_code == SUCCESS){
                                sprintf(temp_buffer_C, "%s%s%s:%s %s\n", PREFIX_MESSAGE, COLORB_YELLOW, temp_buffer_A, COLOR_WHITE, temp_buffer_B);
                            }
                            break;

                        case ACTION_SERVERMSG:
                            response_code = cdc_decode_server_message(&chat, message, temp_buffer_A);
                            
                            if(response_code == SUCCESS){
                                sprintf(temp_buffer_C, "%s%s\n", PREFIX_SERVER, temp_buffer_A);
                            }
                            break;

                        case ACTION_CHANNELMSG:
                            response_code = cdc_decode_channel_message(&chat, message, temp_buffer_A);
                            
                            if(response_code == SUCCESS){
                                sprintf(temp_buffer_C, "%s%s\n", PREFIX_CHANNEL, temp_buffer_A);
                            }
                            break;

                    }// fim switch case;

                    // Copiando as mensagens para o buffer de saída
                    strcat(terminal.output_buffer, temp_buffer_C);
                    terminal.buffer_size += strlen(temp_buffer_C);
                    memset(temp_buffer_C, 0 , sizeof(temp_buffer_C));
                }

                // Se chegou aqui, recv_buffer já foi totalmente decodificado então
                memset(chat.recv_buff, 0, sizeof(chat.recv_buff));
                chat.recv_buff_size = 0;

            pthread_mutex_unlock(chat.recv_mutex);
            
            // 2º - Verifica se existe algo no buffer de output para ser exibido.
            pthread_mutex_lock(terminal.terminal_mutex);
                if(terminal.buffer_size > 0){

                    // Exibe o que foi acumulado no buffer e em seguida o limpa
                    printf("%s", terminal.output_buffer);
                    fflush(stdout);
                    
                    memset(terminal.output_buffer, 0, sizeof(terminal.output_buffer));
                    terminal.buffer_size = 0;
                    
                }
            pthread_mutex_unlock(terminal.terminal_mutex);

        }
        
        return NULL;
    }

/**
 * @function
 * void  sigint_handler(signum)
 * ---------------------------
 * 
 * O client não pode ser terminado por um SIGINT (CTRL+C), então esta 
 * função é implementada apenas para capturar e ignorar este sinal.
 */
    void  sigint_handler(int signum){
        return;
    }
