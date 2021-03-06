#include "client.h"

struct addrinfo *connected_addrinfo;
int timeout_in_s = 1;

static void _check_args(int argc) {
    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
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
    create_socket(server_addrinfo, &socket_file_descriptor, &connected_addrinfo);
    freeaddrinfo(server_addrinfo);

    // does the work :D
    just_do_it(connected_addrinfo, socket_file_descriptor);

    close(socket_file_descriptor);
    exit(0);
}

void just_do_it(struct addrinfo *connected_addrinfo, int socket_file_descriptor) {
    int option;
    int quit = 0;
    while (!quit) {
        print_options_list();
        read_option(&option);

        option = (option >= 1 && option <= 3) ? option : 4;
        printf("\n-- (%d) --\n", option);
        switch (option) {
            case 1: opt_get_full_name_and_picture_from_profile(socket_file_descriptor);
            break;
            case 2: opt_get_profile(socket_file_descriptor);
            break;
            case 3: opt_get_profiles(socket_file_descriptor);
            break;
            default:
                // no message needed since there's no connection with the server
                quit = 1;
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
         if (fgets(option_buffer, sizeof(option_buffer), stdin) == NULL) break;
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
    for (int i = 0; i < img_size; ++i) {
        putc(buffer[i], f);
    }
    fclose(f);
}

void opt_get_full_name_and_picture_from_profile(int socket_file_descriptor) {
    // (1) dado o email de um perfil, retornar nome, sobrenome e foto

    // Read and send input
    int return_value;
    char input_buffer[MAX_INPUT_SIZE+1];
    input_buffer[0] = '1';
    return_value = get_input("Digite o email do perfil> ", input_buffer+1, sizeof(input_buffer));
    if (validate_input(return_value, "\nEmail não digitado", "Email muito longo ", input_buffer) != OK)
        return;
    
    log_timestamp("udp:t1:1:ok", 1);
    sendto_wrapper(socket_file_descriptor, input_buffer, connected_addrinfo->ai_addr, connected_addrinfo->ai_addrlen);

    char *buffer;

    // Print result
    return_value = recvfrom_wrapper_timeout(socket_file_descriptor, &buffer, connected_addrinfo->ai_addr, &(connected_addrinfo->ai_addrlen), timeout_in_s);
    if (return_value == TIMEOUT) {
        log_timestamp("udp:t4:1:timeout", 1);
        printf("\nO servidor não respondeu a tempo (timeout de %ds atingido)\n\n", timeout_in_s);
        return;
    }
    printf("\n%s\n", buffer);

    // Save result
    char header[256];
    snprintf(header, sizeof(header), "--> (1) email: %s\n", input_buffer+1);
    save_result_to_file(header, buffer);
    free(buffer);

    // Save picture
    int img_size;
    return_value = recvfrom_img_wrapper_timeout(socket_file_descriptor, &buffer, &img_size, connected_addrinfo->ai_addr, &(connected_addrinfo->ai_addrlen), timeout_in_s);
    if (return_value == TIMEOUT) {
        log_timestamp("udp:t4:1:timeout", 1);
        printf("\nO servidor não respondeu a tempo (timeout de %ds atingido)\n\n", timeout_in_s);
        return;
    }
    log_timestamp("udp:t4:1:ok", 1);

    if (img_size > 0) {
        save_img(input_buffer+1, buffer, img_size);
        printf("Foto de perfil salva: %s\n\n", input_buffer+1);
    } else {
        printf("Nenhuma foto foi encontrada (%s)\n\n", input_buffer+1);
    }
    free(buffer);
}

void opt_get_profile(int socket_file_descriptor) {
    // (2) dado o email de um perfil, retornar suas informações

    // Read and send input
    int return_value;
    char input_buffer[MAX_INPUT_SIZE+1];
    input_buffer[0] = '2';
    return_value = get_input("Digite o email do perfil> ", input_buffer+1, sizeof(input_buffer));
    if (validate_input(return_value, "\nEmail não digitado", "Email muito longo ", input_buffer) != OK)
        return;
    log_timestamp("udp:t1:2:ok", 2);
    sendto_wrapper(socket_file_descriptor, input_buffer, connected_addrinfo->ai_addr, connected_addrinfo->ai_addrlen);

    char *buffer;

    // Print result
    return_value = recvfrom_wrapper_timeout(socket_file_descriptor, &buffer, connected_addrinfo->ai_addr, &(connected_addrinfo->ai_addrlen), timeout_in_s);
    if (return_value == TIMEOUT) {
        log_timestamp("udp:t4:2:timeout", 2);
        printf("\nO servidor não respondeu a tempo (timeout de %ds atingido)\n\n", timeout_in_s);
        return;
    }
    printf("\n%s\n", buffer);

    // Save result
    char header[256];
    snprintf(header, sizeof(header), "--> (2) email: %s\n", input_buffer+1);
    save_result_to_file(header, buffer);
    free(buffer);

    // Save picture
    int img_size;
    return_value = recvfrom_img_wrapper_timeout(socket_file_descriptor, &buffer, &img_size, connected_addrinfo->ai_addr, &(connected_addrinfo->ai_addrlen), timeout_in_s);
    if (return_value == TIMEOUT) {
        log_timestamp("udp:t4:2:timeout", 2);
        printf("\nO servidor não respondeu a tempo (timeout de %ds atingido)\n\n", timeout_in_s);
        return;
    }
    log_timestamp("udp:t4:2:ok", 2);

    if (img_size > 0) {
        save_img(input_buffer+1, buffer, img_size);
        printf("Foto de perfil salva: %s\n\n", input_buffer+1);
    } else {
        printf("Nenhuma foto foi encontrada (%s)\n\n", input_buffer+1);
    }
    free(buffer);
}

void opt_get_profiles(int socket_file_descriptor) {
    // (3) listar todas as informações de todos os perfis
    log_timestamp("udp:t1:3:ok", 3);
    sendto_wrapper(socket_file_descriptor, "3", connected_addrinfo->ai_addr, connected_addrinfo->ai_addrlen);

    char *buffer;

    // Print result
    int return_value;
    return_value = recvfrom_wrapper_timeout(socket_file_descriptor, &buffer, connected_addrinfo->ai_addr, &(connected_addrinfo->ai_addrlen), timeout_in_s);
    if (return_value == TIMEOUT) {
        log_timestamp("udp:t4:3:timeout", 3);
        printf("\nO servidor não respondeu a tempo (timeout de %ds atingido)\n\n", timeout_in_s);
        return;
    }
    
    printf("\n%s\n", buffer);

    // Save result
    save_result_to_file("--> (3)\n", buffer);
    free(buffer);

    // Save pictures
    char *name;
    int img_size;
    return_value = recvfrom_wrapper_timeout(socket_file_descriptor, &name, connected_addrinfo->ai_addr, &(connected_addrinfo->ai_addrlen), timeout_in_s);
    if (return_value == TIMEOUT) {
        log_timestamp("udp:t4:3:timeout", 3);
        printf("\nO servidor não respondeu a tempo (timeout de %ds atingido)\n\n", timeout_in_s);
        return;
    }
    log_timestamp("udp:t4:3:ok", 3);

    while (strcmp(name, "THATS ALL;\0")) {
        // Save picture
        return_value = recvfrom_img_wrapper_timeout(socket_file_descriptor, &buffer, &img_size, connected_addrinfo->ai_addr, &(connected_addrinfo->ai_addrlen), timeout_in_s);
        if (return_value == TIMEOUT) {
            printf("\nO servidor não respondeu a tempo (timeout de %ds atingido)\n\n", timeout_in_s);
            return;
        }
          
        if (img_size > 0) {
            save_img(name, buffer, img_size);
            printf("Foto de perfil salva: %s\n\n", name);
        } else {
            printf("Nenhuma foto foi encontrada (%s)\n\n", name);
        }
        free(buffer);
        free(name);

        // Get the next name or finish message ("THATS ALL;\0")
        return_value = recvfrom_wrapper_timeout(socket_file_descriptor, &name, connected_addrinfo->ai_addr, &(connected_addrinfo->ai_addrlen), timeout_in_s);
        if (return_value == TIMEOUT) {
            printf("\nO servidor não respondeu a tempo (timeout de %ds atingido)\n\n", timeout_in_s);
            return;
        }
    }
    printf("\n");
    free(name);
}

void print_options_list() {
    printf("Escolha uma das seguintes opções:\n");

    printf("(1) dado o email de um perfil, retornar nome, sobrenome e foto;\n");
    // opt_get_full_name_and_picture_from_profile()
    printf("(2) dado o email de um perfil, retornar suas informações;\n");
    // opt_get_profile()
    printf("(3) listar todas as informações de todos os perfis;\n");
    // opt_get_profiles()

    printf("(%s) sair.\n\n", OPT_QUIT_STR);
}

// CONNECTION ///////////////////////////

void get_server_addrinfo(const char *hostname, const char *port, struct addrinfo **server_addrinfo) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = STRUCT_IPV4;
    hints.ai_socktype = UDP;

    int return_value;
    if ((return_value = getaddrinfo(hostname, port, &hints, server_addrinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
        exit(1);
    }
}

void create_socket(struct addrinfo *server_addrinfo, int *socket_file_descriptor, struct addrinfo **p) {
    // loop through all the results and make a socket
    for (*p = server_addrinfo; *p != NULL; *p = (*p)->ai_next) {
        if ((*socket_file_descriptor = socket((*p)->ai_family, (*p)->ai_socktype, (*p)->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        exit(2);
    }
}
