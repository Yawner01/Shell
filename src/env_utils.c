#include "env_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
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

void search_path(char *command) {
    char *path = getenv("PATH");
    if (path == NULL) {
        fprintf(stderr, "Failed to get $PATH\n");
        exit(1);
    }

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
                exit(EXIT_FAILURE);
            } 
            else if (pid == 0) { //child process
                char *args[100]; //this allows for multiple arguments like the ls -al
                int i = 0;

                //here i am going to split the command into tokens and store them
                char *token = strtok(command, " ");
                while (token != NULL) {
                    args[i++] = token; //storing
                    token = strtok(NULL, " ");
                }
                args[i] = NULL; //null terminator

                execv(full_path, args); //execute command with arguments 

                fprintf(stderr, "Error executing: %s\n", full_path);//failure notif if it doesnt work
                exit(EXIT_FAILURE);
                
            } 
            else { //parent
                //here is where the process waits for
                //the child process to complete
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) { 
                    printf("Child exited with status %d\n", WEXITSTATUS(status)); //failure notif if it doesnt work
                }

            }

            
            free(path_copy);
            return;
        }

        directory = strtok(NULL, ":");
    }

    printf("Command '%s' not found\n", command);
    free(path_copy);
}

char *my_strdup(const char *src) {
    char *dup = malloc(strlen(src) + 1);
    if (dup != NULL) {
        strcpy(dup, src);
    }
    return dup;
}