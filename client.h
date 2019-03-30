#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "libs.h"

#define MAXDATASIZE 100 // max number of bytes we can get at once

void get_server_addrinfo(const char *hostname, const char *port, struct addrinfo **server_addrinfo);
void connect_to_first_match(struct addrinfo *server_addrinfo, int *socket_file_descriptor, struct addrinfo **p);
void *get_in_addr(struct sockaddr *sa);
void receive_message(int socket_file_descriptor);

void print_options_list();
void just_do_it(struct addrinfo *connected_addrinfo, int socket_file_descriptor);

void opt_get_profiles_filtering_education(int socket_file_descriptor);
void opt_get_skills_filtering_city(int socket_file_descriptor);
void opt_add_skill_to_profile(int socket_file_descriptor);
void opt_get_experience_from_profile(int socket_file_descriptor);
void opt_get_profiles(int socket_file_descriptor);
void opt_get_profile(int socket_file_descriptor);

#endif
