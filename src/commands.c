#include "commands.h"

// Exits the process after background processes are completed and prints the last 3 commands.
void cmd_exit(tokenlist* history) {

    // wait for any background processes to finish
    while (waitpid(-1, NULL, WNOHANG) > 0);

    // validate tokenlist
    if (history != NULL) {
        
        // print last 3 commands in from history tokenlist
        printf("Last command(s):\n");

        if (history->size == 0) {
            printf("no commands\n");
        }
        else {
            for (size_t i = (history->size > 3 ? history->size - 3 : 0); i < history->size; ++i) {
                printf("%s\n", history->items[i]);
            }
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
}

// Prints a list of active background processes.
void cmd_jobs(job_t* jobs, int num_jobs) {

    if (num_jobs > 0) {
        for (size_t i = 0; i < num_jobs; ++i) {
            printf("[%d][%d] %s\n", jobs[i].number, jobs[i].pid, jobs[i].command);
        }
    }
    else {
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
		strcpy(history->items[history->size - 1], command);
	}
}

bool is_valid_internal_command(const char* command) {
    return (strcmp(command, "exit") == 0 ||
            strcmp(command, "cd")   == 0 ||
            strcmp(command, "jobs") == 0);
}

bool is_valid_external_command(const char* command) {
    char* path = getenv("PATH");
    char* dir = strtok(path, ":");
    char  full_path[256];

    while (dir != NULL) {
        snprintf(full_path, sizeof(full_path), "%s%s", dir, command);
        if (access(full_path, X_OK) == 0) {
            return true;
        }
        dir = strtok(NULL, ":");
    }
    return false;
}

bool is_valid_command(const char* command) {
    return is_valid_internal_command(command) || is_valid_external_command(command);
}