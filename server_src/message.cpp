#include "message.h"

/* Data e Horario (atuais)*/
void time_log(char* t_log, int t_log_size){
    struct tm *sTime;
    time_t cur_time = time(0);
    sTime = localtime(&cur_time);
    strftime(t_log, t_log_size, "%d-%m-%Y %H:%M:%S", sTime);
}

void msg_inicio(int status) {
    if(status == CONNECT) {
        // system("clear");
        printf("  [ ]   ==========================\n");
        printf("  [+]   Welcome Relay Chat Server.\n");
        printf("  [ ]   ==========================\n");
        fflush(stdout);
    } else {
        // system("clear");
        printf("  [ ]   ==========================\n");
        printf("  [x]   The Relay Chat Server IS NOT RUNNING.\n");
        printf("  [ ]   ==========================\n");
        fflush(stdout);
    }
}

// Informacoes do servidor
void msg_info_server(int port, int sv_socket, int max_conn, int cl_count){

    char t_log[20];
    printf("  [ ]\n");
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Informacoes do Servidor:\n", t_log);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Port %d\n", t_log, port);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Socket Server: %d\n", t_log, sv_socket);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Numero maximo de conexoes permitidas: %d\n", t_log, max_conn); 
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Clientes conectados: %d\n", t_log, cl_count); 
    printf("  [ ]\n"); 
}

/* Informacoes do cliente */
void msg_info_client(int id, int socket, struct sockaddr_in cl_addr){

    char t_log[20];
    printf("  [ ]\n");
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Informacoes do cliente(%d)\n", t_log, id);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Id: %d\n", t_log, id);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Socket: %d\n", t_log, socket);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Host: %d.%d.%d.%d\n",
                                    t_log,
                                    (cl_addr.sin_addr.s_addr & 0xff),
                                    (cl_addr.sin_addr.s_addr & 0xff00) >> 8,
                                    (cl_addr.sin_addr.s_addr & 0xff0000) >> 16,
                                    (cl_addr.sin_addr.s_addr & 0xff000000) >> 24);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Port: %d\n", t_log, cl_addr.sin_port);
    printf("  [ ]\n");
}

/* Mensagem recebida pelo cliente */
void msg_recv_cliente(int id, char* buffer){
    char t_log[20];
    printf("  [ ]\n");
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O cliente %d enviou uma mensagem/comando...\n", t_log, id);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] <CLIENTE %d>: %s", t_log, id, buffer);
    printf("  [ ]\n");
}

/* Mensagem entregue ao(s) cliente(s) */
void msg_send_cliente(int id_emissor, int id_receptor){
    char t_log[20];
    printf("  [ ]\n");
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] A mensagem do cliente %d foi entregue ao cliente %d...\n", t_log, id_emissor, id_receptor);
    printf("  [ ]\n");
}

/* Conexao de um novo cliente */
void msg_cliente_conexao(){
    char t_log[20];
    printf("  [ ]\n");
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Novo cliente no servidor...\n", t_log);
    printf("  [ ]\n");    
}

/* Desconexao do cliente */
void msg_cliente_desconexao(int id){
    char t_log[20];
    printf("  [ ]\n");
    time_log(t_log, sizeof(t_log));
    printf("  [x] - [%s] O cliente %d se desconectou...\n", t_log, id);
    printf("  [ ]\n");
}

/* Mensagem de erro ao atingir o numero maximo de clientes no server */
void msg_max_client(struct sockaddr_in cl_addr){
    char t_log[20];
    printf("  [ ]\n");
    time_log(t_log, sizeof(t_log));
    printf("  [x] - [%s] O numero maximo de clientes foi excedido\n", t_log);
    time_log(t_log, sizeof(t_log));
    printf("  [x] - [%s] Informacoes do cliente que tentou conectar com o servidor:\n", t_log);
    time_log(t_log, sizeof(t_log));
    printf("  [x] - [%s] Host: %d.%d.%d.%d\n",
                                    t_log,
                                    (cl_addr.sin_addr.s_addr & 0xff),
                                    (cl_addr.sin_addr.s_addr & 0xff00) >> 8,
                                    (cl_addr.sin_addr.s_addr & 0xff0000) >> 16,
                                    (cl_addr.sin_addr.s_addr & 0xff000000) >> 24);
    time_log(t_log, sizeof(t_log));
    printf("  [x] - [%s] Port: %d\n", t_log, cl_addr.sin_port);
    printf("  [ ]\n");
}