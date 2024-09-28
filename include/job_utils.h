#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int job_number;
    pid_t pid;
    char *command;
    bool done;
} job_t;

const int   MAX_JOBS = 10;

int find_free_job_slot(job_t *jobs, int max_jobs);
void add_job(job_t *jobs, int max_jobs, int *num_jobs, pid_t pid, int *next_job_number);
void check_jobs(job_t *jobs, int max_jobs, int *num_jobs);