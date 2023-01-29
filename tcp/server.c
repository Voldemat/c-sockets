#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

static volatile int keepRunning = 1; 

void sigint_handler() {
	keepRunning=0;
};
struct client_address {
	char host[INET6_ADDRSTRLEN];
	int port;
};
struct client_address getClientAddress(struct sockaddr_in* addr) {
	struct client_address cl_address;
	cl_address.port=ntohs(addr->sin_port);
    inet_ntop(AF_INET, &addr->sin_addr, cl_address.host, sizeof cl_address.host);
    return cl_address;
}

int main () {
	struct sigaction act;
	act.sa_handler = sigint_handler;
	sigaction(SIGINT, &act, NULL);
	char server_message[256] = "Hello world";
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;
	int x2 = 1;
	if (
		setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &x2, sizeof(server_socket)) == -1
	) {
			printf("Error setting socket SO_REUSEADDR option, errno=%d\n", errno);
	};
	if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))==-1) {
		printf("Error binding to socket: %d\n", errno);
		return 1;
	};
	listen(server_socket, 5);

	int client_socket;
	struct sockaddr_storage client_address;
	socklen_t size = sizeof(client_address);
	while (1) {
		client_socket = accept(server_socket, (struct sockaddr*)&client_address, &size);
		if (!keepRunning) {break;};
		if (client_socket == -1) {
			printf("Error accepting new sockets, errno = %d\n", errno);
			close(server_socket);
			return 1;
		}
	    struct client_address cl_address = getClientAddress((struct sockaddr_in *)&client_address);
	    printf("Connected socket: %s:%i\n", cl_address.host, cl_address.port);
		send(client_socket, server_message, sizeof(server_message), 0);
		close(client_socket);
	};
	close(server_socket);
	return 0;
};
