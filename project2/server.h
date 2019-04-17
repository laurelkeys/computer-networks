#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/wait.h>
#include <signal.h>
#include <sqlite3.h>
#include <string.h>

#include "libs.h"

#define BACKLOG 10 // how many pending connections queue will hold

void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
void receive_messages(int socket_file_descriptor);
void exit_cleanup();
void get_server_addrinfo(const char *hostname, const char *port, struct addrinfo **server_addrinfo);
void bind_to_first_match(struct addrinfo *server_addrinfo, int *socket_file_descriptor, struct addrinfo **p);

void opt_get_profiles_filtering_education(int socket_file_descriptor);
void opt_get_skills_filtering_city(int socket_file_descriptor);
void opt_add_skill_to_profile(int socket_file_descriptor);
void opt_get_experience_from_profile(int socket_file_descriptor);
void opt_get_profiles(int socket_file_descriptor);
void opt_get_profile(int socket_file_descriptor);

void send_file_to_client(int socket_file_descriptor, FILE *f);
void send_picture_to_client(int socket_file_descriptor, FILE *f);

// DB related stuff
void init_db();
void execute_sql(char * sql);
int send_info_callback(void *not_used, int length, char **column_content, char **column_name);

void opt_get_profiles_filtering_education_sql(char * education);
void opt_get_skills_filtering_city_sql(char * city);
void opt_add_skill_to_profile_sql(char * email, char * skill);
void opt_get_experience_from_profile_sql(char * email);
void opt_get_profiles_sql();
void opt_get_profile_sql(char * email);
void get_all_emails_sql(char **emails);

#endif
