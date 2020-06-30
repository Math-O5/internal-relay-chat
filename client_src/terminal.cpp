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
            // printf("#INPUT: ");
            printf(PREFIX_INPUT);
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
    printf("%s\n", COLORB_YELLOW);
    printf("   ██████╗ ███████╗██╗      █████╗ ██╗   ██╗     ██████╗██╗  ██╗ █████╗ ████████╗\n");
    printf("   ██╔══██╗██╔════╝██║     ██╔══██╗╚██╗ ██╔╝    ██╔════╝██║  ██║██╔══██╗╚══██╔══╝\n");
    printf("   ██████╔╝█████╗  ██║     ███████║ ╚████╔╝     ██║     ███████║███████║   ██║   \n");
    printf("   ██╔══██╗██╔══╝  ██║     ██╔══██║  ╚██╔╝      ██║     ██╔══██║██╔══██║   ██║   \n");
    printf("   ██║  ██║███████╗███████╗██║  ██║   ██║       ╚██████╗██║  ██║██║  ██║   ██║   \n");
    printf("   ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝   ╚═╝        ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   \n");
    printf("                                           powered by VanLoon, Math05, JNagasava.\n");
    printf("\n"); 

    printf(PREFIX_NORMAL); printf("%sLista de Comandos Principais:%s\n", COLORB_WHITE, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/connect%s                  - Conectar em DEFAULT_HOST:DEFAULT_PORT.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/connect <HOST_IP> <PORT>%s - Conectar em HOST_IP:PORT.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/disconnect%s               - Fecha a conexão com o servidor atual.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/ping%s                     - Testa a conexão com o servidor.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/help%s                     - Ver lista completa de comandos.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/quit ou Ctrl+D (EOF)%s     - Fecha o programa.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_EMPTY_LINE);
    printf(PREFIX_NORMAL); printf("Pressione %sENTER%s para habilitar/desabilitar o modo %sINPUT [$]:\n", COLORB_YELLOW, COLOR_WHITE, COLORB_YELLOW);
    printf(PREFIX_EMPTY_LINE);

}

void msg_help(terminal_control* t){

    printf(PREFIX_NORMAL); printf("%sLista de Comandos Gerais:%s\n", COLORB_WHITE, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/connect%s                  - Conectar em DEFAULT_HOST:DEFAULT_PORT.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/connect <HOST_IP> <PORT>%s - Conectar em HOST_IP:PORT.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/disconnect%s               - Fecha a conexão com o servidor atual.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/ping%s                     - Testa a conexão com o servidor.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/help%s                     - Ver lista completa de comandos.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/quit ou Ctrl+D (EOF)%s     - Fecha o programa.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_EMPTY_LINE);

    printf(PREFIX_NORMAL); printf("%sLista de Comandos do Servidor%s\n", COLORB_WHITE, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/nickname <NICK>%s          - Define ou altera seu nickname no servidor.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/join <CHANNEL>%s           - Entra em um canal existente ou cria um novo.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/list%s                     - Lista todos os canais ativos no servidor.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_EMPTY_LINE);

    printf(PREFIX_NORMAL); printf("%sLista de Comandos do Operador%s\n", COLORB_WHITE, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/mode <+i/-i>%s             - Altera o canal para 'público' (-i) ou invite-only (+i).\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/invite <NICK>%s            - Convida um usuário do servidor para o canal.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/whois <NICK>%s             - Retorna os dados do usuário.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/mute <NICK>%s              - Silencia o usuário no canal.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/unmute <NICK>%s            - Remove a restrição de silêncio do usuário.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/kick <NICK>%s              - Bane e remove um usuário do canal.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_NORMAL); printf("| --- %s/unkick <NICK>%s            - Revoga o banimento do usuário.\n", COLORB_YELLOW, COLOR_WHITE);
    printf(PREFIX_EMPTY_LINE);

    printf(PREFIX_NORMAL); printf("Pressione %sENTER%s para dispensar a ajuda.\n", COLORB_YELLOW, COLOR_WHITE);
    getchar();

    for(int i = 0; i < 23; i++){
        printf("\033[A\33[2K\r");
    }
}





    // BACKUP para visualizar as cores
    // printf("  \033[0;30m[+]   Comandos de Controle: \n");
    // printf("  \033[0;31m[+]   Comandos de Controle: \n");
    // printf("  \033[0;32m[+]   Comandos de Controle: \n");
    // printf("  \033[0;33m[+]   Comandos de Controle: \n");
    // printf("  \033[0;34m[+]   Comandos de Controle: \n");
    // printf("  \033[0;35m[+]   Comandos de Controle: \n");
    // printf("  \033[0;36m[+]   Comandos de Controle: \n");
    // printf("  \033[0;37m[+]   Comandos de Controle: \n");
    // printf("  [ ]   \n");
    // printf("  \033[1;30m[+]   Comandos de Controle: \n");
    // printf("  \033[1;31m[+]   Comandos de Controle: \n");
    // printf("  \033[1;32m[+]   Comandos de Controle: \n");
    // printf("  \033[1;33m[+]   Comandos de Controle: \n");
    // printf("  \033[1;34m[+]   Comandos de Controle: \n");
    // printf("  \033[1;35m[+]   Comandos de Controle: \n");
    // printf("  \033[1;36m[+]   Comandos de Controle: \n");
    // printf("  \033[1;37m[+]   Comandos de Controle: \n");
    // printf("%s\n", COLOR_RESET);

    // backup logo com USP no inicio
    // printf("%s\n\n", COLORB_YELLOW);
    // printf("   ██╗   ██╗███████╗██████╗     ██████╗ ███████╗██╗      █████╗ ██╗   ██╗     ██████╗██╗  ██╗ █████╗ ████████╗\n");
    // printf("   ██║   ██║██╔════╝██╔══██╗    ██╔══██╗██╔════╝██║     ██╔══██╗╚██╗ ██╔╝    ██╔════╝██║  ██║██╔══██╗╚══██╔══╝\n");
    // printf("   ██║   ██║███████╗██████╔╝    ██████╔╝█████╗  ██║     ███████║ ╚████╔╝     ██║     ███████║███████║   ██║   \n");
    // printf("   ██║   ██║╚════██║██╔═══╝     ██╔══██╗██╔══╝  ██║     ██╔══██║  ╚██╔╝      ██║     ██╔══██║██╔══██║   ██║   \n");
    // printf("   ╚██████╔╝███████║██║         ██║  ██║███████╗███████╗██║  ██║   ██║       ╚██████╗██║  ██║██║  ██║   ██║   \n");
    // printf("    ╚═════╝ ╚══════╝╚═╝         ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝   ╚═╝        ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   \n");
    // printf("                                                                        powered by VanLoon, Math05, JNagasava.\n");
    // printf("\n"); 