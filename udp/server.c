#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT 9000
#define HOST INADDR_ANY
#define RECEIVE_BUFFER_SIZE 1024

static volatile int keepRunning = 1;

void sigintHandler() {
	keepRunning = 0;
};

int main () {
	char receive_buffer[RECEIVE_BUFFER_SIZE];
	// Define SIGINT handler for gracefull shutdown
	struct sigaction act;
	act.sa_handler = sigintHandler;
	sigaction(SIGINT, &act, NULL);
	// Define server address
	struct sockaddr_in server_address;
	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(PORT);
	server_address.sin_addr.s_addr = HOST;

	// Creating server socket
	int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_socket == -1) {
		printf("Error creating socket: %d\n", errno);
		return 1;
	};
	// Binding socket
	int bind_status = bind(
		server_socket, (struct sockaddr*)&server_address, sizeof(server_address)
	);
	if (bind_status == -1) {
		printf("Error binding socket: %d\n", errno);
		close(server_socket);
		return 1;
	};
	// Setup variables used for client sockets
	struct sockaddr_in client_address;
	char client_host[INET6_ADDRSTRLEN];
	int client_port;
	socklen_t client_address_size = sizeof(client_address);
	
	// Main waiting loop
	while (1) {
		int recv_status = recvfrom(
			server_socket,
			receive_buffer,
			sizeof(receive_buffer),
			0,
			(struct sockaddr*) &client_address,
			&client_address_size
		);
		// Gracefull shutdown
		if (!keepRunning) {break;};
		
		if (recv_status == -1) {
			printf("Recvfrom error: %d\n", errno);
			continue;
		};
		// Get human readable client address
		client_port = ntohs(client_address.sin_port);
		inet_ntop(AF_INET, &client_address.sin_addr, client_host, sizeof(client_host));
		printf("Receive data: %s, from %s:%d\n", receive_buffer, client_host, client_port);
		
		int send_to_status = sendto(
			server_socket,
			receive_buffer,
			sizeof(receive_buffer),
			0,
			(struct sockaddr*) &client_address,
			client_address_size
		);
		if (send_to_status == -1) {
			printf("sendto error: %d\n", errno);
			continue;
		};
	}
	close(server_socket);
	return 0;
}