#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/time.h>
#include "libs.h"

#define HEADER_SIZE 12 + 1 // + 1 to add '\0'

void log_timestamp(char *label);

int sendto_wrapper(int file_descriptor, char *message, const struct sockaddr *dest_addr, socklen_t addrlen);
int recvfrom_wrapper(int file_descriptor, char **buffer, struct sockaddr *src_addr, socklen_t *addrlen);

int sendto_img_wrapper(int file_descriptor, char * message, int msg_size, const struct sockaddr *dest_addr, socklen_t addrlen);
int recvfrom_img_wrapper(int file_descriptor, char **buffer, int *size, struct sockaddr *src_addr, socklen_t *addrlen);

#endif
