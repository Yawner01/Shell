#pragma once

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tokenlist **parse_pipes(tokenlist *tokens, int *num_cmds);
void execute_commands(tokenlist **commands, int num_cmds);