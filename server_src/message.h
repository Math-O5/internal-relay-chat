#ifndef MESSAGE_H
    #define MESSAGE_H 2020

    /**
     * Esta Biblioteca é responsável por todas as mensagens de log do servidor
     * Cada mensagem tem o seguinte formato:
     * [tipo de log] [data e horário] - conteúdo da mensagem
     * 
     * Tipo de log
     * -----------
     * [x] - Mensagens de erro ou de desconexão de um cliente
     *     Ex.: [x] - [21/07/2000 - 16:30:00] O cliente 10 não responde...
     * [+] - Mensagens de interação que obtiveram sucesso (novas conexões, servidor inicializado, ...)
     *     Ex.: [+] - [21/07/2000 - 16:30:00] Numero maximo de conexoes permitidas: 10
     * [-] - Mensagem de finalização do servidor
     *     Ex.: [-] - [21/07/2000 - 16:30:00] Servidor finalizado com sucesso.
     * [ ] - Sem log
     * 
     * Data e horário
     * --------------
     * Data e horário em que ocorreu o evento
     *     Ex.: 21/07/2000 - 16:30:00
     * 
     * Conteúdo da mensagem
     * --------------------
     * O conteúdo da mensagem vai ser um evento ocorrido ou a própria mensagem que determinado cliente enviou
     *     Exs.: [+] - [21/07/2000 - 16:30:00] Foi enviado pong para o cliente 10.
     *           [ ]
     *           [+] - [21/07/2000 - 16:30:00] <CLIENTE 12>: Olá !!!
    */

    /**
     * Macros e constantes
     * -------------------
     * 
     * CONNECT
     * Valor que indica que o servidor foi inicializado com sucesso
    */
        #define CONNECT 0
    
    /**
     * Bibliotecas importadas
     * ----------------------
    */
        #include <stdio.h>
        #include <stdlib.h> /* system */
        #include <time.h>
        #include <netinet/in.h> /* struct sockaddr_in */

        #include <string>

    using namespace std;
    /**
     * @function
     * 
     * void time_log(char* t_log, int t_log_size)
     * ------------------------------------------
     * 
     * Retorna via parâmetro (char* t_log), a data e horário atual. O tamanho da string retonarda
     * é indicado por t_log_size. A data e o horário atual foi retirado da função localtime() da biblioteca
     * <time.h>. Esta informação será utilizada para o log de cada evento.
     * 
     * Formato da string: "dd/mm/aaaa - hh:mm:ss" 
    */
    void time_log(char* t_log, int t_log_size);

    /**
     * @function
     * 
     * void msg_inicio(int status)
     * ---------------------------
     * 
     * Mensagem de inicialização do servidor. Se o status == CONNECT (0), o servidor foi inicializado
     * com sucesso, e se o status != CONNECT o servidor não foi inicializado..
    */
    void msg_inicio(int status);

    /**
     * @function
     * 
     * void msg_info_server(int port, int sv_socket, int max_conn, int cl_count)
     * -------------------------------------------------------------------------
     * 
     * Mensagem com as informações do servidor: porta, socket do servidor conectado, máximo de conexões
     * e quantidade de clientes conectados
    */
    void msg_info_server(int port, int sv_socket, int max_conn, int cl_count);

    /**
     * @function
     * 
     * void msg_info_client(int id, int socket, struct sockaddr_in cl_addr)
     * --------------------------------------------------------------------
     * 
     * Mensagem com as informaçẽos do cliente: id, socket co cliente conectado, host e porta
    */
    void msg_info_client(int id, int socket, struct sockaddr_in cl_addr);

    /**
     * @function
     * 
     * void msg_recv_cliente(int id, char* buffer)
     * -------------------------------------------
     * 
     * Mensagem que o cliente enviou para o servidor, de acordo com id do cliente
    */
    void msg_recv_cliente(int id, char* buffer);

    /**
     * @function
     * 
     * void msg_send_cliente(int id_emissor, int id_receptor)
     * ------------------------------------------------------
     * 
     * Mensagem entregue ao cliente: cliente que enviou a mensagem (id_emissor) para o cliente
     * que recebeu (id_receptor)
    */
    void msg_send_cliente(int id_emissor, int id_receptor);

    /**
     * @function
     * 
     * void msg_cliente_conexao()
     * --------------------------
     * 
     * Mensagem de nova conexão no servidor
    */
    void msg_cliente_conexao();

    /**
     * @function
     * 
     * void msg_client_no_response(int id)
     * -----------------------------------
     * 
     * Mensagem para indicar que não houve resposta do cliente (int id)...
    */
    void msg_client_no_response(int id);

    /**
     * @function
     * 
     * void msg_cliente_desconexao(int id)
     * -----------------------------------
     * 
     * Mensagem de desconexão do cliente (int id)
    */
    void msg_cliente_desconexao(int id);

    /**
     * @function
     * 
     * void msg_max_client(struct sockaddr_in cl_addr)
     * -----------------------------------------------
     * 
     * Mensagem de erro, que indica que o servidor já está no limite de clientes conectados.
     * A mensagem também mostra as informações od cliente que tentou se conectar (host e porta)
    */
    void msg_max_client(struct sockaddr_in cl_addr);
    
    /**
     * @function
     * 
     * void msg_info_ping(int id)
     * --------------------------
     * 
     * Mensagem que o cliente (int id) enviou o comando ping para o servidor
    */
    void msg_info_ping(int id);

    /**
     * @function
     * 
     * void msg_info_pong(int id)
     * --------------------------
     * 
     * Mensagem avisando que a reposta do comando /ping (pong) foi entregue ao cliente (int id)
    */
    void msg_info_pong(int id);
    
    /**
     * @function
     * 
     * void msg_encerrar_server()
     * --------------------------
     * 
     * Mensagem de finalização do servidor
    */
    void msg_encerrar_server();

#endif