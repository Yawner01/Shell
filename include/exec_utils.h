#pragma once

#include "lexer.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tokenlist **parse_pipes(tokenlist *tokens, int *num_cmds);
void execute_commands(tokenlist **commands, 
                      int num_cmds, 
                      tokenlist* command_history, 
                      job_t* jobs, 
                      int* num_jobs, 
                      int* next_job_number);
void execute_single_command(tokenlist **commands, 
                            int num_cmds, 
                            tokenlist* command_history, 
                            job_t* jobs, 
                            int* num_jobs, 
                            int* next_job_number);