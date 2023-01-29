#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>


int main () {
	int network_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in remote_address;
	remote_address.sin_family=AF_INET;
	remote_address.sin_port=htons(9002);
	remote_address.sin_addr.s_addr=INADDR_ANY;
	if (
		connect(
			network_socket,
			(struct sockaddr*) &remote_address,
			sizeof(remote_address)
		) == -1
	) {
		printf("Error connecting to remove socket");
		return 1;	
	};
	char server_response[256];
	recv(network_socket, &server_response, sizeof(server_response), 0);
	
	printf("The remove socket sends data: %s\n", server_response);		
	close(network_socket);
	return 0;
};
