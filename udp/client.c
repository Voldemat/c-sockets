#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 9000
#define SERVER_HOST INADDR_ANY
#define SENDER_BUFFER 1024
#define READ_TIMEOUT_S 3


int main () {
	char message[SENDER_BUFFER] = "Hello server";
	char message_in[SENDER_BUFFER];
	// Define server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port=htons(SERVER_PORT);
	server_address.sin_addr.s_addr=SERVER_HOST;
 
 	// Create socket
	int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_socket == -1) {
		printf("Error creating socket: %d\n", errno);
		return 1;
	};

	// Set recvfrom timeout
	struct timeval read_timeout;
	read_timeout.tv_sec = READ_TIMEOUT_S;
	read_timeout.tv_usec = 0;
	if (
		setsockopt(
			client_socket, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout
		) == -1
	) {
		printf("Error setsocketopt: %d\n", errno);
		close(client_socket);
		return 1;
	};
	// Send message to server
	int sendto_status = sendto(
		client_socket,
		message, sizeof(message),
		0,
		(struct sockaddr*) &server_address, sizeof(server_address)
	);
	if (sendto_status == -1) {
		printf("sendto error: %d\n", errno);
		close(client_socket);
		return 1;
	};
	// Receive response from server
	int recv_status = recvfrom(client_socket, message_in, sizeof(message_in), 0, NULL, NULL);
	if (recv_status == -1) {
		printf("recvfrom error %d\n", errno);
		close(client_socket);
		return 1;
	};
	printf("Receives data from server: %s\n", message_in);
	close(client_socket);
	return 0;
}