#include <iostream>
#include <stdlib.h>
#include <string.h>     /* memset, strcmp */
#include <pthread.h>    /* pthread_create, pthread_mutex_t */

#include "terminal.h"   /* terminal_control */
#include "chat.h"       /* relay_chat */

using namespace std;

/**
 * CONSTANTES e UTILITÁRIOS
*/
    #define TRUE  1
    #define FALSE 0 


/**
 * VARIÁVEIS DE ESTADO
 */   
    relay_chat          RC;
    terminal_control    terminal;


    pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t recv_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t terminal_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_t send_msg_thread;
    pthread_t recv_msg_thread;


int main(int argc, char *argv[]){

    // RC = criar_relay_chat(&send_mutex, &recv_mutex);
    // abrir_conexao(&RC);

    // abrindo a thread de leitura
    // if(pthread_create(&recv_msg_thread, NULL, &recv_msg_handler, &RC) != 0)
	// 	printf("ERRO ao iniciar a thread\n");

    // Loop verificando se algo chegou no buffer de entrada
    // while(1){
    //     if(RC.recv_buff_size > 0){
    //         pthread_mutex_lock(RC.recv_mutex);
    //         printf("%s\n",RC.recv_buff);
    //         int it = 0;
    //         while(RC.recv_buff[it] != '\0'){
    //             RC.recv_buff[it++] = '\0';
    //         }
    //         RC.recv_buff_size = 0;
    //         pthread_mutex_unlock(RC.recv_mutex);
    //     }
    // }
    //fechar_conexao(&RC);
    //destruir_relay_chat(&RC);

    terminal_loop_handler(&terminal);

    return 0;
}




