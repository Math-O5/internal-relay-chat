#ifndef CLIENTS_H
    #define CLIENTS_H 4040

	#define BUFFER_SIZE 4096

	#include <iostream>
	#include <netinet/in.h> /* struct sockaddr_in */
	#include <pthread.h> /* pthread_cond_t */
	#include <vector>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h> /* strlen */

	using namespace std;

	typedef struct _client{
		struct sockaddr_in cl_address;
		int cl_socket;
		int cl_id;
		int sv_socket;
		char name[9];
	}client;

	/* Funcao que cria clientes (cria threads e soh utiliza elas quando necessario) */
	client* criar_cliente(struct sockaddr_in address, int socket, int id, int sv_socket);

	/* Destroi o cliente */
	void destruir_cliente(client* cl);

	/* Adiociona o cliente na "queue" */
	int add_queue_client(client** cl_arr, client* cl, int max_cl, pthread_mutex_t* mutex);

	/* Remove um cliente da queue de clientes*/
	client* remove_queue_client(client** cl_arr, int id, int max_cl, pthread_mutex_t* mutex);

	/* Envia a mensagem para todos os clientes */
	void send_message(client** cl_arr, int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer);

	/* Recuperar o cliente pelo id */
	client* get_client_by_id(client** cl_arr, int id, int max_clients);

	/* Funcao que gerencia o cliente */
	void run_client(client** cl_arr, int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex);

#endif