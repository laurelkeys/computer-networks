#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/wait.h>
#include <signal.h>

#include "libs.h"

#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at onces

void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
void receive_message(int socket_file_descriptor);

#endif
