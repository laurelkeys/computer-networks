#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef PORT
#define PORT "3490"  // the port users will be connecting to
#endif

#define UDP SOCK_DGRAM
#define TCP SOCK_STREAM

#define STRUCT_IPV4 AF_INET
#define STRUCT_IPV6 AF_INET6
#define STRUCT_IPVX AF_UNSPEC

#define SOCKET_IPV4 PF_INET
#define SOCKET_IPV6 PF_INET6

typedef struct profile {
    char *email;
    char *name;
    char *surname;
    char *photo_url; // FIXME change to .png or .jpeg file
    char *city;
    char *education;
    char **skills; // array of strings
    char **prior_experience; // array of strings
} profile;
