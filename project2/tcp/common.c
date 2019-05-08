#include "common.h"

void log_timestamp(char *label, int opt_file) {
    struct timeval t;
    gettimeofday(&t, NULL);
    char file_name_buffer[strlen(FILE_TIMESTAMPS_PREFIX) + 1 + strlen(FILE_TIMESTAMPS_EXTENSION)];
    sprintf(file_name_buffer, "%s%d%s", FILE_TIMESTAMPS_PREFIX, opt_file, FILE_TIMESTAMPS_EXTENSION);
    printf("file_name_buffer: %s\n", file_name_buffer);
    FILE *f;
    f = fopen(file_name_buffer, "a");
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

int send_wrapper(int file_descriptor, char *message) {
    int msg_size = strlen(message);
    char *header = _msg_size_to_str(msg_size);
    msg_size += HEADER_SIZE - 1;

    char buffer[msg_size];
    sprintf(buffer, "%s%s", header, message);
    free(header);

    int bytes_sent = 0; // how many bytes we've sent
    int bytes_left = msg_size; // how many we have left to send
    int n;
    while (bytes_sent < msg_size) {
        n = send(file_descriptor, buffer + bytes_sent, bytes_left, 0);
        if (n == -1) {
            perror("send_wrapper: common: send");
            break;
        } else if (n <= 0) {
            perror("send_wrapper: common: n <= 0");
            break;
        }

        bytes_sent += n;
        bytes_left -= n;
    }

    return bytes_left; // return # of unsent bytes
}

int send_img_wrapper(int file_descriptor, char *message, int msg_size) {
    char *header = _msg_size_to_str(msg_size);

    int n;

    // Send header
    int bytes_sent = 0; // how many bytes we've sent
    int bytes_left = HEADER_SIZE - 1; // how many we have left to send
    while (bytes_sent < HEADER_SIZE - 1) {
        n = send(file_descriptor, header + bytes_sent, bytes_left, 0);
        if (n == -1) {
            perror("send_img_wrapper: common: send");
            break;
        } else if (n <= 0) {
            perror("send_img_wrapper: common: n <= 0");
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
        n = send(file_descriptor, message + bytes_sent, bytes_left, 0);
        if (n == -1) {
            perror("send_img_wrapper: common: send");
            break;
        } else if (n <= 0) {
            perror("send_img_wrapper: common: n <= 0");
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

int recv_wrapper(int file_descriptor, char **buffer) {
    char header_buffer[HEADER_SIZE];
    int bytes_received = 0;
    int n;
    while (bytes_received < HEADER_SIZE - 1) {
        n = recv(file_descriptor, header_buffer + bytes_received, HEADER_SIZE - 1 - bytes_received, 0);
        if (n == -1) {
            perror("recv_wrapper: common: header recv");
            break;
        } else if (n <= 0) {
            perror("recv_wrapper: common: header n <= 0");
            printf("n=%d", n);
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
        n = recv(file_descriptor, (*buffer) + bytes_received, msg_size - bytes_received, 0);
        if (n == -1) {
            perror("recv_wrapper: common: msg recv");
            break;
        } else if (n <= 0) {
            perror("recv_wrapper: common: n <= 0");
            break;
        }

        bytes_received += n;
    }

    (*buffer)[msg_size] = '\0';

    return msg_size - bytes_received; // return # of bytes not received
}

int recv_img_wrapper(int file_descriptor, char **buffer, int *size) {
    char header_buffer[HEADER_SIZE];
    int bytes_received = 0;
    int n;
    while (bytes_received < HEADER_SIZE - 1) {
        n = recv(file_descriptor, header_buffer + bytes_received, HEADER_SIZE - 1 - bytes_received, 0);
        if (n == -1) {
            perror("recv_img_wrapper: common: header recv");
            break;
        } else if (n <= 0) {
            perror("recv_img_wrapper: common: header n <= 0");
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
        n = recv(file_descriptor, (*buffer) + bytes_received, msg_size - bytes_received, 0);
        if (n == -1) {
            perror("recv_img_wrapper: common: msg recv");
            break;
        } else if (n <= 0) {
            perror("recv_img_wrapper: common: n <= 0");
            break;
        }

        bytes_received += n;
    }

    (*buffer)[msg_size] = '\0';

    *size = msg_size;
    return msg_size - bytes_received; // return # of bytes not received
}
