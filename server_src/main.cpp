
/**
 * Bibliotecas importadas
 * ----------------------
*/
#include <iostream>
#include <stdlib.h>
#include <signal.h>

/**
 * Bibliotecas do própio projeto
 * -----------------------------
 * 
 * #include "connection.h"
 * -----------------------
 * 
 * Biblioteca responsável pelo servidor (inicializá-lo, abrir conexão, aceitar conexões
 * dos clientes, gerenciá-los, remover conexões e finalizá-lo)
 * 
 * #include "clients.h"
 * --------------------
 * 
 * Biblioteca que gerencia as ações dos clientes (recebe as mensagens, identifica comandos, responde
 * comandos e envia mensagens para clientes)
 * 
 * #include "message.h"
 * --------------------
 * 
 * Biblioteca com as mensagens de log do servidor
*/
#include "connection.h" /* server_conn */
#include "clients.h"
#include "message.h"

using namespace std;

/**
 * Macros e constantes
 * -------------------
 * 
 * MAX_CONN
 * Número solicitado para o máximo de conexões aceitas para o servidor
*/
#define MAX_CONN 3

/**
 * Variáveis globais
 * -----------------
 * 
 * server_conn SC
 * contém as informaçẽos do servidor
*/
server_conn SC;

int main(int argc, char *argv[]){

    system("clear");

    // Execução da função conn_catch_ctrl_c_and_exit() [connection.h] para quando for pressionado CTRL+C
    // para a finalização do servidor
    signal(SIGINT, conn_catch_ctrl_c_and_exit);

    // Criacao do servidor (e inicializacao de suas variaveis)
    SC = conn_criar_server();

    // Abertura do servidor
    int status = conn_abrir_server(&SC, MAX_CONN);
    if(status != 0){
        printf("Ocorreu um erro ao abrir o servidor...\n");
    }

    // Mensagem inicial do servidor...
    msg_inicio(status);

    // Executar o servidor
    conn_run_server(&SC);
    
    return 0;
}