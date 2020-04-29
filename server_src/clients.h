#ifndef CLIENTS_H
    #define CLIENTS_H 4040
	#define MAX_CLIENTS 30	
	#define BUFFER_SIZE 4096

	#include <iostream>
	#include <netinet/in.h> /* struct sockaddr_in */
	#include <pthread.h> /* pthread_cond_t */
	#include <vector>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h> /* strlen */
	#include <unistd.h> /* close */

	using namespace std;

	typedef struct _client{
		struct sockaddr_in cl_address;
		int cl_socket;
		int cl_id;
		int sv_socket;
		char name[9];
	}client;

	/* Funcao que cria clientes (cria threads e soh utiliza elas quando necessario) */
	client* clt_criar(struct sockaddr_in address, int socket, int id, int sv_socket);

	/* Destroi o cliente */
	void clt_destruir_cliente(client* cl);

	/* Libera todos clients e limpa e memória */ 
	void clt_destruir_clientes();

	/* Adiociona o cliente na "queue" */
	int clt_add_queue(client* cl, int max_cl, pthread_mutex_t* mutex);

	/* Remove um cliente da queue de clientes*/
	client* clt_remove_queue(int id, int max_cl, pthread_mutex_t* mutex);

	/* Envia a mensagem para todos os clientes */
	void clt_send_message_all(int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer);

	/* Recuperar o cliente pelo id */
	client* clt_get_by_id(int id, int max_clients);

	/* Funcao que gerencia o cliente */
	void clt_run(int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex);

#endif