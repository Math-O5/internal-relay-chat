#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "terminal.h"


// @Comentários em "terminal.h"
void echo_enable(terminal_control* t){
    struct termios termInfo;
    tcgetattr(0, &termInfo);
    termInfo.c_lflag |= ECHO; 
    tcsetattr(0, TCSANOW, &termInfo);

    t->input_enabled = INPUT_ENABLED;
}

// @Comentários em "terminal.h"
void echo_disable(terminal_control* t){
    struct termios termInfo;
    tcgetattr(0, &termInfo);
    termInfo.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &termInfo); 

    t->input_enabled = INPUT_DISABLED;
}

// @Comentários em "terminal.h"
int terminal_input_iteration(terminal_control* t){

    memset(t->input, 0, sizeof(t->input));
    t->input[0]  = getchar(); 
    
    // O sinal EOF indica que o usuário que sair
    if( t->input[0] == EOF) {

        strcpy(t->input,"/quit");
        return 1;

    // O usuário está requisitando o uso do console.
    } else if( t->input[0] == '\n'){
        pthread_mutex_lock(t->terminal_mutex);
            echo_enable(t);

            // Exibindo o console de input
            printf("#INPUT: ");
            fflush(stdout); fflush(stdin);

            // resetando primeiro caracter de t->input
            t->input[0] = '\0';

            // Lê o input do usuário e consome o \n enviado logo após o fim do input.
            if( scanf("%[^\n]", t->input) == EOF){
                printf("\n"); 
                strcpy(t->input,"/quit");
                return 1;
            }
            getchar();
            
            // Utilize a linha abaixo para remover a linha de output do terminal 
            printf("\033[A\33[2K\r");

            echo_disable(t);

        pthread_mutex_unlock(t->terminal_mutex);

        return (strlen(t->input) >= 1);
    }
    // Fim do uso do console. Caso o usuário envie uma mensagem de tamanho
    // nulo a iteração é ignorada.

    return 0;
}

// @Comentários em "terminal.h"
void msg_inicio(terminal_control* t){

    system("clear");
    printf("[+]   Internet Relay Chat - Client\n");
    printf("[ ]   \n");
    printf("[+]   Comandos de Controle: \n");
    printf("[+]   | --- /connect                  - Conectar em DEFAULT_HOST:DEFAULT_PORT\n");
    printf("[+]   | --- /connect <HOST_IP> <PORT> - Conectar em <HOST_IP>:<PORT>\n");
    printf("[+]   | --- /disconnect               - Fecha a conexão com o servidor atual\n");
    printf("[+]   | --- /ping                     - Testa a conexão com o servidor\n");
    printf("[+]   | --- /quit ou Ctrl+D (EOF)     - Fecha o programa\n");
    printf("[ ]   \n");
    printf("[+]   Pressione ENTER para habilitar/desabilitar o INPUT.\n");
    printf("[ ]   \n");

}