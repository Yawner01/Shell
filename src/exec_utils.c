#include "lexer.h"
#include "env_utils.h"
#include "exec_utils.h"
#include "job_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PERMISSIONS (S_IRUSR | S_IWUSR)

tokenlist **parse_pipes(tokenlist *tokens, int *num_cmds) {
    tokenlist **commands = malloc(8 * sizeof(tokenlist*));
    int cmd_count = 0;
    int token_idx = 0;

    tokenlist *current_cmd = new_tokenlist();

    while(tokens->items[token_idx] != NULL) {
        if (strcmp(tokens->items[token_idx], "|") == 0) {
            commands[cmd_count] = current_cmd;
            cmd_count++;

            current_cmd = new_tokenlist();
        } else if (strcmp(tokens->items[token_idx], "&") == 0) {
            current_cmd->background = true;
        } 
        else {
            add_token(current_cmd, tokens->items[token_idx]);
        }
        token_idx++;
    }

    commands[cmd_count] = current_cmd;
    cmd_count++;

    *num_cmds = cmd_count;
    return commands;
}

void execute_commands(tokenlist **commands, int num_cmds, tokenlist* command_history, job_t* jobs, int* num_jobs, int* next_job_number) {
    if (num_cmds == 1) {
        execute_single_command(commands, num_cmds, command_history, jobs, num_jobs, next_job_number);
        return;
    }

    int pipefds[2 * (num_cmds - 1)];

    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe failed");
            exit(1);
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork failed\n"); //verify its not negative
            exit(1);
        } else if (pid == 0) {
            if (i > 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }

            if (i < num_cmds - 1) {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }

            for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                close(pipefds[j]);
            }

            char *full_path = find_path(commands[i]);
            
            execv(full_path, commands[i]->items);
            perror("execv failed");
            free(full_path);
            exit(1);
        }
    }

    for (int i = 0; i < 2 * (num_cmds - 1); i++) {
        close(pipefds[i]);
    }

    for (int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }
}

void execute_single_command(tokenlist **commands, int num_cmds, tokenlist* command_history, job_t* jobs, int* num_jobs, int* next_job_number) {
    // check built-in commands
    if (strcmp(commands[0]->items[0], "cd") == 0) {
        cmd_cd(commands[0]);
        return;
    } else if (strcmp(commands[0]->items[0], "jobs") == 0) {
        cmd_jobs(jobs, num_jobs);
        return;
    } else if (strcmp(commands[0]->items[0], "exit") == 0) {
        cmd_exit(command_history);
        return;
    }

    int input_fd = -1, output_fd = -1;
    char *input_file = NULL;
    char *output_file = NULL;

    for (int i = 0; i < commands[0]->size; ++i) {
        if(strcmp(commands[0]->items[i], ">") == 0 && i + 1 < commands[0]->size) {
            output_file = commands[0]->items[i + 1];
            commands[0]->items[i] = NULL;
        } else if (strcmp(commands[0]->items[i], "<") == 0 && i + 1 < commands[0]->size) {
            input_file = commands[0]->items[i + 1];
            commands[0]->items[i] = NULL;
        }
    }  

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork failed\n"); //verify its not negative
        exit(1);
    } else if (pid == 0) { //child process
        // Input redirection
        if (input_file != NULL) {
            input_fd = open(input_file, O_RDONLY);
            if (input_fd == -1) {
                perror("Error opening input file");
                exit(1);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        //Output Redirection
        if (output_file != NULL) {
            output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, PERMISSIONS);
            if (output_fd == -1) {
                perror("Error opening output file");
                exit(1);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        char *full_path = find_path(commands[0]);

        commands[0]->items[0] = full_path;
        execv(full_path, commands[0]->items);

        perror("execv failed");
        exit(1);
    } else { //parent process
        if (commands[0]->background) {
            add_job(jobs, MAX_JOBS, num_jobs, pid, next_job_number);
        } else {
            int status;
            wait(&status);
            if (WIFEXITED(status)) {
                printf("child exited with status: %d\n", WEXITSTATUS(status));
                return;
            } else {
                printf("child did not exit right\n");
            }
        }
        return;
    }
}