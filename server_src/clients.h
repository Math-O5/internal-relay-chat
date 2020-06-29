#ifndef CLIENTS_H
    #define CLIENTS_H 4040

	/**
	 * Constantes e Macros
	 * -------------------
	 * 
	 * BUFFER_SIZE
	 * Tamanho que os buffers de receive/send podem ocupar
	 * 
	 * SUCESS e FAIL
	 * Valores genéricos de sucesso e erro
	 * 
	 * CONTINUE, QUIT e PING
	 * Valores que identificam a operação a ser feita pela informação
	 * recebida pelo cliente. CONTINUE para enviar a mensagem para outros clientes,
	 * QUIT para desconectar o cliente e PING para retornar "pong"
	 * 
	 * MAX_CLIENTS
	 * Valor com a quantidade máxima de clientes permitidos no servidor
	 */
		#define BUFFER_SIZE 4096

		#define SUCCESS 0
		#define FAIL 1
        #define ERROR_CREATE_CHANNEL 8

		#define CONTINUE 0
		#define QUIT 1
		#define PING 2

		#define MAX_CLIENTS 20
		#define MAX_SIZE_NAME 30
	
	/**
     * Bibliotecas importadas
     * ----------------------
     * 
     * Devido ao uso da biblioteca "pthread.h", é necessário que o código
     * seja compilado com o parâmetro -pthread junto ao g++
    */
		#include <iostream>
		#include <netinet/in.h> /* struct sockaddr_in */
		#include <pthread.h> /* pthread_cond_t */
		#include <vector>
		#include <stdio.h>
		#include <stdlib.h>
		#include <string.h> /* strlen */
		#include <unistd.h> /* close */

	/**
     * Biblioteca do própio projeto
     * -----------------------------
    */
		#include "message.h"

	using namespace std;

	/**
	 * @struct
	 * typedef struct _client client
	 * -----------------------------
	 * 
	 * Struct com todas as informações do cliente e com o socket do servidor
	 * 
	 * atributos de conexão
	 * --------------------
	 * - cl_address: endereço do cliente
	 * - cl_socket: socket do cliente que foi conetado ao servidor
	 * - sv_socket: socket do servidor que o cliente está conectado
	 * 
	 * atributos do cliente (chat)
	 * ---------------------------
	 * - cl_id: id do cliente. Obs.: O id dos clientes sempre começa pelo 10
	 * - name[9]: nickname(apelido) do cliente
	*/
	// TODO: clear unsed information
	typedef struct _client{
		// atributos de conexão
		struct sockaddr_in cl_address;
		int cl_socket;
		int sv_socket;

		// atributos do cliente (chat)
		int cl_id;
		char nickname[MAX_SIZE_NAME];
		char channel[MAX_SIZE_NAME];
	}client;

	/**
	 * @function
	 * 
	 * client* clt_criar(struct sockaddr_in address, int socket, int id, int sv_socket)
	 * --------------------------------------------------------------------------------
	 * 
	 * Cria um novo cliente de acordo com informações passadas no parâmetro: o endereço do cliente,
	 * socket em que o cliente se conectou, id do cliente e socket em que o servidor se conectou respectivamente.
	 * Obs.: O cliente será alocado na memória.
	 * 
	 * return: cliente* retorna-se o endereço do struct cliente criado
	*/
	client* clt_criar(struct sockaddr_in address, int socket, int id, int sv_socket);

	/**
	 * @function
	 * 
	 * void clt_destruir(int id)
	 * -------------------------
	 * 
	 * Responsável por tirar o cliente do servidor. Primeiro o cliente é identificado pelo seu id, depois 
	 * é fechada a sua conexão com o servidor. Após isso a lista de clientes é renovada (coloca-se NULL na 
	 * posição do array) e o cliente é desalocado da memória.
	*/
	void clt_destruir(int id);

	/**
	 * @function
	 * 
	 * void clt_destruir_clientes()
	 * ----------------------------
	 * 
	 * Fecha a conexão de todos os clientes conectados no servidor e limpa a memória
	*/
	void clt_destruir_clientes();

	/**
	 * @function
	 * 
	 * void clt_add_queue(client* cl, int max_cl, pthread_mutex_t* mutex)
	 * ------------------------------------------------------------------
	 * 
	 * Adiciona um novo cliente na lista de clientes. Será utilizada a primeira posição disponível na lista.
	 * 
	 * return: SUCCESS o cliente foi adicionado na lista com sucesso
	 * 		   FAIL	   ocorreu um erro ao adicionar o cliente na lista (sem posição vaga na lista)
	*/
	int clt_add_queue(client* cl, pthread_mutex_t* mutex);

	/**
	 * @function
	 * 
	 * client* clt_remove_queue(int id, int max_cl, pthread_mutex_t* mutex)
	 * --------------------------------------------------------------------
	 * 
	 * Remove um cliente da lista e atualiza o valor (na lista) para NULL. É retornado o endereço do 
	 * cliente removido
	 * 
	 * return: client* endereço do cliente removido
	 * 		   NULL    cliente não encontrado
	*/
	client* clt_remove_queue(int id, pthread_mutex_t* mutex);

	/**
	 * @function
	 * 
	 * int clt_read_buffer(client* cl, char* buffer)
	 * ---------------------------------------------
	 * 
	 * Verifica se a informação passada pelo cliente é um comando ou se é uma mensagem.
	 * 
	 * Comandos: - /quit : o cliente se desconectou
	 * 			 - /ping : retorna "pong" para o cliente
	 * 
	 * Se nenhum dos comandos for passado, o cliente mandou uma mensagem para ser transmitida para os outros clientes do servidor
	 * 
	 * return: CONTINUE o cliente mandou uma mensagem para os outros clientes
	 * 		   QUIT     o cliente enviou o comando /quit
	 * 		   PING     o cliente enviou o comando /ping			
	*/
	int clt_read_buffer(client* cl, char* buffer);

	/**
	 * @function
	 * 
	 * bool clt_send_message(int cl_socket, char* buffer)
	 * --------------------------------------------------
	 * 
	 * Envia uma mensagem para cl_socket, o qual cl_socket é um id de um socket conectado (são feitas 5 tentativas se não receber 
	 * a confirmação). Se a mensagem não for recebida retorna FAIL, ou caso contrário, SUCCESS.
	 * 
	 * return: SUCESS foi enviada a mensagem para com o cliente 
	 * 		   FAIL não foi possível enviar a mensagem para o cliente
	*/
	bool clt_send_message(int cl_socket, char* buffer);

	/**
	 * @function
	 * 
	 * void clt_send_message_all(int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer)
	 * -----------------------------------------------------------------------------------------
	 * 
	 * Envia uma mensagem para todos os clientes. O conteúdo da mensagem terá, além da informação, o id do cliiente
	 * que enviou a mensagem.
	*/
	void clt_send_message_all(int id_cur, int max_conn, pthread_mutex_t* mutex, char* buffer);

	/**
	 * @function
	 * 
	 * client* clt_get_by_id(int id, int max_clients)
	 * ----------------------------------------------
	 * 
	 * Recupera o struct cliente a partir de seu id
	 * 
	 * return: cliente* struct do cliente com o id passado no parâmetro da função
	 * 		   NULL     o cliente não foi encontrado
	*/
	client* clt_get_by_id(int id, int max_clients);

	/**
	 * @function
	 * 
	 * void clt_run(int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex)
	 * -----------------------------------------------------------------------------
	 * 
	 * Responsável for receber as respostas do cliente. Caso o cliente se desconecte (sem dar o comando /quit)
	 * a função será terminada.
	*/
	void clt_run(int sv_socket, int id_cur, int max_conn, pthread_mutex_t* mutex);

	// Lê até o '\n' o '\0' e retorna a posição que parou
	int decode_message(char* buffer, char* pack, int index);

#endif