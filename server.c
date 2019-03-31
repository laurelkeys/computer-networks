#include "server.h"

sqlite3 *db;
sqlite3_stmt *res;

int current_opt = 0;

int main(void) {
    init_db();

    // opt_get_profiles();
    // exit(0);

    struct addrinfo *server_addrinfo;
    get_server_addrinfo(NULL, PORT, &server_addrinfo);

    int socket_file_descriptor;  // listen on socket_file_descriptor
    struct addrinfo *connected_addrinfo;
    bind_to_first_match(server_addrinfo, &socket_file_descriptor, &connected_addrinfo);
    freeaddrinfo(server_addrinfo); // all done with this structure

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
    int new_file_descriptor; // new connection on new_file_descriptor
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

            int bytes_left = send_wrapper(new_file_descriptor, "Server Hello");
            printf("Hello bytes_left: %d\n", bytes_left);
            // if (send(new_file_descriptor, "Hello, world!", 13, 0) == -1)
            //     perror("send");

            receive_messages(new_file_descriptor);

            close(new_file_descriptor);
            exit(0);
        }

        close(new_file_descriptor);  // parent doesn't need this
    }

    exit(0);
}

void receive_messages(int socket_file_descriptor) {
    int numbytes; // length of the message written to the buffer
    char *buffer;

    while (1) {
        // if ((numbytes = recv(socket_file_descriptor, buffer, MAXDATASIZE-1, 0)) == -1) {
        //     perror("recv");
        //     exit(1);
        // }

        // buffer[numbytes] = '\0';
        recv_wrapper(socket_file_descriptor, &buffer);
        printf("recv_wrapper '%s'\n", buffer);
        send_wrapper(socket_file_descriptor, "testeste");
        free(buffer);
        continue;
        // printf("client: received '%s'\n", buffer);

        if (buffer[0] == OPT_QUIT_STR[0]) break; // FIXME

        switch (buffer[0] - '0') {
            case 1: _opt_get_profiles_filtering_education(socket_file_descriptor);
            break;
            case 2: _opt_get_skills_filtering_city(socket_file_descriptor);
            break;
            case 3: _opt_add_skill_to_profile(socket_file_descriptor);
            break;
            case 4: _opt_get_experience_from_profile(socket_file_descriptor);
            break;
            case 5: _opt_get_profiles(socket_file_descriptor);
            break;
            case 6: _opt_get_profile(socket_file_descriptor);
            break;
        }
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
    if (sa->sa_family == STRUCT_IPV6) {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

    return &(((struct sockaddr_in*)sa)->sin_addr);
}

// OPTIONS //////////////////////////////

void _opt_get_profiles_filtering_education(int socket_file_descriptor) {
    printf("option selected: 1\n");
    // if (send(socket_file_descriptor, "opt selected: 1", 15, 0) == -1) perror("send");
}

void _opt_get_skills_filtering_city(int socket_file_descriptor) {
    printf("option selected: 2\n");
    // if (send(socket_file_descriptor, "opt selected: 2", 15, 0) == -1) perror("send");
}

void _opt_add_skill_to_profile(int socket_file_descriptor) {
    printf("option selected: 3\n");
    // if (send(socket_file_descriptor, "opt selected: 3", 15, 0) == -1) perror("send");
}

void _opt_get_experience_from_profile(int socket_file_descriptor) {
    printf("option selected: 4\n");
    // if (send(socket_file_descriptor, "opt selected: 4", 15, 0) == -1) perror("send");
}

void send_file_to_client(FILE *f) {  
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("file size: %ld\n", file_size);


}

// FIXME
void _opt_get_profiles(int socket_file_descriptor) {
    printf("option selected: 5\n");
    int bytes_left = send_wrapper(socket_file_descriptor, "opt selected: 5");
    printf("bytes_left: %d\n", bytes_left);

    // if (send(socket_file_descriptor, "opt selected: 5", 15, 0) == -1) perror("send");
    opt_get_profiles(); // queries database
    send_file_to_client(fopen("result.txt", "r"));
}

void _opt_get_profile(int socket_file_descriptor) {
    printf("option selected: 6\n");
    // if (send(socket_file_descriptor, "opt selected: 6", 15, 0) == -1) perror("send");
}

void opt_get_profiles_filtering_education(char *education) {
    current_opt = 1;
    char sql[46+strlen(education)];
    char *sql_part = "SELECT name FROM Profile WHERE education = '%s'";
    sprintf(sql, sql_part, education);
    execute_sql(sql);    
}

void opt_get_skills_filtering_city(char *city) {
    current_opt = 2;
    char sql[119+strlen(city)];
    char *sql_part = "SELECT Profile.name, Skill.skill FROM Profile INNER JOIN Skill ON Skill.email = Profile.email WHERE Profile.city = '%s'";
    sprintf(sql, sql_part, city);
    execute_sql(sql);
}

void opt_add_skill_to_profile(char *email, char *skill) {
    current_opt = 3;
    char sql[35+strlen(email)+strlen(skill)];
    char *sql_part = "INSERT INTO Skill VALUES('%s','%s')";
    sprintf(sql, sql_part, email, skill);
    execute_sql(sql);
}

void opt_get_experience_from_profile(char *email) {
    current_opt = 4;
    char sql[119+strlen(email)];
    char *sql_part = "SELECT experience FROM Experience WHERE email = '%s'";
    sprintf(sql, sql_part, email);
    execute_sql(sql);
}

void opt_get_profiles() {
    current_opt = 5;
    char *sql = 
        "SELECT Profile.name, Profile.surname, Profile.city, Profile.education, Skills.skills, Experiences.experiences "
        "FROM Profile "
        "INNER JOIN (SELECT Skill.email, GROUP_CONCAT(Skill.skill, ', ') AS skills FROM Skill GROUP BY Skill.email) AS Skills "
        "ON Profile.email = Skills.email "
        "INNER JOIN (SELECT Experience.email, GROUP_CONCAT(Experience.experience, ', ') AS experiences FROM Experience GROUP BY Experience.email) AS Experiences "
        "ON Profile.email = Experiences.email "
        "GROUP BY Profile.email;";
    execute_sql(sql);
}

void opt_get_profile(char *email) {
    current_opt = 6;
    char sql[500+strlen(email)];
    char *sql_part = 
        "SELECT Profile.name, Profile.surname, Profile.city, Profile.education, Skills.skills, Experiences.experiences "
        "FROM Profile "
        "INNER JOIN (SELECT Skill.email, GROUP_CONCAT(Skill.skill, ', ') AS skills FROM Skill GROUP BY Skill.email) AS Skills "
        "ON Profile.email = Skills.email "
        "INNER JOIN (SELECT Experience.email, GROUP_CONCAT(Experience.experience, ', ') AS experiences FROM Experience GROUP BY Experience.email) AS Experiences "
        "ON Profile.email = Experiences.email "
        "WHERE Profile.email = '%s';";
    sprintf(sql, sql_part, email);
    execute_sql(sql);
}

// CONNECTION ///////////////////////////

void get_server_addrinfo(const char *hostname, const char *port, struct addrinfo **server_addrinfo) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = STRUCT_IPVX;
    hints.ai_socktype = TCP;
    hints.ai_flags = AI_PASSIVE; // use my IP

    int return_value;
    if ((return_value = getaddrinfo(hostname, port, &hints, server_addrinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
        exit(1);
    }
}

void bind_to_first_match(struct addrinfo *server_addrinfo, int *socket_file_descriptor, struct addrinfo **p) {
    // loop through all the results and bind to the first we can
    int yes = 1;
    for (*p = server_addrinfo; *p != NULL; *p = (*p)->ai_next) {
        if ((*socket_file_descriptor = socket((*p)->ai_family, (*p)->ai_socktype, (*p)->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(*socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(*socket_file_descriptor, (*p)->ai_addr, (*p)->ai_addrlen) == -1) {
            close(*socket_file_descriptor);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
}

// DATABASE /////////////////////////////

void execute_sql(char *sql) {
    char *err_msg = 0;
    fclose(fopen("result.txt", "w"));
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
    printf("\nCallback from opt %d\n", current_opt);

    FILE *f;
    f = fopen("result.txt", "a");
    char buffer[512];
    for (int i = 0; i < length; i++) {
        snprintf(buffer, sizeof(buffer), "%s = %s\n", column_name[i], column_content[i] ? column_content[i] : "NULL");

        int line_size = 0;
        char ch = ' ';
        while (ch != '\n' && line_size < 512) {
            ch = buffer[line_size++];
        }
        printf("line size: %d\n", line_size);


        printf("%s", buffer);
        fprintf(f, "%s", buffer);
    }

    fprintf(f, "\n");  
    fclose(f);
    return 0;
}

void init_db() {
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    char *sql = 
        "DROP TABLE IF EXISTS Profile;" 
        "DROP TABLE IF EXISTS Skill;" 
        "DROP TABLE IF EXISTS Experience;" 
        
        "CREATE TABLE Profile(email TEXT PRIMARY KEY, name TEXT, surname TEXT, city TEXT, education TEXT);" //FIXME add picture
        "CREATE TABLE Skill(email TEXT, skill TEXT, PRIMARY KEY (email, skill));"
        "CREATE TABLE Experience(email TEXT, experience TEXT, PRIMARY KEY (email, experience));"
        
        "INSERT INTO Profile VALUES('uno@mail.com','Uno','Dos','Campinas','Linguistics');" 
        "INSERT INTO Profile VALUES('tres@mail.com','Tres','Cuatro','Campinas','Computer Science');" 
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
