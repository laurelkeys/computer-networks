#include "input_parser.h"

int validate_input(int code, char *no_input_msg, char *too_long_msg, char *input_buffer) {
    if (code == NO_INPUT) {
        printf ("%s\n", no_input_msg);
        return !OK;
    }

    if (code == TOO_LONG) {
        printf ("%s [%s]\n", too_long_msg, input_buffer);
        return !OK;
    }

    return OK;
}

int get_input(char *input, char *input_buffer, size_t max_size) {
    if (input != NULL) {
        printf("%s", input);
        fflush(stdout);
    }

    if (fgets(input_buffer, max_size, stdin) == NULL) return NO_INPUT;

    int next_char, too_long;
    if (input_buffer[strlen(input_buffer) - 1] != '\n') {
        too_long = 0;
        while (((next_char = getchar()) != '\n') && (next_char != EOF)) {
            too_long = 1;
        }
        return too_long ? TOO_LONG : OK;
    }

    input_buffer[strlen(input_buffer) - 1] = '\0';
    return OK;
}
