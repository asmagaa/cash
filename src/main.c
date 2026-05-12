#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CASH_RL_BUFSIZE 1024

void cash_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = cash_read_line();
        args = cash_split_line(line);
        status = cash_execute(args);

        free(line);
        free(args);
    } while (status);
}

char *cash_read_line(void) {
    int bufsize = CASH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "cash: memory allocation error.\n");
        exit(-1);
    }

    while (1) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        else {
            buffer[position] = c;
        }
        
        position++;

        if (position >= bufsize) {
            bufsize += CASH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);

            if (!buffer) {
                fprintf(stderr, "cash: memory allocation error.\n");
                exit(-1);
            }
        }
    }
}

void cash_split_line() {
    // ...
}

void cash_execute() {
    // ...
}

int main(int argc, char **argv) {
    cash_loop();
    return 0;
}
