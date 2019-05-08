#ifndef _LIBS_H_
#define _LIBS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common.h"

#ifndef PORT
#define PORT "3490" // the port clients will connect to
#endif

#define UDP SOCK_DGRAM
#define TCP SOCK_STREAM

#define STRUCT_IPV4 AF_INET
#define STRUCT_IPV6 AF_INET6
#define STRUCT_IPVX AF_UNSPEC

#define SOCKET_IPV4 PF_INET
#define SOCKET_IPV6 PF_INET6

#define OPT_QUIT_STR "4"
#define TIMEOUT -2

#define FILE_CLIENT "option_results.txt"
#define FILE_SERVER "last_query_result.txt"
#define FILE_TIMESTAMPS_PREFIX "timestamps"
#define FILE_TIMESTAMPS_EXTENSION ".txt"

#endif