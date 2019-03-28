#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "libs.h"

#define MAXDATASIZE 100 // max number of bytes we can get at once

void get_server_info(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
void connect_to_first_match(struct addrinfo *server_info, int *socket_file_descriptor, struct addrinfo **p);
void *get_in_addr(struct sockaddr *sa);
void receive_message(int socket_file_descriptor);

void opt_get_profiles_filtering_education();
void opt_get_skills_filtering_city();
void opt_add_skill_to_profile();
void opt_get_experience_from_profile();
void opt_get_profiles();
void opt_get_profile();

#endif
