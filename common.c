#include "common.h"

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

int send_wrapper(int file_descriptor, char * message, int verbose) {
    int msg_size = strlen(message);
    char *header = _msg_size_to_str(msg_size);
    msg_size += HEADER_SIZE - 1;
    if (verbose) printf("send_wrapper: msg_size: %d\n", msg_size);

    char buffer[msg_size];
    sprintf(buffer, "%s%s", header, message);
    free(header);
    if (verbose) printf("send_wrapper: header+message: '''%s'''\n", buffer);

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

int recv_wrapper(int file_descriptor, char **buffer, int verbose) {
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
            break; 
        }

        bytes_received += n;
    }

    header_buffer[HEADER_SIZE - 1] = '\0';
    if (verbose) printf("recv_wrapper: header_buffer: %s\n", header_buffer);

    if (header_buffer[HEADER_SIZE - 2] != ';') perror("Header ending not found");

    int msg_size = _msg_size_to_int(header_buffer);
    if (verbose) printf("recv_wrapper: msg_size: %d\n", msg_size);

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
        if (verbose) printf("recv_wrapper: bytes_received: %d\n", bytes_received);
    }

    (*buffer)[msg_size] = '\0';
    if (verbose) printf("recv_wrapper: buffer: '''%s'''\n", *buffer);

    return msg_size - bytes_received; // return # of bytes not received
}
