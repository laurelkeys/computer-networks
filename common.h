#ifndef COMMON_H
#define COMMON_H

#include "libs.h"

#define HEADER_SIZE 12

int send_wrapper(int file_descriptor, char * message);
int recv_wrapper(int file_descriptor, char * buffer);

#endif