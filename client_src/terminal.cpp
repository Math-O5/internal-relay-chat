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
int terminal_loop_handler(terminal_control* t){

    t->input[0] = '\0';
    scanf("%c", t->input);

    if( t->input[0] == '\n' && t->input_enabled == 0 ){

        echo_enable(t);
        t->input[0] = '\0';

        printf("  >> ");
        fflush(stdout); fflush(stdin);
        scanf("%[^\n]", t->input);

        if(strlen(t->input) > 0)
            return 1;
    
    } else if(t->input[0] == '\n' && t->input_enabled == 1){
        echo_disable(t);
    }

    return 0;
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