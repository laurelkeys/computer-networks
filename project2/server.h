#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/wait.h>
#include <signal.h>
#include <sqlite3.h>
#include <string.h>

#include "libs.h"

#define BACKLOG 10 // how many pending connections the queue will hold

void receive_message(int socket_file_descriptor);

void sendto_file_to_client(int socket_file_descriptor, FILE *f);
void sendto_img_to_client(int socket_file_descriptor, FILE *f);

// CONNECTION ///////////////////////////
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
void get_server_addrinfo(const char *hostname, const char *port, struct addrinfo **server_addrinfo);
void bind_to_first_match(struct addrinfo *server_addrinfo, int *socket_file_descriptor, struct addrinfo **p);

// OPTIONS //////////////////////////////
void opt_get_full_name_and_picture_from_profile(int socket_file_descriptor); // (1)
void opt_get_profile(int socket_file_descriptor);                            // (2)
void opt_get_profiles(int socket_file_descriptor);                           // (3)

// DATABASE /////////////////////////////
void init_db();
void exit_cleanup();
void execute_sql(char * sql);
int send_info_callback(void *_unused, int length, char **column_content, char **column_name);

void get_all_emails_sql(char **emails);
void get_full_name_from_profile_sql(char *email);
void opt_get_profile_sql(char *email);
void opt_get_profiles_sql();

#endif
