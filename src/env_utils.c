#include "env_utils.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define PERMISSIONS (S_IRUSR | S_IWUSR)

void get_env_variable(char *variable, char ** token) {
	char *env_value = getenv(variable);
	if (env_value != NULL) {
		*token = realloc(*token, strlen(env_value) + 1);

		if (*token == NULL) {
		    fprintf(stderr, "Memory allocation failure\n");
			exit(1);
		}

		strcpy(*token, env_value);
	} else {
		printf("Environment variable %s not found.\n", variable);
	}
}

void replace_tilde(char ** token) {
    char *home = getenv("HOME");
    if (home != NULL) {
        *token = realloc(*token, strlen(home) + strlen(*token));

        if (*token == NULL) {
            fprintf(stderr, "Memory allocation falure\n");
            exit(1);
        }

        char rest[strlen(*token + 1) + 1];
        strcpy(rest, *token + 1);

        strcpy(*token, home);

        strcat(*token, rest);
    } else {
        printf("$HOME environment variable not found\n");
    }
}

void search_path(tokenlist *tokens) { //changed command to input to take in the whole line
    char *path = getenv("PATH");
    if (path == NULL) {
        fprintf(stderr, "Failed to get $PATH\n");
        exit(1);
    }

    char *command = tokens->items[0]; //first token is command
    char *path_copy = my_strdup(path);
    if (path_copy == NULL) {
        fprintf(stderr, "Memory allocation failure\n");
        exit(1);
    }

    char *directory = strtok(path_copy, ":");
    char full_path[1024];
    int input_fd = -1, output_fd = -1;

    char *input_file = NULL;
    char *output_file = NULL;

    for (int i = 0; i < tokens->size; ++i) {
        if(strcmp(tokens->items[i], ">") == 0 && i + 1 < tokens->size) {
            output_file = tokens->items[i + 1];
            tokens->items[i] = NULL;
        } else if (strcmp(tokens->items[i], "<") == 0 && i + 1 < tokens->size) {
            input_file = tokens->items[i + 1];
            tokens->items[i] = NULL;
        }
    }

    while (directory != NULL) {
        if (directory[strlen(directory) - 1] == '/') {
            snprintf(full_path, sizeof(full_path), "%s%s", directory, command);
        } else {        
            snprintf(full_path, sizeof(full_path), "%s/%s", directory, command);
        }

        if (access(full_path, X_OK) == 0) {
            printf("Command found: %s\n", full_path);

            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Fork failed\n"); //verify its not negative
                exit(1);
            } else if (pid == 0) { //child process
                // Input redirection
                if (input_file != NULL) {
                    input_fd = open(input_file, O_RDONLY);
                    if (input_fd == -1) {
                        perror("Error opening input file");
                        exit(1);
                    }
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                }

                //Output Redirection
                if (output_file != NULL) {
                    output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, PERMISSIONS);
                    if (output_fd == -1) {
                        perror("Error opening output file");
                        exit(1);
                    }
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);
                }

                tokens->items[0] = full_path;
                execv(full_path, tokens->items);

                perror("execv failed");
                free(path_copy);
                exit(1);
            } else { //parent waits
                int status;
                wait(&status);
                if (WIFEXITED(status)) {
                    printf("child exited with status: %d\n", WEXITSTATUS(status));
                    return;
                } else {
                    printf("child did not exit right\n");
                }
            }
        }

        directory = strtok(NULL, ":");
    }

    printf("Command '%s' not found\n", command);
    free(path_copy);
    free_tokens(tokens);
}

char *my_strdup(const char *src) {
    char *dup = malloc(strlen(src) + 1);
    if (dup != NULL) {
        strcpy(dup, src);
    }
    return dup;
}