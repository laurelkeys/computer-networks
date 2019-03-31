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

    return 0;
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