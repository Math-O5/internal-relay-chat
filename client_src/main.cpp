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
 *          Mathias Fernandes   ()
 *          João Nagasava       ()
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

using namespace std;

/**
 * Constantes e Macros
 * -------------------
 * 
 * TRUE e FALSE
 * Macros apenas para tornar valores booleanos mais semânticos.
 * 
 * ACTION_*****
 * Valores utilizados para identificar a ação que o usuário deseja
 * realizar durante cada input. A descrição de cada ação está melhor 
 * definida na devida thread de input.
 *  
 *  
 */
    #define TRUE  1
    #define FALSE 0 

    #define ACTION_NONE     999
    #define ACTION_CONNECT  900
    #define ACTION_QUIT     901
    #define ACTION_MESSAGE  902

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
 */   
    relay_chat          chat;
    terminal_control    terminal;

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
    pthread_mutex_t state_mutex     = PTHREAD_MUTEX_INITIALIZER;

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
        
        system("clear");
        echo_enable(&terminal);
        
        /**
         * Iniciando as variáveis globais necessárias;
         */ 
            printf("   [+] Inicializando variáveis....\n"); 
            
            send_mutex     = PTHREAD_MUTEX_INITIALIZER;
            recv_mutex     = PTHREAD_MUTEX_INITIALIZER;
            state_mutex    = PTHREAD_MUTEX_INITIALIZER;

            terminal_mutex = PTHREAD_MUTEX_INITIALIZER;

            cond_recv_waiting = PTHREAD_COND_INITIALIZER;
            cond_send_waiting = PTHREAD_COND_INITIALIZER;

            chat = criar_relay_chat(&send_mutex, &recv_mutex, &cond_send_waiting, &cond_recv_waiting);
            chat.state_mutex = &state_mutex;        
            terminal.terminal_mutex = &terminal_mutex;
            
            printf("   [+] Variáveis iniciadas com sucesso.\n"); 

        /**
         * Abrindo conexão com servidor padrão
         */
            printf("   [+] Abrindo conexão com o servidor 127.0.0.1:9002...\n"); 
            
            if(abrir_conexao(&chat) != 0){
                destruir_relay_chat(&chat);
                printf("   [X] ERRO ao abrir conexão com o servidor.\n");
                exit(EXIT_FAILURE);
            } 
            
            printf("   [+] Conexão com o SERVIDOR estabelecida com SUCESSO.\n"); 

        /**
         * Iniciando as Threads do Servidor
         */ 

            printf("   [+] Iniciando Threads de Conexão...\n"); 

            if(pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, &chat) != 0){
                destruir_relay_chat(&chat);
                printf("   [x] ERRO ao inicializar o 'Receive Messages Handler'.\n");
                exit(EXIT_FAILURE);
            }
            
            if(pthread_create(&send_msg_thread, NULL, &send_msg_handler, &chat) != 0){
                destruir_relay_chat(&chat);
                printf("   [x] Erro ao inicializar o 'Send Messages Handler'\n");
                exit(EXIT_FAILURE);
            }

            printf("   [+] THREADS de Conexão iniciadas com SUCESSO.\n"); 

        /**
         * Iniciando as Threads de I/O
         */ 
            printf("   [+] Iniciando THREADS  de I/O...\n"); 
            
            if(pthread_create(&out_terminal_thread, NULL, &output_terminal_handler, NULL) != 0){
                destruir_relay_chat(&chat);
                printf("   [x] Erro ao inicializar o 'Output Terminal Handler'.\n");
                exit(EXIT_FAILURE);
            }

            if(pthread_create(&in_terminal_thread, NULL, &input_terminal_handler, NULL) != 0) {
                destruir_relay_chat(&chat);
                printf("   [x] Erro ao inicializar o 'Input Terminal Handler'.\n");
                exit(EXIT_FAILURE);
            }

            printf("   [+] THREADS  de I/O iniciadas com SUCESSO.\n"); 


         // Na primeira entrega, como a conexão é feita por padrão então 
        // o join é feito até que o usuário saia ou o 
        pthread_join(recv_msg_thread, NULL);
        printf("   [x] Aviso: perda de conexão com o servidor, utilize /quit para sair.\n");

        // O programa é finalizado quando o usuário
        // finaliza o input por meio de um /quit ou EOF
        pthread_join(in_terminal_thread, NULL);

        fechar_conexao(&chat);
        destruir_relay_chat(&chat);
        
        echo_enable(&terminal);

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

        while(repeat_loop && chat.connection_status == CONNECTION_OPEN){
            
            action_code = ACTION_NONE;

            /**
             * Verificando se o usuário realizou algum input. Em caso afirmativo
             * verifica o tipo de ação pretendida.
             */
            if(terminal_input_iteration(&terminal)){
                
                // Verificando qual ação o input expressa.
                if ( strstr(terminal.input, "/connect") != NULL ){
                    action_code = ACTION_CONNECT;
                } else if ( strstr(terminal.input, "/quit") != NULL ) {
                    action_code = ACTION_QUIT;
                } else {
                    action_code = ACTION_MESSAGE;
                }
            }
            
            /**
             * Definindo qual sequência de ações tomar baseada na ação pretendida 
             * no input do usuário.
             */
            switch (action_code) {

                // @TODO: Para a entrega 2 será necessário realizar a conexão aqui.
                // case ACTION_CONNECT:
                //     if(abrir_conexao(&chat) != 0){
                //         printf("   [x] Erro ao abrir conexão com o servidor.\n");
                //         repeat_loop = 0;
                //     } else {
                //         printf("   [+] Conexão estabelecida com o servidor.\n");
                //     }
                //     break;
                

                /**
                 * ACTION_QUIT (/quit)
                 * --------------------
                 * O usuário pretende finalizar a aplicação. Neste caso basta que
                 * a flag seja iterrompida para a thread ser finalizada e a main iniciar
                 * então os processos de encerramento do programa.
                 */
                case ACTION_QUIT:
                    if(recv_msg_thread >= 0) pthread_cancel(recv_msg_thread);
                    if(send_msg_thread >= 0) pthread_cancel(send_msg_thread);
                    fechar_conexao(&chat);
                    repeat_loop = 0;
                    break;

                /**
                 * ACTION_MESSAGE (/msg)
                 * ---------------------
                 * 
                 * O usuário pretende enviar uma mensagem ao canal em que se encontra.
                 * Neste caso a string deve ser processada e então copiada ao buffer de
                 * envio do chat (chat.send_buff).
                 */
                case ACTION_MESSAGE:

                    pthread_mutex_lock(chat.send_mutex);
                        pthread_cond_signal(chat.cond_send_waiting); 

                        if(chat.connection_status == CONNECTION_OPEN){
                            
                            // Encode Input -> Protocol
                            char** encoded = encode_message(&chat, terminal.input, strlen(terminal.input));

                            if(encoded != NULL){
                                int it = 0;
                                while(encoded[it] != NULL){
                                    if(chat.send_buff_size <= 0){
                                        strcpy(chat.send_buff, encoded[it]);
                                        chat.send_buff_size = strlen(encoded[it]);
                                    } else{
                                        strcat(chat.send_buff, encoded[it]);
                                        chat.send_buff_size += strlen(encoded[it]);
                                    }

                                    free(encoded[it++]);
                                }
                                free(encoded);
                            }

                        }

                        // pthread_mutex_lock(terminal.terminal_mutex);
                        //     printf("  [input]: \"%s\" %d \n%s \n", terminal.input, chat.send_buff_size, chat.send_buff);
                        // pthread_mutex_unlock(terminal.terminal_mutex);

                    pthread_mutex_unlock(chat.send_mutex);


                    // garante que a thread de envio vai conseguir rodar antes de o while
                    // de input travar tudo novamente. 
                    // sleep(1);
                    break;
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
        while(repeat_loop && chat.connection_status == CONNECTION_OPEN){
            
            // 1º - Verifica se existe algo no buffer de received.
            //      Se sim, copia para o buffer de output.
            pthread_mutex_lock(chat.recv_mutex);
                
                // Caso o buffer esteja vazio, a thread dorme até que exista algo 
                // novo para ser processado pela mesma.
                if(chat.recv_buff_size <= 0){ 
                    pthread_cond_wait(chat.cond_recv_waiting, chat.recv_mutex); 
                }

                // printf("\n %d, %d\n", chat.connection_status, chat.recv_buff_size);
                if( chat.connection_status == CONNECTION_OPEN && chat.recv_buff_size > 0 ){
                    
                    // @TODO: Nas próximas etapas terá que haver processamento do Protocolo -> Terminal
                    if(terminal.buffer_size == 0){
                        strcpy(terminal.output_buffer, chat.recv_buff);
                    } else {
                        strcat(terminal.output_buffer, chat.recv_buff);
                    }
                    terminal.buffer_size = strlen(terminal.output_buffer);

                    // Remove elementos retirados do buffer do chat
                    memset(chat.recv_buff, 0, sizeof(chat.recv_buff));
                    chat.recv_buff_size = 0;
                }
                
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
