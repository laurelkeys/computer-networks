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

// DB related stuff
void init_db();
void execute_sql(char * sql);
int send_info_callback(void *not_used, int length, char **column_content, char **column_name);

void opt_get_profiles_filtering_education(char * education);
void opt_get_skills_filtering_city(char * city);
void opt_add_skill_to_profile(char * email, char * skill);
void opt_get_experience_from_profile(char * email);
void opt_get_profiles();
void opt_get_profile(char * email);

#endif
