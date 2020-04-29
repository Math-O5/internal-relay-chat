#include <iostream>
#include <stdlib.h>
#include "connection.h" /* server_conn */
#include "message.h"
#include "clients.h"

using namespace std;

#define MAX_CONN 30

server_conn SC;

int main(int argc, char *argv[]){

    /* Criacao do servidor (e inicializacao de suas variaveis) */
    SC = criar_server();

    /* Abertura do servidor */
    int status = abrir_server(&SC, MAX_CONN);
    if(status != 0){
        printf("Ocorreu um erro ao abrir o servidor...\n");
    }

    /* Mensagem inicial do servidor... */
    msg_inicio(status);

    /* RUN SERVER */
    run_server(&SC);

    /* Fechamento do server... */
    status = fechar_server(&SC);
    
    return 0;
}