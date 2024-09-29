#pragma once

#include <lexer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    int job_number;
    pid_t pid;
    char *command;
    bool done;
} job_t;

int find_free_job_slot(job_t *jobs, int max_jobs);
void add_job(job_t *jobs, int max_jobs, int *num_jobs, pid_t pid, const char *command_line, int *next_job_number);
int check_jobs(job_t *jobs, int max_jobs, int *num_jobs);
char* reconstruct_command_line(tokenlist ** commands, int num_cmds);
