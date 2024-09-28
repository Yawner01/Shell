#include "job_utils.h"

int find_free_job_slot(job_t *jobs, int max_jobs) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].job_number == 0) {
            return i;
        }
    }
    return -1;
}

void add_job(job_t *jobs, int max_jobs, int *num_jobs, pid_t pid, int *next_job_number) {
    if (*num_jobs >= max_jobs) {
        fprintf(stderr, "Maximum background jobs reached. Cannot add more\n");
        return;
    }

    int slot = find_free_job_slot(jobs, max_jobs);
    if (slot == -1) {
        fprintf(stderr, "No free job slots available.\n");
        return;
    }

    jobs[slot].job_number = (*next_job_number)++;
    jobs[slot].pid = pid;
    jobs[slot].done = false;
    
    (*num_jobs)++;

    printf("[%d] %d\n", jobs[slot].job_number, jobs[slot].pid);
}

void check_jobs(job_t *jobs, int max_jobs, int *num_jobs) {
    int status;
    pid_t pid;

    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].job_number != 0 && !jobs[i].done) {
            pid = waitpid(jobs[i].pid, &status, WNOHANG);
            if (pid == -1) {
                perror("waitpid error");
                continue;
            }
            if (pid > 0) {
                printf("\n[%d] + done\n", jobs[i].job_number);

                jobs[i].command = NULL;
                jobs[i].job_number = 0;
                jobs[i].pid = 0;
                jobs[i].done = false;
                (*num_jobs)--;
            } else {
                printf("[%d][%d] %s\n", jobs[i].job_number, jobs[i].pid, jobs[i].command);
            }
        }
    }
}
