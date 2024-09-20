#pragma once

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_env_variable(char *variable, char ** token);
void replace_tilde(char ** token);
char *find_path(tokenlist *tokens);
char *my_strdup(const char *src);