#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "libs.h"
#include "input_parser.h"

void just_do_it(struct addrinfo *connected_addrinfo, int socket_file_descriptor);

// CONNECTION ///////////////////////////
void get_server_addrinfo(const char *hostname, const char *port, struct addrinfo **server_addrinfo);
void create_socket(struct addrinfo *server_addrinfo, int *socket_file_descriptor, struct addrinfo **p);

// OPTIONS //////////////////////////////
void print_options_list();
void opt_get_full_name_and_picture_from_profile(int socket_file_descriptor); // (1)
void opt_get_profile(int socket_file_descriptor);                            // (2)
void opt_get_profiles(int socket_file_descriptor);                           // (3)

void read_option(int *option);
void save_result_to_file(char *result_header, char *result_buffer);
void save_img(char *file_name, char *buffer, int img_size);

#endif
