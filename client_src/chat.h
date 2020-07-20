 /**
  * #include "chat.h"
  * -----------------
  * 
  * Biblioteca responsável pelas duas principais tarefas do chat.
  * 
  * 1º Controlar Conexões: responsável por gerenciar o socket de
  * comunicação com o servidor designado, além de armazenar em dois 
  * buffers distintos os pacotes recebidos e aguardando envio.
  * 
  * 2º Controle de Estados: responsável por salvar estados e configurações
  * do chat durante sua iteração com o servidor ou com o aplicativo.
  * (Ex: nickname atual, canal atual, é um administrador?, etc...)
  *
  * As mensagens enviadas seguem uma versão adaptada do RFC 1459, contendo o
  * tamanho máximo definido pela constante MAX_MESSAGE_LENGHT e terminados 
  * pelo caracter '\n'
  */

#ifndef RELAY_CHAT_H
    #define RELAY_CHAT_H 1459

    /**
     * Bibliotecas importadas
     * ----------------------
     * 
     * Devido ao uso de threads, é necessário que essa biblioteca seja compilada
     * com o parâmetro -pthread jutno ao g++.
     */

        #include <sys/types.h>
        #include <sys/socket.h>
        #include <netinet/in.h>     /* struct sockaddr_in, */
        #include <netdb.h>          /* gethostbyname(), */
        #include <unistd.h>         /* close, sleep */
        #include <pthread.h>        /* pthread_create, pthread_mutex_t */

    /**
     * Constantes e Macros
     * -------------------
     * 
     * CONNECTION_CLOSED e CONNECTION_OPEN
     * Valores retornados pela função connect(2) e indicam se um socket
     * foi bem sucedido ao abrir uma nova conexão.
     * 
     * MAX_MESSAGE_LENGHT
     * Valor máximo que uma mensagem do protocolo utilizado neste trabalho
     * pode utilizar. Caso seja necessário enviar algo que ultrapasse esse
     * limite a mensagem deve ser quebrada e enviada separadamente. 
     *  
     * BUFFER_SIZE
     * Tamanho que os buffers de receive/send podem ocupar.
     * 
     * DEFAULT_SERVER_HOST e DEFAULT_SERVER_PORT
     * Definem um endereço padrão com quem o servidor se conectará. Por
     * padrão o servidor está localizado no endereço "127.0.0.1:9002"
     *
     * NICKNAME_MAXLENGHT e CHANNEL_NAME_MAXLENGH
     * Valor utilizado como tamanho máximo permitido tanto para o nickname
     * quanto para os nomes de canais.
     * 
     */

        #define CONNECTION_CLOSED -1
        #define CONNECTION_OPEN    0

        #define MAX_MESSAGE_LENGHT 512
        #define BUFFER_SIZE ((MAX_MESSAGE_LENGHT * 30) + 1)

        #define DEFAULT_SERVER_HOST "127.0.0.1"
        #define DEFAULT_SERVER_PORT "9002"
        
        #define NICKNAME_MAXLENGHT       50
        #define CHANNEL_NAME_MAXLENGHT   50
        #define CHANNEL_NAME_SIMBOL      '&'
        #define CHANNEL_ADMIN_SIMBOL     '#'
        #define CHANNEL_USER_SIMBOL      '@'

    /**
     * @struct 
     * typedef struct _relay_chat relay_chat
     * -------------------------------------
     * Struct responsável por centralizar as variáveis de conexão, estado, buffer
     * e também mutexes/conditionals que são necessárias para o funcionamento do
     * client.
     * 
     * Uma vez configurada, deve ser utilizada para abrir/fechar conexões ou
     * atualizar estados como mudança de canal, mudança de nick, etc.
     * 
     * Atributos de conexão
     * --------------------
     * - sserver: string contendo o ip do servidor e é inicializada com o valor definido
     *            em DEFAULT_SERVER_HOST.
     * - sport: similar à sserver, iniciada com o valor definido em DEFAULT_SERVER_PORT
     * - hostent(*): valor retornado por 
     * - port(*): valor numérico definido em sport. 
     * - network_socket(*): indica o socket utilizado para conectar ao servidor.
     * - connection_status(*): indica se há uma conexão aberta, podendo conter o valor de 
     *                         CONNECTION_CLOSED ou CONNECTION_OPEN
     * 
     * Atributos de bufferização
     * -------------------------
     * - send_buff(*): vetor alocado no tamanho de BUFFER_SIZE e que salva as mensagens
     *              em uma fila para serem enviadas ao servidor.
     * - recv_buff(*): vetor idem que salva as mensagens recebidas por servidor em uma
     *              fila para ser processada pelo cliente.
     * - *_buff_size(*): inteiro que realiza o controle de tamanho do buffer.
     * 
     * Mutexes e Conditionals
     * ----------------------
     * - send_mutex(*): mutex utilizado para impedir acesso de escrita simultânea ao 
     *                  send_buff.
     * - recv_mutex(*):  idem para controle do recv_buff
     * - state_mutex(*): caso alguma das variáveis de estado tenha que ser alterada.
     * 
     * - cond_send_waiting(*): utilizada para indicar que uma nova mensagem foi
     *                         adicionada no send_buff. É utilizada pela thread de Input
     *                         para acordar a send_msg_handler.
     * 
     * - cond_recv_waiting(*): utilizada para indicar que uma nova mensagem foi recebida
     *                         do servidor e adicionada ao recv_buff. É utilizada pela thread
     *                         de recv para acordar a output_terminal_handler
     * 
     * 
     * (*) STRUCT DE USO INTERNO, NÃO DEVE SER MANIPULADA DIRETAMENTE.
     *
     * Atributos de estado: 
     * - has_channel:   indica se o usuário já se encontra em algum canal.
     * - has_nick:      indica se o usuário já cadastrou algum nickname.
     * - is_admin:      indica se o usuário é administrador do canal atual.
     * - nickname:      salva o nickname atual do usuário.
     * - channel:       salva o nome do canal atual do usuário.
     *
     * (*) OS ESTADOS SÃO ALTERADOS DE ACORDO COM AS ITERAÇÕES DO USUÁRIO
     *     COM O SERVIDOR.
     */
        typedef struct _relay_chat {
            // atributos de conexão
            char            sserver[24], sport[24];
            struct hostent  *server;
            int             port;
            int             network_socket;
            int             connection_status;

            // atributos de bufferização
            char *send_buff, *recv_buff;
            int  send_buff_size, recv_buff_size;

            // mutexes e conditionals
            pthread_mutex_t* send_mutex;
            pthread_mutex_t* recv_mutex;
            pthread_mutex_t* state_mutex;

            pthread_cond_t*  cond_send_waiting;
            pthread_cond_t*  cond_recv_waiting;
            
            // Variáveis de estado do cliente.
            bool  has_channel,has_nick, is_admin;  
            char  nickname[NICKNAME_MAXLENGHT+1];
            int   nick_len;
            char  channel[CHANNEL_NAME_MAXLENGHT+1];
            int   channel_len;

        } relay_chat;

    /**
     * @function
     * relay_chat criar_relay_chat(&send_mutex, &recv_mutex, &cond_send_waiting, &cond_recv_waiting)
     * ---------------------------------------------------------------------------------------------------
     * 
     * Retorna uma variável do tipo relay_chat com os parâmetros inicializados e buffer alocados
     * vazios. Recebe também a referência dos mutexes e conditionals(*) definidos no escopo global 
     * da main para serem utilizados nas Threads de Conexão.
     * 
     * return: relay_chat com atributos inicializados com valores padrões.
     * 
     * (*) consultar documentação do relay_chat e também README.md
     */
    relay_chat criar_relay_chat(pthread_mutex_t* send_mutex, pthread_mutex_t* recv_mutex, 
            pthread_cond_t* cond_send_waiting, pthread_cond_t* cond_recv_waiting);

    /**
     * @function
     * int destruir_relay_chat(&rc)
     * ----------------------------
     * 
     * Recebe a referência de uma variável relay_chat para ser destruida. Caso haja alguma
     * conexão aberta a mesma é fechada e os buffers são desalocados da memória.
     * 
     * return: 0 caso seja desalocada corretamente ou 1 para caso ocorra algum erro.
     */
    int destruir_relay_chat(relay_chat* rc);

    /**
     * @function
     * int abrir_conexao(&rc)
     * ----------------------
     * 
     * Abre uma conexão com o servidor cujo endereço está definido nos parâmetros rc->sserver
     * e rc->sport. Caso a conexão seja estabelecida corretamente o identificador do socket é
     * salvo em rc->network_socket e o status é atualizado para CONNECTION_OPEN.
     * 
     * return: == 0 para caso a conexão tenha sido aberta corretamente 
     *         >= 1 caso algum erro tenha ocorrido.
     */
    int abrir_conexao(relay_chat* rc);

    /**
     * @function
     * int fechar_conexao(&rc)
     * -----------------------
     * 
     * Fecha a conexão existente com o servidor, reconfigurando os atributos rc->network_socket e
     * o status para CONNECTION_CLOSED.
     * 
     * return: 0 caso a conexão tenha sido fechada com sucesso.
     *         1 caso não tenha sido possível fechar a conexão. 
     */
    int fechar_conexao(relay_chat* rc);

    /**
     * @thread 
     * void* recv_msg_handler(&rc)
     * ---------------------------
     * 
     * Thread responsável por verificar periodicamente se alguma o servidor
     * enviou alguma mensagem ao cliente. Se sim, salva as mensagens no buffer
     * de receive (recv_buffer).
     * 
     * Além disso, transmite um sinal para acordar a thread 'Output Terminal Handler'
     * caso a mesma esteja dormindo e aguardando por atualizações.
     * 
     * Obs: As mensagens recebidas não são processadas, sendo salvas no buffer da forma
     *      exata que foram recebidas pelo socket.
     * 
     */ 
    void* recv_msg_handler(void* rc);

    /**
     * @thread
     * void* send_msg_handler(&rc)
     * ---------------------------
     * 
     * Thread responsável por verificar se há mensagens na fila para serem enviadas
     * ao servidor. Caso afirmativo, remove cada mensagem individualmente do buffer
     * e as envia.
     * 
     * Caso o buffer esteja vazio, um wait é executado e então a thread dorme até que
     * a thread de 'Input Terminal Handler' envie um sinal alertando modificações no buffer.
     * 
     * Obs: as mensagens salvas no buffer não são processadas, devendo ser salvas já no formato
     *      adequado para ser enviado ao servidor.
     * 
     * Exemplo: "Olá pessoas, como vão?\r\nNão sei..\r\nMas e vocês?\r\n\0\0\0\0\0\0\0...."
     * Mensagens contidas no exemplo: "Olá pessoas, como vão?", "Não sei..", "Mãs e vocês?".
     */
    void* send_msg_handler(void* rc);


#endif