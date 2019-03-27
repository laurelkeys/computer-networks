/*
** client.c -- a stream socket client demo
*/

#include "libs.h"

#define MAXDATASIZE 100 // max number of bytes we can get at once

void *get_in_addr(struct sockaddr *sa);
void receive_message(int socket_file_descriptor);

int main(int argc, char *argv[]) {
	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

    struct addrinfo hints, *servinfo;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // change to AF_UNSPEC if you also want IPv6
	hints.ai_socktype = SOCK_STREAM;

    // argv[1] is the hostname
	int return_value;
	if ((return_value = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
		return 1;
	}

	// loop through all the results and connect to the first we can
    int socket_file_descriptor;
    struct addrinfo *p;
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_file_descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(socket_file_descriptor, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(socket_file_descriptor);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

    char network_addr_string[INET6_ADDRSTRLEN]; // network address of the connected server (pointed by p) as a string
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), network_addr_string, sizeof network_addr_string);
	printf("client: connecting to %s\n", network_addr_string);

	freeaddrinfo(servinfo); // all done with this structure

	send(socket_file_descriptor, "Hello from the client!", 22, 0);

	receive_message(socket_file_descriptor);

	close(socket_file_descriptor);

	return 0;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET6) {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);  // IPv6
	}

    return &(((struct sockaddr_in*)sa)->sin_addr); // IPv4
}

void receive_message(int socket_file_descriptor) {
	int numbytes; // length of the message written to the buffer
	char buffer[MAXDATASIZE];
	if ((numbytes = recv(socket_file_descriptor, buffer, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buffer[numbytes] = '\0';

	printf("client: received '%s'\n", buffer);
}
