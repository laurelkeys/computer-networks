#include "client.h"

void check_args(int argc) {
    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    // argv[1] is the server's hostname
    check_args(argc);

    // linked list of addrinfo structures that match the server's hostname and PORT
    struct addrinfo *server_addrinfo;
    get_server_addrinfo(argv[1], PORT, &server_addrinfo);

    // connects the socket referred to by socket_file_descriptor 
    // to the first matching specified address in server_addrinfo (i.e. connected_addrinfo)
    int socket_file_descriptor;
    struct addrinfo *connected_addrinfo;
    connect_to_first_match(server_addrinfo, &socket_file_descriptor, &connected_addrinfo);
    freeaddrinfo(server_addrinfo);

    // does the work ;)
    just_do_it(connected_addrinfo, socket_file_descriptor);

    close(socket_file_descriptor);
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

void just_do_it(struct addrinfo *connected_addrinfo, int socket_file_descriptor) {
    // receive_message(socket_file_descriptor);
    send(socket_file_descriptor, "Hello from client!", 22, SEND_NO_FLAGS);

    int option;
    int quit = 0;
    while (!quit) {
        print_options_list();

        char *end;
        char option_buffer[3]; // ['option', '\n', '\0']
        do {
             if (!fgets(option_buffer, sizeof option_buffer, stdin)) break;             
             option_buffer[strlen(option_buffer) - 1] = 0; // remove newline
             option = strtol(option_buffer, &end, 10);
        } while (end != option_buffer + strlen(option_buffer));

        printf("\n-- (%d) --\n", option);
        option = (option >= 1 && option <= 6) ? option : 7;
        switch (option) {
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
            default:
                // send(socket_file_descriptor, "Goodbye from client!", 24, SEND_NO_FLAGS);
                send(socket_file_descriptor, OPT_QUIT_STR, 1, SEND_NO_FLAGS);
                quit = 1;
                break;
        }
    }

    printf("          / )\n");
    printf("        .' /\n");
    printf("   ----'  (____\n");
    printf("              _)\n");
    printf("              __)\n");
    printf("             __)   Até logo!\n");
    printf("  ---.._______)\n\n");
}

// OPTIONS

// (1) listar todas as pessoas formadas em um determinado curso
void opt_get_profiles_filtering_education(int socket_file_descriptor) {
    send(socket_file_descriptor, "1", 1, SEND_NO_FLAGS);
}

// (2) listar as habilidades dos perfis que moram em uma determinada cidade
void opt_get_skills_filtering_city(int socket_file_descriptor) {
    send(socket_file_descriptor, "2", 1, SEND_NO_FLAGS);
}

// (3) acrescentar uma nova experiência em um perfil
void opt_add_skill_to_profile(int socket_file_descriptor) {
    send(socket_file_descriptor, "3", 1, SEND_NO_FLAGS);
}

// (4) dado o email do perfil, retornar sua experiência
void opt_get_experience_from_profile(int socket_file_descriptor) {
    send(socket_file_descriptor, "4", 1, SEND_NO_FLAGS);
}

// (5) listar todas as informações de todos os perfis
void opt_get_profiles(int socket_file_descriptor) {
    send(socket_file_descriptor, "5", 1, SEND_NO_FLAGS);
}

// (6) dado o email de um perfil, retornar suas informações
void opt_get_profile(int socket_file_descriptor) {
    send(socket_file_descriptor, "6", 1, SEND_NO_FLAGS);
}

void print_options_list(int socket_file_descriptor) {
    printf("Escolha uma das seguintes opções:\n");

    printf("(1) listar todas as pessoas formadas em um determinado curso;\n");
    // opt_get_profiles_filtering_education()
    printf("(2) listar as habilidades dos perfis que moram em uma determinada cidade;\n");
    // opt_get_skills_filtering_city()
    printf("(3) acrescentar uma nova experiência em um perfil;\n");
    // opt_add_skill_to_profile()
    printf("(4) dado o email do perfil, retornar sua experiência;\n");
    // opt_get_experience_from_profile()
    printf("(5) listar todas as informações de todos os perfis;\n");
    // opt_get_profiles()
    printf("(6) dado o email de um perfil, retornar suas informações;\n");
    // opt_get_profile()

    printf("(%s) sair.\n\n", OPT_QUIT_STR);
}

// CONNECTION 

void get_server_addrinfo(const char *hostname, const char *port, struct addrinfo **server_addrinfo) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = STRUCT_IPV4;
    hints.ai_socktype = TCP;

    int return_value;
    if ((return_value = getaddrinfo(hostname, port, &hints, server_addrinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
        exit(1);
    }
}

void connect_to_first_match(struct addrinfo *server_addrinfo, int *socket_file_descriptor, struct addrinfo **p) {
    // loop through all the results and connect to the first we can
    int connection_refused = 1;
    for (*p = server_addrinfo; *p != NULL; *p = (*p)->ai_next) {
        if ((*socket_file_descriptor = socket((*p)->ai_family, (*p)->ai_socktype, (*p)->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(*socket_file_descriptor, (*p)->ai_addr, (*p)->ai_addrlen) == -1) {
            perror("client: connect");
            close(*socket_file_descriptor);
            continue;
        } else connection_refused = 0;

        break;
    }

    if (p == NULL || connection_refused) {
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
