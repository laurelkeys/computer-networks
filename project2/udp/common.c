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

int sendto_wrapper(int file_descriptor, char *message, const struct sockaddr *dest_addr, socklen_t addrlen) {
    int msg_size = strlen(message);
    int n;

    n = sendto(file_descriptor, message, msg_size, 0, dest_addr, addrlen);
    if (n == -1) {
        perror("sendto_wrapper: common: send");
        return 1;
    }

    return 0;
}

int sendto_img_wrapper(int file_descriptor, char *message, int msg_size, const struct sockaddr *dest_addr, socklen_t addrlen) {
    // Send picture
    int n;
    n = sendto(file_descriptor, message ? message : "", msg_size, 0, dest_addr, addrlen);
    if (n == -1) {
        perror("sendto_img_wrapper: common: send");
        return 1;
    }

    return 0;
}

int recvfrom_wrapper(int file_descriptor, char **buffer, struct sockaddr *src_addr, socklen_t *addrlen) {
    int msg_size;

    *buffer = malloc(sizeof(char*) * MAX_MSG_SIZE);
    msg_size = recvfrom(file_descriptor, (*buffer), MAX_MSG_SIZE, 0, src_addr, addrlen);
    if (msg_size == -1) {
        perror("recvfrom_wrapper: common: msg recv");
        return 1;
    }

    (*buffer)[msg_size] = '\0';

    return msg_size; // return # of bytes not received
}

int recvfrom_wrapper_timeout(int file_descriptor, char **buffer, struct sockaddr *src_addr, socklen_t *addrlen, int timeout_in_s) {
    *buffer = malloc(sizeof(char*) * MAX_MSG_SIZE);

    struct pollfd fd;
    fd.fd = file_descriptor;
    fd.events = POLLIN;

    if (poll(&fd, 1, timeout_in_s*1000) > 0) {
        int msg_size;
        msg_size = recvfrom(file_descriptor, (*buffer), MAX_MSG_SIZE, 0, src_addr, addrlen);
        if (msg_size == -1) {
            perror("recvfrom_wrapper_timeout: common: msg recv");
            return 1;
        }

        (*buffer)[msg_size] = '\0';

        return msg_size; // return # of bytes not received
    }
    return TIMEOUT;
}

int recvfrom_img_wrapper(int file_descriptor, char **buffer, int *size, struct sockaddr *src_addr, socklen_t *addrlen) {
    int msg_size;

    *buffer = malloc(sizeof(char*) * MAX_MSG_SIZE);
    msg_size = recvfrom(file_descriptor, (*buffer), MAX_MSG_SIZE, 0, src_addr, addrlen);
    if (msg_size == -1) {
        perror("recvfrom_img_wrapper: common: msg recv");
        return 1;
    }

    (*buffer)[msg_size] = '\0';

    *size = msg_size;
    return msg_size; // return # of bytes not received
}

int recvfrom_img_wrapper_timeout(int file_descriptor, char **buffer, int *size, struct sockaddr *src_addr, socklen_t *addrlen, int timeout_in_s) {
    *buffer = malloc(sizeof(char*) * MAX_MSG_SIZE);

    struct pollfd fd;
    fd.fd = file_descriptor;
    fd.events = POLLIN;

    if (poll(&fd, 1, timeout_in_s*1000) > 0) {
        int msg_size;
        msg_size = recvfrom(file_descriptor, (*buffer), MAX_MSG_SIZE, 0, src_addr, addrlen);
        if (msg_size == -1) {
            perror("recvfrom_img_wrapper_timeout: common: msg recv");
            return 1;
        }

        (*buffer)[msg_size] = '\0';

        *size = msg_size;
        return msg_size; // return # of bytes not received
    }
    return TIMEOUT;
}
