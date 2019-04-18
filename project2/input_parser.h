#ifndef _INPUT_PARSER_H_
#define _INPUT_PARSER_H_

#define NOT_OK  -1
#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2

#define MAX_INPUT_SIZE 128

#include <stdio.h>
#include <string.h>

int validate_input(int code, char *no_input_msg, char *too_long_msg, char *input_buffer);
int get_input(char *input_msg, char *input_buffer, size_t max_size);

#endif
