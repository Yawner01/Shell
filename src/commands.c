#include "commands.h"
#include "env_utils.h"
#include "job_utils.h"
#include <stdlib.h>
// Exits the process after background processes are completed and prints the last 3 commands.
void cmd_exit(tokenlist* history) {

    // wait for any background processes to finish
    while (waitpid(-1, NULL, WNOHANG) > 0);

    // validate tokenlist
    if (history != NULL) {
        
        // print last 3 commands in from history tokenlist
        printf("Last command(s):\n");

        if (history->size == 0) {
            printf("no valid commands\n");
        }
        else if (history->size >= 3) {
            for (size_t i = history->size - 3; i < history->size; ++i) {
                printf("%s\n", history->items[i]);
            }
        } else {
            printf("%s\n", history->items[history->size - 1]);
        }
    }
    else {
        printf("[ERROR] history tokenlist is null, ignoring\n");
    }

    // exit the process
    exit(0);
}

// Changes current working directory. If no arguments given, then $HOME becomes the current directory.
void cmd_cd(tokenlist* args) {

    if (args == NULL) {
        
        // go to home directory
        if (chdir(getenv("HOME")) != 0) {
            perror("cd");
        }
    }
    else if (args->items == NULL) {

        // go to home directory
        if (chdir(getenv("HOME")) != 0) {
            perror("cd");
        } 
    }
    else if (args->items[1] == NULL) {
        
        // go to home directory
        if (chdir(getenv("HOME")) != 0) {
            perror("cd");
        }    
    }
    else if (args->items[2] != NULL) {
        
        fprintf(stderr, "cd: too many arguments\n");
    }
    else {
        // go to specified directory
        if (chdir(args->items[1]) != 0) {
            perror("cd");
        }
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (setenv("PWD", cwd, 1) != 0) {
            perror("setenv error");
        }
    } else {
        perror("getcwd error");
    }
}

// Prints a list of active background processes.
#define MAX_JOBS 10  // Define a constant for max jobs

void cmd_jobs(job_t* jobs, int* num_jobs) {
    check_jobs(jobs, MAX_JOBS, num_jobs);

    int active_jobs = 0;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_number != 0 && !jobs[i].done && jobs[i].command != NULL) {
            printf("[%d]+ %d %s\n", jobs[i].job_number, jobs[i].pid, jobs[i].command);
            active_jobs++;
        }
    }

    if (active_jobs == 0) {
        printf("no active background processes\n");
    }
}




void add_to_history(tokenlist* history, char* command) {
	
    // validate history
    if (history == NULL) {
		printf("[ERROR] attempted to add command to null history, ignoring\n");
        return;
	}

    // validate command
    if (command == NULL) {
        printf("[ERROR] attempted to add null command to history, ignoring\n");
        return;
    }

	const size_t MAX_HIST_CMDS = 10;

    // add command to history if not full, otherwise remove
    // oldest command from history and insert most recent one
	if (history->size < MAX_HIST_CMDS) {
		add_token(history, command);
	}
	else {
		free(history->items[0]);

        // shift tokens left
		for (size_t i = 1; i < history->size; ++i) {
			history->items[i - 1] = history->items[i];
		}

        // insert most recent command
		history->items[history->size - 1] = my_strdup(command);
	}
}

bool is_valid_internal_command(const char* command) {
    return (strcmp(command, "cd")   == 0 ||
            strcmp(command, "jobs") == 0);
}

bool is_valid_external_command(const char* command) {
    char* path = getenv("PATH");

    if (path == NULL) {
        return false;
    }

    char* path_copy = my_strdup(path);
    char* dir = strtok(path_copy, ":");
    char  full_path[256];

    while (dir != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return true;
        }
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return false;
}

bool is_valid_command(const char* command) {
    return is_valid_internal_command(command) || is_valid_external_command(command);
}
