#include "server.h"

int main(void) {
	struct addrinfo hints, *servinfo, *p;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = STRUCT_IPVX;
	hints.ai_socktype = TCP;
	hints.ai_flags = AI_PASSIVE; // use my IP
	int return_value;

	if ((return_value = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
		exit(1);
	}

	int socket_file_descriptor, new_file_descriptor;  // listen on sock_fd, new connection on new_file_descriptor
	int yes = 1;

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_file_descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(socket_file_descriptor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_file_descriptor);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(socket_file_descriptor, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	struct sigaction sa;
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];

	while (1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&their_addr, &sin_size);
		if (new_file_descriptor == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(socket_file_descriptor); // child doesn't need the listener
			if (send(new_file_descriptor, "Hello, world!", 13, 0) == -1)
				perror("send");
			close(new_file_descriptor);
			exit(0);
		}

		receive_message(new_file_descriptor);

		close(new_file_descriptor);  // parent doesn't need this
	}

	exit(0);
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

void sigchld_handler(int s) {
	(void) s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
