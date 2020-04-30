#ifndef MESSAGE_H
    #define MESSAGE_H 2020

    #define CONNECT 0
    
    #include <stdio.h>
    #include <stdlib.h> /* system */
    #include <time.h>
    #include <netinet/in.h> /* struct sockaddr_in */

    /* Data e Horario (atuais)*/
    void time_log(char* t_log, int t_log_size);

    /* Mensagem de inicio */
    void msg_inicio(int status);

    /* Informacoes do servidor */
    void msg_info_server(int port, int sv_socket, int max_conn, int cl_count);

    /* Informacoes do cliente */
    void msg_info_client(int id, int socket, struct sockaddr_in cl_addr);

    /* Mensagem recebida pelo cliente */
    void msg_recv_cliente(int id, char* buffer);

    /* Mensagem entregue ao(s) cliente(s) */
    void msg_send_cliente(int id_emissor, int id_receptor);

    /* Conexao de um novo cliente */
    void msg_cliente_conexao(void);

    /* Não recebeu resposta do client */
    void msg_client_no_response(int id);

    /* Desconexao do cliente */
    void msg_cliente_desconexao(int id);

    /* Mensagem de erro ao atingir o numero maximo de clientes no server */
    void msg_max_client(struct sockaddr_in cl_addr);
    
    /* Receive a ping from user */ 
    void msg_info_ping(int id);

    /* Send a pong to user */
    void msg_info_pong(int id);
    
    /* output: Mensagem de finalização do servidor */ 
    void msg_encerrar_server(void);

#endif