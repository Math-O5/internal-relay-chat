#include <iostream>
#include <stdlib.h>
#include "connection.h" /* server_conn */

using namespace std;

#define MAX_CONN 10

server_conn SC;

int main(int argc, char *argv[]){

    SC = criar_server();

    int res = abrir_server(&SC, MAX_CONN);
    res = info_server(&SC);
    res = fechar_server(&SC);
    run_server(&SC);
    
    res = info_server(sv);
    msg_inicio(res);
    
    return 0;
}