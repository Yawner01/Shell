#include "env_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

void search_path(char *input) { //changed command to input to take in the whole line
    char *path = getenv("PATH");
    if (path == NULL) {
        fprintf(stderr, "Failed to get $PATH\n");
        exit(1);
    }

    tokenlist *tokens = get_tokens(input);
    if (tokens->size == 0) {
        fprintf(stderr, "No command providec\n");
        return;
    }

    char *command = tokens->items[0]; //first token is command
    char *path_copy = my_strdup(path);
    if (path_copy == NULL) {
        fprintf(stderr, "Memory allocation failure\n");
        exit(1);
    }

    char *directory = strtok(path_copy, ":");

    char full_path[1024];

    while (directory != NULL) {
        if (directory[strlen(directory) - 1] == '/') {
            snprintf(full_path, sizeof(full_path), "%s%s", directory, command);
        } else {        
            snprintf(full_path, sizeof(full_path), "%s/%s", directory, command);
        }

        if (access(full_path, X_OK) == 0) {
            printf("Command found: %s\n", full_path);

            //fork
            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Fork failed\n"); //verify its not negative
                exit(1);
            } 
            
            if (pid == 0) { //child process

                printf("[debug] Child process executing command: %s\n", full_path);
                excev(full_path, token->items);

                printf("[debug] Execution command %s failed.\n", full_path);
                exit(1);
                
            } 
            else {
                //parent waits
                int status;
                wait(&status);
                if (WIFEXITED(status)) {
                    printf("[debgu] child exited with status: %d\n", WEXITSTATUS(status));
                } else {
                    printf("[debug] child did not exit right\n");
                }

            }

            break;
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