#include "server.h"

sqlite3 *db;
sqlite3_stmt *res;

int main(void) {
    // initializes the db with data
    init_db();
    atexit(exit_cleanup);

    // linked list of addrinfo structures that match the server's hostname and PORT
    struct addrinfo *server_addrinfo;
    get_server_addrinfo(NULL, PORT, &server_addrinfo);

    // associates the socket referred to by socket_file_descriptor with the local address
    int socket_file_descriptor;
    struct addrinfo *connected_addrinfo;
    bind_to_first_match(server_addrinfo, &socket_file_descriptor, &connected_addrinfo);
    freeaddrinfo(server_addrinfo);

    // listens for connections on the socket referred to by socket_file_descriptor
    if (listen(socket_file_descriptor, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    // reaps all dead processes
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // main accept() loop
    printf("server: waiting for connections...\n");

    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    struct sockaddr_storage their_addr; // connector's address information
    int new_file_descriptor; // associated with new client connections
    
    while (1) {
        sin_size = sizeof(their_addr);
        new_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&their_addr, &sin_size);
        if (new_file_descriptor == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(socket_file_descriptor); // child doesn't need the listener
            receive_messages(new_file_descriptor); // loops untill the client decides to quit
            close(new_file_descriptor);
            exit(0);
        }

        close(new_file_descriptor); // parent doesn't need this
    }

    exit(0);
}

void receive_messages(int socket_file_descriptor) {
    char opt;
    char *buffer;
    while (1) {
        // Read client message
        recv_wrapper(socket_file_descriptor, &buffer);
        printf("server: received from client: '%s'\n", buffer);
        
        opt = buffer[0];
        free(buffer);
        if (opt == OPT_QUIT_STR[0]) break;

        switch (opt - '0') {
            case 1: opt_get_full_name_and_picture_from_profile(socket_file_descriptor);
            break;
            case 2: opt_get_profile(socket_file_descriptor);
            break;
            case 3: opt_get_profiles(socket_file_descriptor);
            break;
        }
    }
}

void send_file_to_client(int socket_file_descriptor, FILE *f) {
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size == 0) {
        send_wrapper(socket_file_descriptor, "Nenhum resultado encontrado no banco de dados");
    } else {
        char text[file_size + 1];
        text[0] = '\0';
        char buffer[file_size + 1];

        while (fgets(buffer,file_size,f)) strcat(text,buffer);
        text[file_size] = '\0';

        send_wrapper(socket_file_descriptor, text);
    }
}

void send_img_to_client(int socket_file_descriptor, FILE *f) {
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        long file_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        int i;
        char img_buffer[file_size + 1];
        for (i = 0; i < sizeof(img_buffer); i++) {
            fread(img_buffer + i, 1, 1, f);
        }

        send_img_wrapper(socket_file_descriptor, img_buffer, sizeof(img_buffer));
    } else {
        send_img_wrapper(socket_file_descriptor, NULL, 0);
    }
}

// OPTIONS //////////////////////////////

void opt_get_full_name_and_picture_from_profile(int socket_file_descriptor) {
    // (1) dado o email de um perfil, retornar nome, sobrenome e foto
    printf("server: client selected option 1:\n");

    char *email;
    recv_wrapper(socket_file_descriptor, &email);
    printf("server: email '%s'\n", email);

    log_timestamp("tcp:t2:1:ok", 1);
    get_full_name_from_profile_sql(email); // queries database
    log_timestamp("tcp:t3:1:ok", 1);

    // Send name and surname
    FILE *f = fopen(FILE_SERVER, "r");
    if (f) {
        send_file_to_client(socket_file_descriptor, f);
        fclose(f);
    }

    // Send picture
    char img_file_path[strlen(email) + 11]; // strlen("./imgs/.png") == 11
    sprintf(img_file_path, "./imgs/%s.png", email);
    FILE *img_file = fopen(img_file_path, "rb");
    if (img_file) {
        send_img_to_client(socket_file_descriptor, img_file);
        fclose(img_file);
    } else {
        printf("server: img_file not found (path: '%s')\n", img_file_path);
        send_img_to_client(socket_file_descriptor, NULL);
    }

    free(email);
}

void opt_get_profile(int socket_file_descriptor) {
    // (2) dado o email de um perfil, retornar suas informações
    printf("server: client selected option 2:\n");

    char *email;
    recv_wrapper(socket_file_descriptor, &email);
    printf("server: email '%s'\n", email);

    log_timestamp("tcp:t2:2:ok", 2);
    opt_get_profile_sql(email); // queries database
    log_timestamp("tcp:t3:2:ok", 2);

    // Send profile info
    FILE *f = fopen(FILE_SERVER, "r");
    if (f) {
        send_file_to_client(socket_file_descriptor, f);
        fclose(f);
    }

    // Send picture
    char img_file_path[strlen(email) + 11]; // strlen("./imgs/.png") == 11
    sprintf(img_file_path, "./imgs/%s.png", email);
    FILE *img_file = fopen(img_file_path, "rb");
    if (img_file) {
        send_img_to_client(socket_file_descriptor, img_file);
        fclose(img_file);
    } else {
        printf("server: img_file not found (path: '%s')\n", img_file_path);
        send_img_to_client(socket_file_descriptor, NULL);
    }

    free(email);
}

void opt_get_profiles(int socket_file_descriptor) {
    // (3) listar todas as informações de todos os perfis
    printf("server: client selected option 3:\n");

    log_timestamp("tcp:t2:3:ok", 3);
    opt_get_profiles_sql(); // queries database
    log_timestamp("tcp:t3:3:ok", 3);

    // Send profiles info
    FILE *f = fopen(FILE_SERVER, "r");
    if (f) {
        send_file_to_client(socket_file_descriptor, f);
        fclose(f);
    }

    // Get the list of emails and send their pictures
    char *email;
    char *emails;
    get_all_emails_sql(&emails);
    
    int cut;
    int end = 0;
    int begin = 0;
    while (end < strlen(emails) && *(emails+end) != '\0') {
        // Put "email = XXXXXXX@XXXX.com" in the buffer
        while (*(emails+end) != '\n') end++;
        email = malloc(end-begin+5);
        strncpy(email, emails+begin, end-begin);

        email[1] = '.';
        email[2] = '/';
        email[3] = 'i';
        email[4] = 'm';
        email[5] = 'g';
        email[6] = 's';
        email[7] = '/';
        email[end-begin] = '.';
        email[end-begin+1] = 'p';
        email[end-begin+2] = 'n';
        email[end-begin+3] = 'g';
        email[end-begin+4] = '\0';

        f = fopen((email+1),"rb");
        printf("server: uploading picture to client: %s\n", (email+1));
        email[end-begin] = '\0';
        send_wrapper(socket_file_descriptor, (email+8));
        if (f) {
            send_img_to_client(socket_file_descriptor, f);
            fclose(f);
            printf("server: picture sent\n");
        } else {
            printf("server: img_file not found (path: '%s')\n", (email+1));
            send_img_to_client(socket_file_descriptor, NULL);
        }
        free(email);
        end += 2;
        begin = end;
    }

    free(emails);
    send_wrapper(socket_file_descriptor, "THATS ALL;");
}

// CONNECTION ///////////////////////////

void sigchld_handler(int s) {
    (void) s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == STRUCT_IPV6) {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

    return &(((struct sockaddr_in*)sa)->sin_addr);
}

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
    fclose(fopen(FILE_SERVER, "w")); // clears FILE_SERVER to write the latest query's result
    if (sqlite3_exec(db, sql, send_info_callback, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }
}

int send_info_callback(void *_unused, int length, char **column_content, char **column_name) {
    FILE *f;
    f = fopen(FILE_SERVER, "a");
    char buffer[512];
    for (int i = 0; i < length; i++) {
        snprintf(buffer, sizeof(buffer), "%s = %s\n", 
                 column_name[i], column_content[i] 
                                 ? column_content[i] 
                                 : "NULL");
        printf("%s", buffer); // prints the result on the server's terminal
        fprintf(f, "%s", buffer); // writes the result to FILE_SERVER
    }
    fprintf(f, "\n");
    fclose(f);
    return 0;
}

void get_all_emails_sql(char **emails) {
    char *sql = "SELECT Profile.email FROM Profile;";
    execute_sql(sql);

    FILE *f = fopen(FILE_SERVER, "r");
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    *emails = malloc(file_size + 1);
    for (int i = 0; i < file_size; i++) {
        fread(*emails + i, 1, 1, f);
    }
    fclose(f);
    *((*emails) + file_size) = '\0';
}

void get_full_name_from_profile_sql(char *email) {
    // current_opt = 1;
    char *sql_part =
        "SELECT Profile.name, Profile.surname "
        "FROM Profile "
        "WHERE Profile.email = '%s';";

    char sql[strlen(sql_part) + strlen(email)];
    sprintf(sql, sql_part, email);
    execute_sql(sql);
}

void opt_get_profile_sql(char *email) {
    // current_opt = 2;
    char *sql_part =
        "SELECT Profile.name, Profile.surname, Profile.city, Profile.education, Skills.skills, Experiences.experiences "
        "FROM Profile "
        "INNER JOIN (SELECT Skill.email, GROUP_CONCAT(Skill.skill, ', ') AS skills FROM Skill GROUP BY Skill.email) AS Skills "
        "ON Profile.email = Skills.email "
        "INNER JOIN (SELECT Experience.email, GROUP_CONCAT(Experience.experience, ', ') AS experiences FROM Experience GROUP BY Experience.email) AS Experiences "
        "ON Profile.email = Experiences.email "
        "WHERE Profile.email = '%s';";

    char sql[strlen(sql_part) + strlen(email)];
    sprintf(sql, sql_part, email);
    execute_sql(sql);
}

void opt_get_profiles_sql() {
    // current_opt = 3;
    char *sql =
        "SELECT Profile.email, Profile.name, Profile.surname, Profile.city, Profile.education, Skills.skills, Experiences.experiences "
        "FROM Profile "
        "INNER JOIN (SELECT Skill.email, GROUP_CONCAT(Skill.skill, ', ') AS skills FROM Skill GROUP BY Skill.email) AS Skills "
        "ON Profile.email = Skills.email "
        "INNER JOIN (SELECT Experience.email, GROUP_CONCAT(Experience.experience, ', ') AS experiences FROM Experience GROUP BY Experience.email) AS Experiences "
        "ON Profile.email = Experiences.email "
        "GROUP BY Profile.email;";

    execute_sql(sql);
}

void exit_cleanup() {
    sqlite3_finalize(res);
    sqlite3_close(db);
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

        "CREATE TABLE Profile(email TEXT PRIMARY KEY, name TEXT, surname TEXT, city TEXT, education TEXT);"
        "CREATE TABLE Skill(email TEXT, skill TEXT, PRIMARY KEY (email, skill));"
        "CREATE TABLE Experience(email TEXT, experience TEXT, PRIMARY KEY (email, experience));"

        "INSERT INTO Profile VALUES('uno@mail.com','Uno','Dos','Campinas','Linguistics');"
        "INSERT INTO Profile VALUES('tres@mail.com','Tres','Cuatro','Campinas','CS');"
        "INSERT INTO Profile VALUES('cinco@mail.com','Cinco','Seis','Seattle','CS');"

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
