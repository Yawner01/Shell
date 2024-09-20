#ifndef COMMANDS_H
#define COMMANDS_H

#include "lexer.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int number;
    pid_t pid;
    char* command;
} job_t;

void add_to_history(tokenlist* history, char* command);

bool is_valid_internal_command(const char* command);

bool is_valid_external_command(const char* command);

bool is_valid_command(const char* command);

// functions to handle internal commands, or built-in functions, of shell
//----------------------------------------------------

// Exits the process after background processes are completed and prints the last 3 commands.
void cmd_exit(tokenlist* history);

// Changes current working directory. If no arguments given, then $HOME becomes the current directory.
void cmd_cd(tokenlist* args);

// Prints a list of active background processes.
void cmd_jobs(job_t* jobs, int num_jobs);

#endif
