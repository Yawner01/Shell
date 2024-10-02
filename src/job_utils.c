#include "job_utils.h"
#include "env_utils.h"
#include "lexer.h"

#define MAX_JOBS 10  // Define the maximum number of jobs

int find_free_job_slot(job_t *jobs) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_number == 0) {
            return i;
        }
    }
    return -1;
}

void add_job(job_t *jobs, int *num_jobs, pid_t pid, const char *command_line, int *next_job_number) {
    if (*num_jobs >= MAX_JOBS) {
        fprintf(stderr, "Maximum background jobs reached. Cannot add more\n");
        return;
    }

    int slot = find_free_job_slot(jobs);
    if (slot == -1) {
        fprintf(stderr, "No free job slots available.\n");
        return;
    }

    jobs[slot].job_number = (*next_job_number)++;
    jobs[slot].pid = pid;
    jobs[slot].done = false;
    jobs[slot].command = my_strdup(command_line);
    if (jobs[slot].command == NULL) {
        perror("my_strdup");
        exit(1);
    }

    (*num_jobs)++;

    printf("[%d] %d\n", jobs[slot].job_number, jobs[slot].pid);
}

int check_jobs(job_t *jobs, int *num_jobs) {
    int status;
    pid_t pid;
    int background_process_completed = 0;

    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_number != 0 && !jobs[i].done) {
            // Call waitpid with WNOHANG to avoid blocking
            pid = waitpid(jobs[i].pid, &status, WNOHANG);
            
            if (pid == 0) {
                //process is still running,skip to next job
                continue;
            }
            else if (pid == -1) {
                //ignore error, just skip job if waitpid fails
                continue;
            }
            else if (pid == jobs[i].pid) {
                //completed
                printf("\n[%d] + done   %s\n", jobs[i].job_number, jobs[i].command);

                free(jobs[i].command);
                jobs[i].command = NULL;
                jobs[i].job_number = 0;
                jobs[i].pid = 0;
                jobs[i].done = false;
                (*num_jobs)--;

                background_process_completed = 1;
            }
        }
    }
    return background_process_completed;
}

char* reconstruct_command_line(tokenlist ** commands, int num_cmds) {
    size_t length = 0;
    for (int i = 0; i < num_cmds; i++) {
        for (int j = 0; j < commands[i]->size; j++) {
            length += strlen(commands[i]->items[j]) + 1;
        }
        if (i < num_cmds - 1) {
            length += 2;
        }
    }

    char *cmd_line = malloc(length + 1);
    if (cmd_line == NULL) {
        perror("malloc");
        exit(1);
    }

    cmd_line[0] = '\0';

    for (int i = 0; i < num_cmds; i++) {
        for (int j = 0; j < commands[i]->size; j++) {
            strcat(cmd_line, commands[i]->items[j]);
            strcat(cmd_line, " ");
        }
        if (i < num_cmds - 1) {
            strcat(cmd_line, "| ");
        }
    }

    size_t len = strlen(cmd_line);

    if (len > 0 && cmd_line[len-1] == ' ') {
        cmd_line[len-1] = '\0';
    }

    return cmd_line;
}
