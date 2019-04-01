#ifndef _COMMON_H_
#define _COMMON_H_

#include "libs.h"

#define HEADER_SIZE 12 + 1 // + 1 to add '\0'

int send_wrapper(int file_descriptor, char *message, int verbose);
int recv_wrapper(int file_descriptor, char **buffer, int verbose);
int send_img_wrapper(int file_descriptor, char * message, int msg_size, int verbose);
int recv_img_wrapper(int file_descriptor, char **buffer, int *size, int verbose);

#endif