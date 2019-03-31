#include "common.h"

char* msg_size_to_str(int size) {
    char * result = malloc(sizeof(char)*HEADER_SIZE);
    int index = HEADER_SIZE - 2;
    *(result + HEADER_SIZE - 1) = ';';
    while (size > 0) {
        *(result + index) = (size % 10) + '0';
        index--;
        size /= 10;
    }
    while (index >= 0) {
        *(result + index) = '0';
        index--;
    }
    return result;
}

int msg_size_to_int(char * size) {
    int result = 0;
    for (int i = 0; i < HEADER_SIZE-1; i++) {
        result *= 10;
        result += *(size + i) - '0';
    }
    return result;
}

int send_wrapper(int file_descriptor, char * message) {
    int msg_size = strlen(message);
    char * header = msg_size_to_str(msg_size);
    msg_size += HEADER_SIZE;
    char buffer[msg_size];
    strcat(buffer, header);
    strcat(buffer, message);

    int bytes_sent = 0; // how many bytes we've sent
    int bytes_left = msg_size; // how many we have left to send
    int n;
    while (bytes_sent < msg_size) {
        n = send(file_descriptor, buffer+bytes_sent, bytes_left, 0);
        if (n == -1) { break; }
        bytes_sent += n;
        bytes_left -= n;
    }
    
    return bytes_left; // return # of unsent bytes
}

int recv_wrapper(int file_descriptor, char * buffer) {
    char header_buffer[HEADER_SIZE];
    recv(file_descriptor, header_buffer, HEADER_SIZE, 0);
    int msg_size = msg_size_to_int(header_buffer);

    buffer = malloc(sizeof(char)*msg_size);
    recv(file_descriptor, buffer, msg_size, 0);
    return 0;
}

int main(void) {
    char * str = msg_size_to_str(12345678);
    printf("%s\n", str);
    int size = msg_size_to_int("00012345678;");
    printf("%d\n", size);
}