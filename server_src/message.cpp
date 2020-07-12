#include "message.h"

// @Comentários em "message.h"
void time_log(char* t_log, int t_log_size)
{
    struct tm *sTime;
    time_t cur_time = time(0);
    sTime = localtime(&cur_time);
    strftime(t_log, t_log_size, "%d-%m-%Y %H:%M:%S", sTime);
}

// @Comentários em "message.h"
void msg_inicio(int status)
{
    if(status == CONNECT) 
    {
        // system("clear");
        printf("  [ ]   ==========================\n");
        printf("  [+]   Welcome Relay Chat Server.\n");
        printf("  [ ]   ==========================\n");
        fflush(stdout);
    } else 
    {
        // system("clear");
        printf("  [ ]   ==========================\n");
        printf("  [x]   The Relay Chat Server IS NOT RUNNING.\n");
        printf("  [ ]   ==========================\n");
        fflush(stdout);
    }
}



// @Comentários em "message.h"
void msg_info_server(int port, int sv_socket, int max_conn, int cl_count)
{
    char t_log[20];
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
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_info_client(int id, int socket, struct sockaddr_in cl_addr)
{
    char t_log[20];
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
}

// @Comentários em "message.h"
void msg_recv_cliente(int id, char* buffer)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O cliente %d enviou uma mensagem/comando...\n", t_log, id);
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] <CLIENTE %d>: %s", t_log, id, buffer);
}

// @Comentários em "message.h"
void msg_send_cliente(int id_emissor, int id_receptor){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] A mensagem do cliente %d foi entregue ao cliente %d...\n", t_log, id_emissor, id_receptor);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_cliente_conexao()
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Novo cliente no servidor...\n", t_log);
    printf("  [ ]\n");    
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_client_no_response(int id) 
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [x] - [%s] O cliente %d não responde...\n", t_log, id);
    fflush(stderr);
}

// @Comentários em "message.h"
void msg_client_no_response_channel(int id, char* nickname, char* channel) 
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [x] - [%s] O cliente %s(%d) não responde e foi removido do canal %s...\n", t_log, nickname, id, channel);
    fflush(stderr);
}

// @Comentários em "message.h"
void msg_cliente_desconexao(int id){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [x] - [%s] O cliente %d se desconectou...\n", t_log, id);
    fflush(stderr);
}

// @Comentários em "message.h"
void msg_max_client(struct sockaddr_in cl_addr){
    char t_log[20];
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
}

// @Comentários em "message.h"
void msg_info_ping(int id) {
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Recebido o ping do cliente %d.\n", t_log, id);
    fflush(stdout);
} 

// @Comentários em "message.h"
void msg_info_pong(int id) {
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Foi enviado pong para o cliente %d.\n", t_log, id);
    fflush(stdout);
} 

// @Comentários em "message.h"
void msg_nickname_cliente(int id, const char* newNickname, char* oldNickname)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O cliente %d mudou o nickname de %s para %s...\n", t_log, id, oldNickname, newNickname);
}

void msg_join_channel(struct _client* clt) {
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O %s %d entrou no canal %s.\n", t_log, clt->nickname, clt->cl_id, clt->channel->nickname_channel);
}

// @Comentários em "message.h"
void msg_encerrar_server() {
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [-] - [%s] Servidor finalizado com sucesso.\n", t_log);
    fflush(stdout);
}


// ############################################################ PARTE 3 ############################################################

// ******************************* MENSAGENS GERAIS *******************************

// @Comentários em "message.h"
void msg(char type, char* msg)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [%c] - [%s] %s\n", type, t_log, msg);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_client_channel(const char* nickname, const char* channel)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] %s enviou uma mensagem para o canal %s...\n", t_log, nickname, channel);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_channel_client(const char* nickname, const char* channel)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O canal %s enviou uma mensagem para o usuario %s...\n", t_log, channel, nickname);
    fflush(stdout);
}
// ******************************* LIST *******************************

// @Comentários em "message.h"
void msg_list(char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] Foi listado para o usuario '%s' a listagem dos canais do servidor.\n", t_log, nickname);
    fflush(stdout);
}

// ******************************* JOIN *******************************

// @Comentários em "message.h"
void msg_create_channel(char* nickname, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O canal '%s' foi criado pelo usuario '%s'.\n", t_log, channel_name, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_join_channel(char* nickname, char* channel_name)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O usuario '%s' deu join no canal '%s'.\n", t_log, nickname, channel_name);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_exit_channel(char* nickname, char* channel_name)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [-] - [%s] O usuario '%s' saiu do canal '%s'.\n", t_log, nickname, channel_name);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_destroy_channel(char* channel_name)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [-] - [%s] O canal '%s' foi terminado pois nao haviam mais participantes.\n", t_log, channel_name);
    fflush(stdout);
}

// ******************************* NICKNAME *******************************

// @Comentários em "message.h"
void msg_create_nickname(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O cliente %d criou seu nickname: %s.\n", t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_change_nickname(char* nickname, char* oldnickname)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O usuario %s alterou seu nickname para %s.\n", t_log, nickname, oldnickname);
    fflush(stdout);
}

// ******************************* KICK *******************************

// @Comentários em "message.h"
void msg_kick(char* nickname_adm, char* nickname_user, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [-] - [%s] O usuario '%s' foi banido do canal '%s' pelo administrador '%s'.\n", t_log, nickname_user, channel_name, nickname_adm);
    fflush(stdout);
}

// ******************************* UNKICK *******************************

// @Comentários em "message.h"
void msg_unkick(char* nickname_adm, char* nickname_user, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O usuario '%s' voltou a ser permitido pelo administrador '%s' a participar do canal '%s'.\n", t_log, nickname_user, nickname_adm, channel_name);
    fflush(stdout);
}

// ******************************* MUTE *******************************

// @Comentários em "message.h"
void msg_mute(char* nickname_adm, char* nickname_user, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [-] - [%s] O usuario '%s' foi silenciado pelo administrador '%s' no canal '%s'.\n", t_log, nickname_user, nickname_adm, channel_name);
    fflush(stdout);
}

// ******************************* UNMUTE *******************************

// @Comentários em "message.h"
void msg_unmute(char* nickname_adm, char* nickname_user, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O usuario '%s' nao esta mais silenciado pelo administrador '%s' no canal '%s'.\n", t_log, nickname_user, nickname_adm, channel_name);
    fflush(stdout);
}

// ******************************** WHOIS *******************************

// @Comentários em "message.h"
void msg_whois(char* nickname_adm, char* nickname_user, char* channel_name, struct sockaddr_in cl_addr){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O administrador '%s' do canal '%s' solicitou o IP do usuario '%s' o qual eh %d.%d.%d.%d .\n",
        t_log, 
        nickname_user, 
        channel_name, 
        nickname_adm, 
        (cl_addr.sin_addr.s_addr & 0xff),
        (cl_addr.sin_addr.s_addr & 0xff00) >> 8,
        (cl_addr.sin_addr.s_addr & 0xff0000) >> 16,
        (cl_addr.sin_addr.s_addr & 0xff000000) >> 24);
    fflush(stdout);
}

// ******************************** MODE ********************************

// @Comentários em "message.h"
void msg_invite_only_on(char* nickname_adm, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O administrador '%s' mudou o modo do canal '%s' para apenas convidados.\n", t_log, nickname_adm, channel_name);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_invite_only_off(char* nickname_adm, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [-] - [%s] O administrador '%s' mudou o modo do canal '%s' para qualquer usuario possa participar.\n", t_log, nickname_adm, channel_name);
    fflush(stdout);
}

// ******************************** INVITE ********************************

// @Comentários em "message.h"
void msg_invite(char* nickname_in, char* nickname_out, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O usuario '%s' convidou o outro usuario '%s' para o canal '%s'.\n", t_log, nickname_in, nickname_out, channel_name);
    fflush(stdout);
}

// ******************************** INVALID PROTOCOL ********************************

// @Comentários em "message.h"
void msg_invalid_protocol(int id){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [+] - [%s] O cliente %d enviou um protocolo com erro.\n", t_log, id);
    fflush(stdout);
}
