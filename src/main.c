#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define CASH_RL_BUFSIZE 1024

#define CASH_TOK_BUFSIZE 64
#define CASH_TOK_DELIM "\t\r\n\a"

int cash_help(char **args);
int cash_exit(char **args);

char *builtin_str[] = {
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &cash_help,
    &cash_exit
};

int cash_num_builtin() {
    return sizeof(builtin_str) / sizeof(char *);
}

int cash_help(char **args) {
    int i;
    printf("commands:\n");

    for (i = 0; i < cash_num_builtin(); i++) {
        printf("%d. %s\n", i + 1, builtin_str[i]);
    }

    return 1;
}

int cash_exit(char **args) {
    return 0;
}

int cash_launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("cash");
        }
        exit(EXIT_FAILURE);
    } 
    else if (pid < 0) {
        perror("cash");
    }
    else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
    return 1;
}

int cash_execute(char **args) {
    int i;
    if (args[0] == NULL) {
        return -1;
    }

    for (i = 0; i < cash_num_builtin(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return cash_launch(args);
}

char *cash_read_line(void) {
    int bufsize = CASH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "cash: memory allocation error.\n");
        exit(EXIT_FAILURE);
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
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **cash_split_line(char *line) {
    int bufsize = CASH_RL_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    if (!tokens) {
        fprintf(stderr, "cash: memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, CASH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        if (position >= bufsize) {
            bufsize += CASH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "cash: memory allocation error.\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, CASH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

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

int main(int argc, char **argv) {
    cash_loop();
    return EXIT_SUCCESS;
}
