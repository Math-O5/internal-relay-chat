#include "message.h"

// @Comentários em "message.h"
void time_log(char* t_log, int t_log_size){
    struct tm *sTime;
    time_t cur_time = time(0);
    sTime = localtime(&cur_time);
    strftime(t_log, t_log_size, "%d-%m-%Y %H:%M:%S", sTime);
}

// @Comentários em "message.h"
void msg_inicio(int status){
    if(status == CONNECT) 
    {
        system("clear");
        printf("%s\n", COLORB_YELLOW);
        printf("   ██████╗ ███████╗██╗      █████╗ ██╗   ██╗    ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗ \n");
        printf("   ██╔══██╗██╔════╝██║     ██╔══██╗╚██╗ ██╔╝    ██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗\n");
        printf("   ██████╔╝█████╗  ██║     ███████║ ╚████╔╝     ███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝\n");
        printf("   ██╔══██╗██╔══╝  ██║     ██╔══██║  ╚██╔╝      ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗\n");
        printf("   ██║  ██║███████╗███████╗██║  ██║   ██║       ███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║\n");
        printf("   ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝   ╚═╝       ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\n");
        printf("                                                           powered by VanLoon, Math05, JNagasava.\n");
        printf("%s\n", COLOR_RESET);
        fflush(stdout);
    } else {
        system("clear");
        printf("%s", PREFIX_EMPTY_LINE);
        printf("%s The Relay Chat Server IS NOT RUNNING ...\n", PREFIX_ERROR);
        printf("%s", PREFIX_EMPTY_LINE);
        fflush(stdout);
    }
}

// @Comentários em "message.h"
void msg_info_server(int port, int sv_socket, int max_conn, int cl_count)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Informacoes do Servidor:\n", PREFIX_NORMAL, t_log);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Port %d\n", PREFIX_NORMAL, t_log, port);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Socket Server: %d\n", PREFIX_NORMAL, t_log, sv_socket);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Numero maximo de conexoes permitidas: %d\n", PREFIX_NORMAL, t_log, max_conn); 
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Clientes conectados: %d\n", PREFIX_NORMAL, t_log, cl_count); 
    printf("%s", PREFIX_EMPTY_LINE); 
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_info_client(int id, int socket, struct sockaddr_in cl_addr)
{
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Informacoes do cliente(%d)\n", PREFIX_NORMAL, t_log, id);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Id: %d\n", PREFIX_NORMAL, t_log, id);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Socket: %d\n", PREFIX_NORMAL, t_log, socket);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Host: %d.%d.%d.%d\n",
                                    PREFIX_NORMAL,
                                    t_log,
                                    (cl_addr.sin_addr.s_addr & 0xff),
                                    (cl_addr.sin_addr.s_addr & 0xff00) >> 8,
                                    (cl_addr.sin_addr.s_addr & 0xff0000) >> 16,
                                    (cl_addr.sin_addr.s_addr & 0xff000000) >> 24);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Port: %d\n", PREFIX_NORMAL, t_log, cl_addr.sin_port);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_recv_cliente(int id, char* buffer){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d enviou uma mensagem/comando...\n", PREFIX_SUCCESS, t_log, id);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] <CLIENTE %d>: %s\n", PREFIX_SUCCESS, t_log, id, buffer);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_send_cliente(int id_emissor, int id_receptor){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] A mensagem do cliente %d foi entregue ao cliente %d...\n", PREFIX_SUCCESS, t_log, id_emissor, id_receptor);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_cliente_conexao(){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Novo cliente no servidor...\n", PREFIX_SUCCESS, t_log); 
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_client_no_response(int id){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d não responde...\n", PREFIX_ERROR, t_log, id);
    fflush(stderr);
}

// @Comentários em "message.h"
void msg_client_no_response_channel(int id, char* nickname, char* channel){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d (%s) não responde e foi removido do canal %s...\n", PREFIX_ERROR, t_log, id, nickname, channel);
    fflush(stderr);
}

// @Comentários em "message.h"
void msg_cliente_desconexao(int id){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d se desconectou...\n", PREFIX_ERROR, t_log, id);
    fflush(stderr);
}

// @Comentários em "message.h"
void msg_max_client(struct sockaddr_in cl_addr){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O numero maximo de clientes foi excedido\n", PREFIX_ERROR, t_log);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Informacoes do cliente que tentou conectar com o servidor:\n", PREFIX_ERROR, t_log);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Host: %d.%d.%d.%d\n",
                                    PREFIX_ERROR,
                                    t_log,
                                    (cl_addr.sin_addr.s_addr & 0xff),
                                    (cl_addr.sin_addr.s_addr & 0xff00) >> 8,
                                    (cl_addr.sin_addr.s_addr & 0xff0000) >> 16,
                                    (cl_addr.sin_addr.s_addr & 0xff000000) >> 24);
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Port: %d\n", PREFIX_ERROR, t_log, cl_addr.sin_port);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_info_ping(int id) {
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Recebido o ping do cliente %d.\n", PREFIX_SUCCESS, t_log, id);
    fflush(stdout);
} 

// @Comentários em "message.h"
void msg_info_pong(int id) {
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Foi enviado pong para o cliente %d.\n", PREFIX_SUCCESS, t_log, id);
    fflush(stdout);
} 

// @Comentários em "message.h"
void msg_nickname_cliente(int id, const char* newNickname, char* oldNickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d mudou o nickname de %s para %s...\n", PREFIX_SUCCESS, t_log, id, oldNickname, newNickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_join_channel(struct _client* clt){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O %s %d entrou no canal %s.\n", PREFIX_SUCCESS, t_log, clt->nickname, clt->cl_id, clt->channel->nickname_channel);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_encerrar_server(){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Servidor finalizado com sucesso.\n", PREFIX_SUCCESS, t_log);
    fflush(stdout);
}

// ******************************* MENSAGENS GERAIS *******************************

// @Comentários em "message.h"
void msg(char type, char* msg){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("  [%c] - [%s] %s\n", type, t_log, msg);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_request_client_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') deseja mandar uma mensagem\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_client_channel(const char* nickname, const char* channel){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente ('%s') enviou uma mensagem para o canal '%s'\n", PREFIX_NORMAL, t_log, nickname, channel);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_channel_client(const char* nickname, const char* channel){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O canal '%s' enviou uma mensagem para o usuario '%s'\n", PREFIX_NORMAL, t_log, channel, nickname);
    fflush(stdout);
}
// ******************************* LIST *******************************

// @Comentários em "message.h"
void msg_request_list(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou a listagem dos canais do servidor\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_list(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Foi listado para o cliente %d ('%s') os canais do servidor\n", PREFIX_SUCCESS, t_log, id, nickname);
    fflush(stdout);
}

// ******************************* JOIN *******************************

// @Comentários em "message.h"
void msg_request_create_channel(int id, char* nickname, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou entrar no canal '%s'.\n", PREFIX_NORMAL, t_log, id, nickname, channel_name);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_create_channel(int id, char* nickname, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O canal '%s' foi criado pelo cliente %d ('%s')\n", PREFIX_SUCCESS, t_log, channel_name, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_create_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O nome do canal que o cliente %d ('%s') solicitou criar/entrar nao eh valido\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout); 
}

// @Comentários em "message.h"
void msg_join_channel(int id, char* nickname, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') deu join no canal '%s'.\n", PREFIX_SUCCESS, t_log, id, nickname, channel_name);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_exit_channel(char* nickname, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O usuario '%s' saiu do canal '%s'.\n", PREFIX_ERROR, t_log, nickname, channel_name);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_destroy_channel(char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O canal '%s' foi terminado pois nao haviam mais participantes.\n", PREFIX_ERROR, t_log, channel_name);
    fflush(stdout);
}

// ******************************* NICKNAME *******************************

// @Comentários em "message.h"
void msg_create_nickname(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d criou seu nickname: %s.\n", PREFIX_SUCCESS, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_request_nickname(int id, char* new_nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d solicitou alterar o seu nickname para %s\n", PREFIX_NORMAL, t_log, id, new_nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_change_nickname(int id, char* old_nickname, char* new_nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d alterou seu nickname de %s para %s.\n", PREFIX_SUCCESS, t_log, id, old_nickname, new_nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_nicknameinuse(int id){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O nickname que o cliente %d solicitou ja esta em uso.\n", PREFIX_ERROR, t_log, id);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_nickname(int id){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] Ocorreu um erro ao alterar o nickname do cliente %d\n", PREFIX_ERROR, t_log, id);
    fflush(stdout);
}

// ******************************* KICK *******************************

// @Comentários em "message.h"
void msg_request_kick(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou a expulsao de um cliente\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_kick(int id, char* nickname_adm, char* nickname_user){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') expulsou o cliente ('%s')\n", PREFIX_SUCCESS, t_log, id, nickname_adm, nickname_user);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_kick_no_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') esta em nenhum canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_kick_denied(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao tem permissao para expulsar um cliente\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_kick_no_nick(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou expulsar um nick que nao existe no canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// ******************************* UNKICK *******************************

// @Comentários em "message.h"
void msg_request_unkick(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou que a expulsao de um cliente seja removida\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_unkick(int id, char* nickname_adm, char* nickname_user){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') removeu a expulsao do cliente ('%s')\n", PREFIX_SUCCESS, t_log, id, nickname_adm, nickname_user);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_unkick_no_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') esta em nenhum canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_unkick_denied(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao tem permissao para remover a expulsao de um cliente\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_unkick_no_nick(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou remover a expulsao de um nick que nao existe no canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// ******************************* MUTE *******************************

// @Comentários em "message.h"
void msg_request_mute(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou o silenciamento de um cliente\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_mute(int id, char* nickname_adm, char* nickname_user){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') silenciou o cliente ('%s')\n", PREFIX_SUCCESS, t_log, id, nickname_adm, nickname_user);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_mute_no_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') esta em nenhum canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_mute_denied(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao tem permissao para silenciar um cliente\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_mute_no_nick(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou silenciar um nick que nao existe no canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// ******************************* UNMUTE *******************************

// @Comentários em "message.h"
void msg_request_unmute(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou o remover o silenciamento de um cliente\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_unmute(int id, char* nickname_adm, char* nickname_user){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') removeu o silenciamento do cliente ('%s')\n", PREFIX_SUCCESS, t_log, id, nickname_adm, nickname_user);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_unmute_no_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') esta em nenhum canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_unmute_denied(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao tem permissao para remover o silenciamento de um cliente\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_unmute_no_nick(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou remover o silenciamento de um nick que nao existe no canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// ******************************** WHOIS *******************************

// @Comentários em "message.h"
void msg_request_whois(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou o IP de outro cliente\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_whois(int id, char* nickname_adm, char* nickname_user_ip){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') recebeu o IP do cliente solicitado ('%s')\n", PREFIX_SUCCESS, t_log, id, nickname_adm, nickname_user_ip);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_whois_no_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') esta em nenhum canal para solicitar o IP de alguem\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_whois_not_adm(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao tem permissao para solicitar o IP\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_whois_no_nick(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao recebeu o IP solicitado pois o nick nao existe\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// ******************************** MODE ********************************

// @Comentários em "message.h"
void msg_request_mode(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') solicitou mudar o modo do canal\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_invite_only_on(int id, char* nickname_adm, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') mudou o modo do canal '%s' para apenas convidados.\n", PREFIX_SUCCESS, t_log, id, nickname_adm, channel_name);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_invite_only_off(int id, char* nickname_adm, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') mudou o modo do canal '%s' para qualquer usuario possa participar.\n", PREFIX_SUCCESS, t_log, id, nickname_adm, channel_name);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_mode_no_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao pode alterar o modo do canal pois ele esta em nenhum canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_mode_not_adm(int id, char* nickname, char* channel_name){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao pode alterar o modo do canal ('%s') pois ele nao eh o administrador\n", PREFIX_ERROR, t_log, id, nickname, channel_name);
    fflush(stdout);
}

// ******************************** INVITE ********************************

// @Comentários em "message.h"
void msg_request_invite(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') deseja convidar um cliente\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_invite(int id, char* nickname_adm, char* nickname_user){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') convidou o cliente ('%s')\n", PREFIX_SUCCESS, t_log, id, nickname_adm, nickname_user);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_invite_no_channel(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') esta em nenhum canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_invite_denied(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') nao tem permissao para convidar um cliente\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_error_invite_no_nick(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') deseja convidar um nick que nao existe no canal\n", PREFIX_ERROR, t_log, id, nickname);
    fflush(stdout);
}

// ******************************** PING ********************************

// @Comentários em "message.h"
void msg_request_ping(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') enviou um 'ping' para o servidor\n", PREFIX_NORMAL, t_log, id, nickname);
    fflush(stdout);
}

// @Comentários em "message.h"
void msg_pong(int id, char* nickname){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d ('%s') recebeu um 'pong' do servidor\n", PREFIX_SUCCESS, t_log, id, nickname);
    fflush(stdout);
}

// ******************************** INVALID PROTOCOL ********************************

// @Comentários em "message.h"
void msg_invalid_protocol(int id){
    char t_log[20];
    time_log(t_log, sizeof(t_log));
    printf("%s - [%s] O cliente %d enviou um protocolo com erro.\n", PREFIX_ERROR, t_log, id);
    fflush(stdout);
}