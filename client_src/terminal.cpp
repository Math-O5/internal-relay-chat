#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "terminal.h"


// Habilita a propriedade ECHO do terminal.
void echo_enable(terminal_control* t){
    struct termios termInfo;
    tcgetattr(0, &termInfo);
    termInfo.c_lflag |= ECHO; 
    tcsetattr(0, TCSANOW, &termInfo);

    t->input_enabled = 1;
}

// Disabilita a propriedade ECHO do terminal.
void echo_disable(terminal_control* t){
    struct termios termInfo;
    tcgetattr(0, &termInfo);
    termInfo.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &termInfo); 

    t->input_enabled = 0;
}

// Handler responsável por lidar com uma iteração de loop no terminal.
int terminal_input_iteration(terminal_control* t){

    memset(t->input, 0, sizeof(t->input));
    t->input[0]  = getchar(); 
    // scanf("%c", t->input);
    
    if( t->input[0] == '\n'){
        
        pthread_mutex_lock(t->terminal_mutex);
            echo_enable(t);

            printf("  >> ");
            fflush(stdout); fflush(stdin);

            scanf("%[^\n]", t->input);
            getchar(); // Consome o \n restante do input

            echo_disable(t);

        pthread_mutex_unlock(t->terminal_mutex);
    }

    return (strlen(t->input) >= 3);
}

void msg_inicio(terminal_control* t){
    system("clear");
    printf("  [+]   Relay Chat Client iniciado com sucesso.\n");
    printf("  [ ]   \n");
    printf("  [+]   Comandos disponíveis: \n");
    printf("  [+]   /connect                  - conectar no servidor padrão.\n");
    printf("  [+]   /connect <HOST_IP> <PORT> - conectar em HOST específico\n");
    printf("  [+]   /quit                     - sair do programa\n");
    printf("  [ ]   \n");
    printf("  [+]   Pressione ENTER para digitar algum comando.\n");
    printf("  [ ]   \n");

}