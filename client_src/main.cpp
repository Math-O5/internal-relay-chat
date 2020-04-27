#include <iostream>
#include <stdlib.h>
#include <string.h>     /* memset, strcmp */
#include <pthread.h>    /* pthread_create, pthread_mutex_t */
#include <unistd.h>     /* sleep */

#include "terminal.h"   /* terminal_control */
#include "chat.h"       /* relay_chat */

using namespace std;

/**
 * CONSTANTES e UTILITÁRIOS
*/
    #define TRUE  1
    #define FALSE 0 

    #define ACTION_NONE     999
    #define ACTION_CONNECT  900
    #define ACTION_QUIT     901
    #define ACTION_MESSAGE  902

/**
 * VARIÁVEIS DE ESTADO
 */   
    relay_chat          chat;
    terminal_control    terminal;

    pthread_mutex_t send_mutex     = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t recv_mutex     = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t terminal_mutex = PTHREAD_MUTEX_INITIALIZER;

    // Otimização das threads - condições para acordar do sleep
    pthread_cond_t  cond_recv_waiting = PTHREAD_COND_INITIALIZER;
    pthread_cond_t  cond_send_waiting = PTHREAD_COND_INITIALIZER;

    pthread_t send_msg_thread;
    pthread_t recv_msg_thread;

    pthread_t in_terminal_thread;
    pthread_t out_terminal_thread;

/**
 * Threads de I/O com o Terminal
 */
    void* input_terminal_handler(void*);
    void* output_terminal_handler(void*);

int main(int argc, char *argv[]){

    system("clear");

    /**
     * Iniciando as variáveis globais necessárias;
     */ 
        printf("   [+] Inicializando variáveis....\n"); 
        
        send_mutex     = PTHREAD_MUTEX_INITIALIZER;
        recv_mutex     = PTHREAD_MUTEX_INITIALIZER;
        terminal_mutex = PTHREAD_MUTEX_INITIALIZER;

        cond_recv_waiting = PTHREAD_COND_INITIALIZER;
        cond_send_waiting = PTHREAD_COND_INITIALIZER;

        chat = criar_relay_chat(&send_mutex, &recv_mutex, &cond_send_waiting, &cond_recv_waiting);        
        terminal.terminal_mutex = &terminal_mutex;
        
        printf("   [+] Variáveis iniciadas com sucesso.\n"); 

    /**
     * Abrindo conexão com servidor padrão
     */
        printf("   [+] Abrindo conexão com o servidor 127.0.0.1:9002...\n"); 
        
        if(abrir_conexao(&chat) != 0){
            printf("   [X] ERRO ao abrir conexão com o servidor.\n");
            exit(1);
        } 
           
        printf("   [+] Conexão com o SERVIDOR estabelecida com SUCESSO.\n"); 

    /**
     * Iniciando as Threads do Servidor
     */ 

        printf("   [+] Iniciando Threads de conexão...\n"); 

        if(pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, &chat) != 0){
	        printf("   [x] ERRO ao inicializar o 'Receive Messages Handler'.\n");
        }
        
        if(pthread_create(&send_msg_thread, NULL, &send_msg_handler, &chat) != 0){
	        printf("   [x] Erro ao inicializar o 'Send Messages Handler'\n");
        }

        printf("   [+] THREADS de conexão iniciadas com SUCESSO.\n"); 

    /**
     * Iniciando as Threads de I/O
     */ 
        printf("   [+] Iniciando THREADS  de I/O...\n"); 
        
        if(pthread_create(&out_terminal_thread, NULL, &output_terminal_handler, NULL) != 0){
	        printf("   [x] Erro ao inicializar o 'Output Terminal Handler'.\n");
        }

        if(pthread_create(&in_terminal_thread, NULL, &input_terminal_handler, NULL) != 0) {
	        printf("   [x] Erro ao inicializar o 'Input Terminal Handler'.\n");
        }

        printf("   [+] THREADS  de I/O iniciadas com SUCESSO.\n"); 
        
    // O programa é finalizado quando o usuário
    // finaliza o input por meio de um /quit ou EOF
    pthread_join(in_terminal_thread, NULL);
    
    fechar_conexao(&chat);
    destruir_relay_chat(&chat);

    return 0;
}

void* input_terminal_handler(void*) {

    echo_disable(&terminal);

    int action_code;
    int repeat_loop = 1;

    while(repeat_loop){
        
        action_code = ACTION_NONE;

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
        
        // Executando a ação
        switch (action_code) {

            // Por enquanto faz apenas a conexão padrão
            // case ACTION_CONNECT:
            //     if(abrir_conexao(&chat) != 0){
            //         printf("   [x] Erro ao abrir conexão com o servidor.\n");
            //         repeat_loop = 0;
            //     } else {
            //         printf("   [+] Conexão estabelecida com o servidor.\n");
            //     }
            //     break;
            
            case ACTION_QUIT:
                if(recv_msg_thread >= 0) pthread_cancel(recv_msg_thread);
                if(send_msg_thread >= 0) pthread_cancel(send_msg_thread);
                fechar_conexao(&chat);
                repeat_loop = 0;
                break;

            case ACTION_MESSAGE:
                // pthread_mutex_lock(terminal.terminal_mutex);
                //    printf("  [input]: \"%s\" %d\n", terminal.input, chat.send_buff_size);
                // pthread_mutex_unlock(terminal.terminal_mutex);

                pthread_mutex_lock(chat.send_mutex);
                    pthread_cond_signal(chat.cond_send_waiting); 

                    if(chat.connection_status == CONNECTION_OPEN){
                        
                        if(chat.send_buff_size <= 0){
                            strcpy(chat.send_buff, terminal.input);
                        } else{
                            strcat(chat.send_buff, terminal.input);
                        }
                        strcat(chat.send_buff, "\n");

                        chat.send_buff_size = strlen(chat.send_buff);
                    }
                pthread_mutex_unlock(chat.send_mutex);
                
                // garante que a thread de envio vai conseguir rodar antes de o while
                // de input travar tudo novamente. 
                sleep(1);

                break;
        }

    }
    
    echo_enable(&terminal);
}

void* output_terminal_handler(void*){

    pthread_mutex_lock(terminal.terminal_mutex);
        msg_inicio(&terminal);
    pthread_mutex_unlock(terminal.terminal_mutex);

    int repeat_loop = 1;
    while(repeat_loop){
        
        // 1º - Verifica se existe algo no buffer de received.
        //      Se sim, copia para o buffer de output.
        pthread_mutex_lock(chat.recv_mutex);
            
            if(chat.recv_buff_size <= 0)
                pthread_cond_wait(chat.cond_recv_waiting, chat.recv_mutex); 

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
                int it = 0;
                while(it < chat.recv_buff_size){
                    chat.recv_buff[it++] = '\0';
                }
                chat.recv_buff_size = 0;
            }
        pthread_mutex_unlock(chat.recv_mutex);
        
        // 2º - Verifica se existe algo no buffer de output para ser exibido.
        pthread_mutex_lock(terminal.terminal_mutex);
            if(terminal.buffer_size > 0){

                // Exibe o que foi acumulado no buffer e em seguida o limpa
                printf("   %s\n", terminal.output_buffer);

                int it = 0;
                while(it < terminal.buffer_size){
                    terminal.output_buffer[it++] = '\0';
                }
                terminal.buffer_size = 0;
            }
        pthread_mutex_unlock(terminal.terminal_mutex);

    }
    
}
