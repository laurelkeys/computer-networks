#include "common.h"

void log_timestamp(char *label) {
    struct timeval t;
    gettimeofday(&t, NULL);
    FILE *f;
    f = fopen(FILE_TIMESTAMPS,"a");
    if (f) {
        fprintf(f, "%s:%ldus\n", label, t.tv_usec);
        fclose(f);
    }
}

static char* _msg_size_to_str(int size) {
    char *result = malloc(sizeof(char) * HEADER_SIZE);
    *(result + HEADER_SIZE - 2) = ';';
    *(result + HEADER_SIZE - 1) = '\0';

    int index = HEADER_SIZE - 3;
    while (size > 0) {
        *(result + index) = (size % 10) + '0';
        index -= 1;
        size /= 10;
    }
    while (index >= 0) {
        *(result + index) = '0';
        index -= 1;
    }

    return result;
}

int sendto_wrapper(int file_descriptor, char *message, const struct sockaddr *dest_addr, socklen_t addrlen) {
    int msg_size = strlen(message);
    int n;

    n = sendto(file_descriptor, message, msg_size, 0, dest_addr, addrlen);
    if (n == -1) {
        perror("sendto_wrapper: common: send");
        return 0;
    }

    return 1; // return # of unsent bytes
}

int sendto_img_wrapper(int file_descriptor, char *message, int msg_size, const struct sockaddr *dest_addr, socklen_t addrlen) {
    return 1; //FIXME send imgs
    char *header = _msg_size_to_str(msg_size);

    int n;

    // Send header
    int bytes_sent = 0; // how many bytes we've sent
    int bytes_left = HEADER_SIZE - 1; // how many we have left to send
    while (bytes_sent < HEADER_SIZE - 1) {
        n = sendto(file_descriptor, header + bytes_sent, bytes_left, 0, dest_addr, addrlen);
        if (n == -1) {
            perror("sendto_img_wrapper: common: send");
            break;
        } else if (n <= 0) {
            perror("sendto_img_wrapper: common: n <= 0");
            break;
        }

        bytes_sent += n;
        bytes_left -= n;
    }
    free(header);

    // Send picture
    bytes_sent = 0;
    bytes_left = msg_size;
    while (bytes_sent < msg_size) {
        n = sendto(file_descriptor, message + bytes_sent, bytes_left, 0, dest_addr, addrlen);
        if (n == -1) {
            perror("sendto_img_wrapper: common: send");
            break;
        } else if (n <= 0) {
            perror("sendto_img_wrapper: common: n <= 0");
            break;
        }

        bytes_sent += n;
        bytes_left -= n;
    }

    return bytes_left; // return # of unsent bytes
}

static int _msg_size_to_int(char * size) {
    int result = 0;
    for (int i = 0; i < HEADER_SIZE - 2; i++) {
        if (*(size + i) != ';' && *(size + i) != '\0') {
            result *= 10;
            result += *(size + i) - '0';
        }
    }
    return result;
}

// TODO set a timeout if no message is received
int recvfrom_wrapper(int file_descriptor, char **buffer, struct sockaddr *src_addr, socklen_t *addrlen) {
    int msg_size;

    *buffer = malloc(sizeof(char*) * MAX_MSG_SIZE);
    msg_size = recvfrom(file_descriptor, (*buffer), MAX_MSG_SIZE, 0, src_addr, addrlen);
    if (msg_size == -1) {
        perror("recvfrom_wrapper: common: msg recv");
    }

    (*buffer)[msg_size] = '\0';

    return msg_size; // return # of bytes not received
}

// TODO set a timeout if no message is received
int recvfrom_img_wrapper(int file_descriptor, char **buffer, int *size, struct sockaddr *src_addr, socklen_t *addrlen) {
    return 1; //FIXME send imgs
    char header_buffer[HEADER_SIZE];
    int bytes_received = 0;
    int n;
    while (bytes_received < HEADER_SIZE - 1) {
        n = recvfrom(file_descriptor, header_buffer + bytes_received, HEADER_SIZE - 1 - bytes_received, 0, src_addr, addrlen);
        if (n == -1) {
            perror("recvfrom_img_wrapper: common: header recv");
            break;
        } else if (n <= 0) {
            perror("recvfrom_img_wrapper: common: header n <= 0");
            break;
        }

        bytes_received += n;
    }

    header_buffer[HEADER_SIZE - 1] = '\0';

    if (header_buffer[HEADER_SIZE - 2] != ';') perror("Header ending not found");

    int msg_size = _msg_size_to_int(header_buffer);

    *buffer = malloc(sizeof(char*) * (msg_size + 1));
    bytes_received = 0;
    while (bytes_received < msg_size) {
        n = recvfrom(file_descriptor, (*buffer) + bytes_received, msg_size - bytes_received, 0, src_addr, addrlen);
        if (n == -1) {
            perror("recvfrom_img_wrapper: common: msg recv");
            break;
        } else if (n <= 0) {
            perror("recvfrom_img_wrapper: common: n <= 0");
            break;
        }

        bytes_received += n;
    }

    (*buffer)[msg_size] = '\0';

    *size = msg_size;
    return msg_size - bytes_received; // return # of bytes not received
}
