#include "client.h"

int v;

static void _check_args(int argc) {
    if (argc < 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
    v = (argc == 2) ? 0 : 1; // FIXME check for -v with getopt
}

int main(int argc, char *argv[]) {
    // argv[1] is the server's hostname
    _check_args(argc);

    // linked list of addrinfo structures that match the server's hostname and PORT
    struct addrinfo *server_addrinfo;
    get_server_addrinfo(argv[1], PORT, &server_addrinfo);

    // connects the socket referred to by socket_file_descriptor 
    // to the first matching specified address in server_addrinfo (i.e. connected_addrinfo)
    int socket_file_descriptor;
    struct addrinfo *connected_addrinfo;
    connect_to_first_match(server_addrinfo, &socket_file_descriptor, &connected_addrinfo);
    freeaddrinfo(server_addrinfo);

    // does the work :D
    just_do_it(connected_addrinfo, socket_file_descriptor);

    close(socket_file_descriptor);
    exit(0);
}

void just_do_it(struct addrinfo *connected_addrinfo, int socket_file_descriptor) {
    int option;
    bool quit = false;
    while (!quit) {
        print_options_list();
        read_option(&option);

        option = (option >= 1 && option <= 6) ? option : 7;
        printf("\n-- (%d) --\n", option);
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
                send_wrapper(socket_file_descriptor, OPT_QUIT_STR, v);
                quit = true;
                break;
        } // end of option, loops to allow a new one
    }

    printf("          / )\n");
    printf("        .' /\n");
    printf("   ----'  (____\n");
    printf("              _)\n");
    printf("              __)\n");
    printf("             __)   Até logo!\n");
    printf("  ---.._______)\n\n");
}

// OPTIONS //////////////////////////////

void read_option(int *option) {
    char *end;
    char option_buffer[3]; // ['option', '\n', '\0']
    do {
         if (fgets(option_buffer, sizeof option_buffer, stdin) == NULL) break;
         option_buffer[strlen(option_buffer) - 1] = '\0'; // remove newline
         *option = strtol(option_buffer, &end, 10);
    } while (end != option_buffer + strlen(option_buffer));
}

void save_result_to_file(char *result_header, char *result_buffer) {
    FILE *f;
    f = fopen(FILE_CLIENT, "a");
    fprintf(f, "%s", result_header);
    fprintf(f, "%s", result_buffer);
    fclose(f);
}

void save_img(char *file_name, char *buffer, int img_size) {
    FILE *f;
    strcat(file_name, ".png\0");
    f = fopen(file_name, "wb");
    for (int i = 0; i < img_size; i++) {
        putc(buffer[i], f);
    }
    fclose(f);
}

void opt_get_profiles_filtering_education(int socket_file_descriptor) {
    // (1) listar todas as pessoas formadas em um determinado curso
    send_wrapper(socket_file_descriptor, "1", v);

    // Read and send input
    int return_value;
    char input_buffer[MAX_INPUT_SIZE];
    return_value = get_input("Digite o curso> ", input_buffer, sizeof(input_buffer));
    if (validate_input(return_value, "\nCurso não digitado", "Nome muito longo ", input_buffer) != OK)
        return;
    send_wrapper(socket_file_descriptor, input_buffer, v);

    char *buffer;

    // Print result
    recv_wrapper(socket_file_descriptor, &buffer, v);
    printf("\n%s\n", buffer);

    // Save result
    char header[256];
    snprintf(header, sizeof(header), "--> (1) curso: %s\n", input_buffer);
    save_result_to_file(header, buffer);

    free(buffer);
}

void opt_get_skills_filtering_city(int socket_file_descriptor) {
    // (2) listar as habilidades dos perfis que moram em uma determinada cidade
    send_wrapper(socket_file_descriptor, "2", v);

    // Read and send input
    int return_value;
    char input_buffer[MAX_INPUT_SIZE];
    return_value = get_input("Digite a cidade> ", input_buffer, sizeof(input_buffer));    
    if (validate_input(return_value, "\nCidade não digitada", "Nome muito longo ", input_buffer) != OK)
        return;
    send_wrapper(socket_file_descriptor, input_buffer, v);

    char *buffer;

    // Print result
    recv_wrapper(socket_file_descriptor, &buffer, v);
    printf("\n%s\n", buffer);

    // Save result
    char header[256];
    snprintf(header, sizeof(header), "--> (2) cidade: %s\n", input_buffer);
    save_result_to_file(header, buffer);
    free(buffer);
}

void opt_add_skill_to_profile(int socket_file_descriptor) {
    // (3) acrescentar uma nova experiência em um perfil
    send_wrapper(socket_file_descriptor, "3", v);

    // Read and send input
    int return_value;
    char input_buffer_email[MAX_INPUT_SIZE];
    return_value = get_input("Digite o email do perfil> ", input_buffer_email, sizeof(input_buffer_email));    
    if (validate_input(return_value, "\nEmail não digitado", "Email muito longo ", input_buffer_email) != OK)
        return;
    send_wrapper(socket_file_descriptor, input_buffer_email, v);
    // TODO verify if the profile exists before asking for the skill
    char input_buffer_skill[MAX_INPUT_SIZE];
    return_value = get_input("Digite a habilidade> ", input_buffer_skill, sizeof(input_buffer_skill));    
    if (validate_input(return_value, "\nHabilidade não digitada", "Nome muito longo ", input_buffer_skill) != OK)
        return;
    send_wrapper(socket_file_descriptor, input_buffer_skill, v);

    char *buffer;

    // Print result
    recv_wrapper(socket_file_descriptor, &buffer, v);
    printf("\n%s\n", buffer);

    // Save result
    char header[256];
    snprintf(header, sizeof(header), "--> (3) email: %s, habilidade: %s\n", input_buffer_email, input_buffer_skill);
    save_result_to_file(header, buffer);
    free(buffer);
}

void opt_get_experience_from_profile(int socket_file_descriptor) {
    // (4) dado o email do perfil, retornar sua experiência
    send_wrapper(socket_file_descriptor, "4", v);

    // Read and send input
    int return_value;
    char input_buffer[MAX_INPUT_SIZE];
    return_value = get_input("Digite o email do perfil> ", input_buffer, sizeof(input_buffer));    
    if (validate_input(return_value, "\nEmail não digitado", "Email muito longo ", input_buffer) != OK)
        return;
    send_wrapper(socket_file_descriptor, input_buffer, v);

    char *buffer;

    // Print result
    recv_wrapper(socket_file_descriptor, &buffer, v);
    printf("\n%s\n", buffer);

    // Save result
    char header[256];
    snprintf(header, sizeof(header), "--> (4) email: %s\n", input_buffer);
    save_result_to_file(header, buffer);
    free(buffer);
}

void opt_get_profiles(int socket_file_descriptor) {
    // (5) listar todas as informações de todos os perfis
    send_wrapper(socket_file_descriptor, "5", v);

    char *buffer;

    // Print result
    recv_wrapper(socket_file_descriptor, &buffer, v);
    printf("\n%s\n", buffer); // prints the received result

    // Save result
    save_result_to_file("--> (5)\n", buffer);
    free(buffer);

    // TODO save pictures
}

void opt_get_profile(int socket_file_descriptor) {
    // (6) dado o email de um perfil, retornar suas informações
    send_wrapper(socket_file_descriptor, "6", v);

    // Read and send input
    int return_value;
    char input_buffer[MAX_INPUT_SIZE];
    return_value = get_input("Digite o email do perfil> ", input_buffer, sizeof(input_buffer));    
    if (validate_input(return_value, "\nEmail não digitado", "Email muito longo ", input_buffer) != OK)
        return;
    send_wrapper(socket_file_descriptor, input_buffer, v);

    char *buffer;

    // Print result
    recv_wrapper(socket_file_descriptor, &buffer, v);
    printf("\n%s\n", buffer);
    
    // Save result
    char header[256];
    snprintf(header, sizeof(header), "--> (6) email: %s\n", input_buffer);
    save_result_to_file(header, buffer);
    free(buffer);

    // Save picture
    int img_size;
    recv_img_wrapper(socket_file_descriptor, &buffer, &img_size, v);
    save_img(input_buffer, buffer, img_size);
    free(buffer);
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

    printf("(%s) sair.\n\n", OPT_QUIT_STR);
}

// CONNECTION ///////////////////////////

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
