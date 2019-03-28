#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/wait.h>
#include <signal.h>
#include <sqlite3.h>
#include <string.h>

#include "libs.h"

#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at onces

void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
void receive_messages(int socket_file_descriptor);
void exit_cleanup();

// DB related stuff
void init_db();
void execute_sql(char * sql);
void opt_get_profiles_filtering_education();
void opt_get_skills_filtering_city();
void opt_add_skill_to_profile();
void opt_get_experience_from_profile();
void opt_get_profiles();
void opt_get_profile();

#endif
