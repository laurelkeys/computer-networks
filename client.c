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

    just_do_it(connected_addrinfo, socket_file_descriptor);

    close(socket_file_descriptor);

    exit(0);
}

void print_options_list() {
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

    printf("(7) sair.\n");
}

void just_do_it(struct addrinfo *connected_addrinfo, int socket_file_descriptor) {

    receive_message(socket_file_descriptor);
    send(socket_file_descriptor, "Hello from the client!", 22, 0);

    int option;

    int quit = 0;
    while (!quit) {
        print_options_list();

        scanf("%d", &option);
        printf("\nOpção %d: \n", option);

        switch (option) {
            case 1:
                send(socket_file_descriptor, "1", 1, 0);
                opt_get_profiles_filtering_education();
                break;
            case 2:
                send(socket_file_descriptor, "2", 1, 0);
                opt_get_skills_filtering_city();
                break;
            case 3:
                send(socket_file_descriptor, "3", 1, 0);
                opt_add_skill_to_profile();
                break;
            case 4:
                send(socket_file_descriptor, "4", 1, 0);
                opt_get_experience_from_profile();
                break;
            case 5:
                send(socket_file_descriptor, "5", 1, 0);
                opt_get_profiles();
                break;
            case 6:
                send(socket_file_descriptor, "6", 1, 0);
                opt_get_profile();
                break;
            default:
                send(socket_file_descriptor, "Goodbye from the client!", 24, 0);
                send(socket_file_descriptor, QUIT_CODE_STR, 1, 0);
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

// OPTIONS

void opt_get_profiles_filtering_education() {
    printf("-- um --\n");
}

void opt_get_skills_filtering_city() {
    printf("-- dois --\n");
}

void opt_add_skill_to_profile() {
    printf("-- três --\n");
}

void opt_get_experience_from_profile() {
    printf("-- quatro --\n");
}

void opt_get_profiles() {
    printf("-- cinco --\n");
}

void opt_get_profile() {
    printf("-- seis --\n");
}
