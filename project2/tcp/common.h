#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/time.h>
#include "libs.h"

#define HEADER_SIZE 12 + 1 // + 1 to add '\0'

void log_timestamp(char *label, int opt_file);

int send_wrapper(int file_descriptor, char *message);
int recv_wrapper(int file_descriptor, char **buffer);

int send_img_wrapper(int file_descriptor, char * message, int msg_size);
int recv_img_wrapper(int file_descriptor, char **buffer, int *size);

#endif
