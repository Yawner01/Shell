#include "env_utils.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

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

char *find_path(tokenlist *tokens) { //changed command to input to take in the whole line
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

    while (directory != NULL) {
        if (directory[strlen(directory) - 1] == '/') {
            snprintf(full_path, sizeof(full_path), "%s%s", directory, command);
        } else {        
            snprintf(full_path, sizeof(full_path), "%s/%s", directory, command);
        }

        if (access(full_path, X_OK) == 0) {
            printf("Command found: %s\n", full_path);
            free(path_copy);
            return my_strdup(full_path);
        }

        directory = strtok(NULL, ":");
    }

    printf("Command '%s' not found\n", command);
    free(path_copy);
    free_tokens(tokens);
    return NULL;
}

char *my_strdup(const char *src) {
    char *dup = malloc(strlen(src) + 1);
    if (dup != NULL) {
        strcpy(dup, src);
    }
    return dup;
}