#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "connection.h" /* server_conn */
#include "message.h"
#include "clients.h"
#include "connection.h"

using namespace std;

#define MAX_CONN 3

server_conn SC;

int main(int argc, char *argv[]){

    system("clear");

    signal(SIGINT, conn_catch_ctrl_c_and_exit);

    /* Criacao do servidor (e inicializacao de suas variaveis) */
    SC = conn_criar_server();

    /* Abertura do servidor */
    int status = conn_abrir_server(&SC, MAX_CONN);
    if(status != 0){
        printf("Ocorreu um erro ao abrir o servidor...\n");
    }

    /* Mensagem inicial do servidor... */
    msg_inicio(status);

    /* RUN SERVER */
    conn_run_server(&SC);

    /* Fechamento do server... */
    // status = fechar_server(&SC);
    
    return 0;
}