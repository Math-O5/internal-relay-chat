#include <iostream>
#include <stdlib.h>
#include "chat.h" /* relay_chat */

using namespace std;

/**
 * CONSTANTES e UTILITÁRIOS
*/
    #define TRUE  1
    #define FALSE 0 


/**
 * VARIÁVEIS DE ESTADO
 */   
    relay_chat RC;


int main(int argc, char *argv[]){

    RC = criar_relay_chat();

    int ret = abrir_conexao(&RC);

    char server_response[6000];
    recv(RC.network_socket, &server_response, sizeof(server_response), 0);
    printf("SERVER's: %s\n", server_response);
    

    fechar_conexao(&RC);
    destruir_relay_chat(&RC);

    return 0;
}




