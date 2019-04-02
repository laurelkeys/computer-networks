#include "server.h"

sqlite3 *db;
sqlite3_stmt *res;

int current_opt = 0;
int v = false; // TODO change for less info output

int main(void) {
    init_db();

    struct addrinfo *server_addrinfo;
    get_server_addrinfo(NULL, PORT, &server_addrinfo);

    int socket_file_descriptor; // listen on socket_file_descriptor
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
    while (true) { // main accept() loop
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

            receive_messages(new_file_descriptor);

            close(new_file_descriptor);
            exit(0);
        }

        close(new_file_descriptor); // parent doesn't need this
    }

    exit(0);
}

void receive_messages(int socket_file_descriptor) {
    int numbytes; // length of the message written to the buffer
    char *buffer;
    char opt;
    struct timeval tv3, tv4;

    while (true) {
        recv_wrapper(socket_file_descriptor, &buffer, v);
        printf("server: received from client: '%s'\n", buffer);
        opt = buffer[0];
        free(buffer);

        if (opt == OPT_QUIT_STR[0]) break; // FIXME

        gettimeofday(&tv3, NULL);
        switch (opt - '0') {
            case 1: opt_get_profiles_filtering_education(socket_file_descriptor);
            break;
            case 2: opt_get_skills_filtering_city(socket_file_descriptor);
            break;
            case 3: opt_add_skill_to_profile(socket_file_descriptor);
            break;
            case 4: opt_get_experience_from_profile(socket_file_descriptor);
            break;
            case 5: opt_get_profiles(socket_file_descriptor);
            break;
            case 6: opt_get_profile(socket_file_descriptor);
            break;
        }
        gettimeofday(&tv4, NULL);
        printf("TEMPO DE PROCESSAMENTO: %ldus\n", tv4.tv_usec-tv3.tv_usec);
    }
}

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

// OPTIONS //////////////////////////////

void send_file_to_client(int socket_file_descriptor, FILE *f) {  
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (v) printf("send_file_to_client: file size: %ld\n", file_size);

    if (file_size == 0) {
        send_wrapper(socket_file_descriptor, "No results", v);
    } else {
        char text[file_size + 1];
        text[0] = '\0';
        char buffer[file_size + 1];
        
        while (fgets(buffer,file_size,f)) strcat(text,buffer);
        text[file_size] = '\0';

        send_wrapper(socket_file_descriptor, text, v);
    }
}

void send_picture_to_client(int socket_file_descriptor, FILE *f) {  
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (v) printf("send_picture_to_client: file size: %ld\n", file_size);

    int i;
    char img_buffer[file_size + 1];
    for (i = 0; i < sizeof(img_buffer); i++) {
        fread(img_buffer + i, 1, 1, f);
    }
    
    send_img_wrapper(socket_file_descriptor, img_buffer, sizeof(img_buffer), v);
}

void opt_get_profiles_filtering_education(int socket_file_descriptor) {
    // (1) listar todas as pessoas formadas em um determinado curso
    printf("server: client selected option 1:\n");

    char *buffer;
    recv_wrapper(socket_file_descriptor, &buffer, v);
    printf("server: education '%s'\n", buffer);

    opt_get_profiles_filtering_education_sql(buffer); // queries database
    free(buffer);

    FILE *f = fopen(FILE_SERVER, "r");
    if (f) {
        send_file_to_client(socket_file_descriptor, f);
        fclose(f);
    }
}

void opt_get_skills_filtering_city(int socket_file_descriptor) {
    // (2) listar as habilidades dos perfis que moram em uma determinada cidade
    printf("server: client selected option 2:\n");

    char *buffer;
    recv_wrapper(socket_file_descriptor, &buffer, v);
    printf("server: city '%s'\n", buffer);

    opt_get_skills_filtering_city_sql(buffer); // queries database
    free(buffer);

    FILE *f = fopen(FILE_SERVER, "r");
    if (f) {
        send_file_to_client(socket_file_descriptor, f);
        fclose(f);
    }
}

void opt_add_skill_to_profile(int socket_file_descriptor) {
    // (3) acrescentar uma nova experiência em um perfil
    printf("server: client selected option 3:\n");

    char *email;
    recv_wrapper(socket_file_descriptor, &email, v);
    printf("server: email '%s'\n", email);

    char *skill;
    recv_wrapper(socket_file_descriptor, &skill, v);
    printf("server: skill '%s'\n", skill);

    opt_add_skill_to_profile_sql(email, skill); // queries database
    free(email);
    free(skill);

    // TODO send a 'success' message
    // send_file_to_client(socket_file_descriptor, fopen(FILE_SERVER, "r"));
}

void opt_get_experience_from_profile(int socket_file_descriptor) {
    // (4) dado o email do perfil, retornar sua experiência
    printf("server: client selected option 4:\n");

    char *email;
    recv_wrapper(socket_file_descriptor, &email, v);
    printf("server: email '%s'\n", email);

    opt_get_experience_from_profile_sql(email); // queries database
    free(email);

    FILE *f = fopen(FILE_SERVER, "r");
    if (f) {
        send_file_to_client(socket_file_descriptor, f);
        fclose(f);
    }
}

void opt_get_profiles(int socket_file_descriptor) {
    // (5) listar todas as informações de todos os perfis
    printf("server: client selected option 5:\n");

    opt_get_profiles_sql(); // queries database

    FILE *f = fopen(FILE_SERVER, "r");
    if (f) {
        send_file_to_client(socket_file_descriptor, f);
        fclose(f);
    }

    // get list of emails and send their pictures
    char *emails;
    char *email;
    get_all_emails_sql(&emails);
    int end = 0;
    int begin = 0;
    int cut;
    // printf("Get profiles sending pics - all emails:''%s''\n", emails);
    // printf("Get profiles sending pics - size(all emails):%ld\n", strlen(emails));
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
        if (f) {
            send_wrapper(socket_file_descriptor, (email+8), v);
            send_picture_to_client(socket_file_descriptor, f);
            fclose(f);
            // printf("Sent img\n");
        } else {
            printf("server: img_file not found (path: '%s')\n", f);
        }
        free(email);
        end += 2;
        begin = end;
    }
    free(emails);
    send_wrapper(socket_file_descriptor, "THATS ALL;", v);
}

// FIXME
void opt_get_profile(int socket_file_descriptor) {
    // (6) dado o email de um perfil, retornar suas informações
    printf("server: client selected option 6:\n");

    char *email;    
    recv_wrapper(socket_file_descriptor, &email, v);
    printf("server: email '%s'\n", email);

    opt_get_profile_sql(email); // queries database

    FILE *f = fopen(FILE_SERVER, "r");
    if (f) {
        send_file_to_client(socket_file_descriptor, f);
        fclose(f);
    }

    // Send img
    char *img_file_name = picture_name_from_email(email);
    free(email);
    
    char img_file_path[strlen(img_file_name) + 11];
    sprintf(img_file_path, "./imgs/%s.png", img_file_name);
    free(img_file_name);

    FILE *img_file = fopen(img_file_path, "rb");
    if (img_file) {
        send_picture_to_client(socket_file_descriptor, img_file);
        fclose(img_file);
    } else {
        printf("server: img_file not found (path: '%s')\n", img_file_path);
    }
    
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
    fclose(fopen(FILE_SERVER, "w")); // rewrites FILE_SERVER with the latest query result
    if (sqlite3_exec(db, sql, send_info_callback, 0, &err_msg) != SQLITE_OK) {
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
    if (v) printf("callback start (opt %d):\n", current_opt);

    FILE *f;
    f = fopen(FILE_SERVER, "a");
    char buffer[512];
    for (int i = 0; i < length; i++) {
        snprintf(buffer, sizeof(buffer), "%s = %s\n", column_name[i], column_content[i] ? column_content[i] : "NULL");
        printf("%s", buffer);
        fprintf(f, "%s", buffer);
    }
    
    if (v) printf("callback end (opt %d).\n", current_opt);
    fprintf(f, "\n");
    fclose(f);
    return 0;
}

void get_all_emails_sql(char **emails) {
    current_opt = 1;
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
    *((*emails)+file_size) = '\0';
}

void opt_get_profiles_filtering_education_sql(char *education) {
    current_opt = 1;
    char *sql_part = "SELECT name FROM Profile WHERE education = '%s'";
    char sql[strlen(sql_part) + strlen(education)];
    
    sprintf(sql, sql_part, education);
    
    execute_sql(sql);    
}

void opt_get_skills_filtering_city_sql(char *city) {
    current_opt = 2;
    char *sql_part = "SELECT Skill.skill, Profile.name FROM Profile INNER JOIN Skill ON Skill.email = Profile.email WHERE Profile.city = '%s'";
    char sql[strlen(sql_part) + strlen(city)];
    
    sprintf(sql, sql_part, city);
    
    execute_sql(sql);
}

void opt_add_skill_to_profile_sql(char *email, char *skill) {
    current_opt = 3;
    char *sql_part = "INSERT INTO Skill VALUES('%s','%s')";
    char sql[strlen(sql_part) + strlen(email) + strlen(skill)];
    
    sprintf(sql, sql_part, email, skill);
    
    execute_sql(sql);
}

void opt_get_experience_from_profile_sql(char *email) {
    current_opt = 4;
    char *sql_part = "SELECT experience FROM Experience WHERE email = '%s'";
    char sql[strlen(sql_part) + strlen(email)];
    
    sprintf(sql, sql_part, email);
    
    execute_sql(sql);
}

void opt_get_profiles_sql() {
    current_opt = 5;
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

void opt_get_profile_sql(char *email) {
    current_opt = 6;
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
