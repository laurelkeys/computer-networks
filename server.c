#include "server.h"

sqlite3 *db;
sqlite3_stmt *res;

int main(void) {
	if (1){
		init_db();
		opt_get_profiles_filtering_education("Computer Science");
	} else {

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

	atexit(exit_cleanup);

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

			receive_messages(new_file_descriptor);

			close(new_file_descriptor);
			exit(0);
		}

		close(new_file_descriptor);  // parent doesn't need this
	}

	exit(0);
	}
}

void receive_messages(int socket_file_descriptor) {
	int numbytes; // length of the message written to the buffer
	char buffer[MAXDATASIZE];

	while (1) {
		if ((numbytes = recv(socket_file_descriptor, buffer, MAXDATASIZE-1, 0)) == -1) {
		    perror("recv");
		    exit(1);
		}

		buffer[numbytes] = '\0';

		printf("client: received '%s'\n", buffer);

		if (buffer[0] == QUIT_CODE_CHR) break; // FIXME
	}
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

void init_db() {
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
	char *sql = "DROP TABLE IF EXISTS Profile;" 
                "CREATE TABLE Profile(email TEXT PRIMARY KEY, name TEXT, surname TEXT, city TEXT, education TEXT);" //FIXME add picture
				"CREATE TABLE Skill(email TEXT, skill TEXT, PRIMARY KEY (email, skill));"
				"CREATE TABLE Experience(email TEXT, experience TEXT, PRIMARY KEY (email, experience));"
                "INSERT INTO Profile VALUES('uno@mail.com','Uno','Dos','Campinas','Linguistics');" 
                "INSERT INTO Profile VALUES('tres@mail.com','Tres','Cuatro','London','Computer Science');" 
                "INSERT INTO Profile VALUES('cinco@mail.com','Cinco','Seis','Seattle','Computer Engineering');" 
				"INSERT INTO Skill VALUES('uno@mail.com','Acoustic Engineering');" 
				"INSERT INTO Skill VALUES('uno@mail.com','English');" 
				"INSERT INTO Skill VALUES('tres@mail.com','Read');" 
				"INSERT INTO Skill VALUES('tres@mail.com','Write');" 
				"INSERT INTO Skill VALUES('tres@mail.com','Code');" 
				"INSERT INTO Skill VALUES('cinco@mail.com','Spanish');" 
				"INSERT INTO Skill VALUES('cinco@mail.com','English');" 
				"INSERT INTO Skill VALUES('cinco@mail.com','Reap');" 
				"INSERT INTO Skill VALUES('cinco@mail.com','Sow');" 
				"INSERT INTO Experience VALUES('uno@mail.com','Work');" 
				"INSERT INTO Experience VALUES('uno@mail.com','Research');" 
				"INSERT INTO Experience VALUES('tres@mail.com','Work');" 
				"INSERT INTO Experience VALUES('tres@mail.com','Study');" 
				"INSERT INTO Experience VALUES('cinco@mail.com','Study');" 
				"INSERT INTO Experience VALUES('cinco@mail.com','Work');" 
				"INSERT INTO Experience VALUES('cinco@mail.com','Study more');";

	execute_sql(sql);
}

void execute_sql(char * sql) {
	char *err_msg = 0;
    if (sqlite3_exec(db, sql, send_info_callback, 0, &err_msg) != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        exit(1);
    } 	
}

void exit_cleanup() {
	sqlite3_finalize(res);
	sqlite3_close(db);
}

int send_info_callback(void *not_used, int length, char **column_content, char **column_name) {
	for (int i = 0; i < length; i++) {
        printf("%s = %s\n", column_name[i], column_content[i] ? column_content[i] : "NULL");
    }
	return 0;
}

void opt_get_profiles_filtering_education(char * education) {
	char sql[46+strlen(education)];
	char * sql_part = "SELECT name FROM Profile WHERE education = '%s'";
	sprintf(sql, sql_part, education);
	execute_sql(sql);	
}

void opt_add_skill_to_profile(char * email, char * skill) {
	char sql[35+strlen(email)+strlen(skill)];
	char * sql_part = "INSERT INTO Skill VALUES('%s','%s')";
	sprintf(sql, email, skill);
	execute_sql(sql);
}