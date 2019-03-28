#include "client.h"

void check_args(int argc) {
	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}
}

void just_do_it(struct addrinfo *connected_addrinfo, int socket_file_descriptor);

int main(int argc, char *argv[]) {
	check_args(argc); // argv[1] is the server's hostname

    struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = STRUCT_IPV4;
	hints.ai_socktype = TCP;

	struct addrinfo *server_info;                          // linked list of addrinfo structures that match the
	get_server_info(argv[1], PORT, &hints, &server_info);  // server's hostname and PORT (subject to hints' restrictions)

    int socket_file_descriptor;
	struct addrinfo *connected_addrinfo;
	connect_to_first_match(server_info, &socket_file_descriptor, &connected_addrinfo);
	freeaddrinfo(server_info); // all done with this structure

	/* TODO */
	just_do_it(connected_addrinfo, socket_file_descriptor);
	/* end TODO */

	close(socket_file_descriptor);

	exit(0);
}

void just_do_it(struct addrinfo *connected_addrinfo, int socket_file_descriptor) {
	char network_addr_string[INET6_ADDRSTRLEN]; // network address of the connected server
												// (pointed by connected_addrinfo) as a string
	inet_ntop(connected_addrinfo->ai_family,
		      get_in_addr(connected_addrinfo->ai_addr),
			  network_addr_string, sizeof network_addr_string);
	printf("client: connecting to %s\n", network_addr_string);

	send(socket_file_descriptor, "Hello from the client!", 22, 0);

	receive_message(socket_file_descriptor);
}

void get_server_info(const char *hostname, const char *port, const struct addrinfo *hints, struct addrinfo **server_info) {
	int return_value;
	if ((return_value = getaddrinfo(hostname, port, hints, server_info)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
		exit(1);
	}
}

void connect_to_first_match(struct addrinfo *server_info, int *socket_file_descriptor, struct addrinfo **p) {
	// loop through all the results and connect to the first we can
	for (*p = server_info; *p != NULL; *p = (*p)->ai_next) {
		if ((*socket_file_descriptor = socket((*p)->ai_family, (*p)->ai_socktype, (*p)->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(*socket_file_descriptor, (*p)->ai_addr, (*p)->ai_addrlen) == -1) {
			perror("client: connect");
			close(*socket_file_descriptor);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(2);
	}
}

void *get_in_addr(struct sockaddr *sa) {
	// get sockaddr, IPv4 or IPv6:
	if (sa->sa_family == STRUCT_IPV6) {
        return &(((struct sockaddr_in6*)sa)->sin6_addr); // IPv6
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
