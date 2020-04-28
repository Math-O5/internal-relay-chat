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
    
    // O usuário está requisitando o uso do console.
    if( t->input[0] == '\n'){
        pthread_mutex_lock(t->terminal_mutex);
            echo_enable(t);

            // Exibindo o console de input
            printf("   >> <MSG>: ");
            fflush(stdout); fflush(stdin);

            // resetando primeiro caracter de t->input
            t->input[0] = '\0';

            // Lê o input do usuário e consome o \n enviado logo após o fim do input.
            scanf("%[^\n]", t->input);
            getchar();

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
    printf("   [+]   Relay Chat Client iniciado com sucesso.\n");
    printf("   [ ]   \n");
    printf("   [+]   Comandos disponíveis: \n");
    printf("   [+]   /connect                  - conectar no servidor padrão.\n");
    printf("   [+]   /connect <HOST_IP> <PORT> - conectar em HOST específico\n");
    printf("   [+]   /quit                     - sair do programa\n");
    printf("   [ ]   \n");
    printf("   [+]   Pressione ENTER para digitar algum comando.\n");
    printf("   [ ]   \n");

}