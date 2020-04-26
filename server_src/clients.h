
typedef struct _client{
    struct sockaddr_in cl_address;
	int cl_socket;
	int cl_id;
	char name[9];
}client