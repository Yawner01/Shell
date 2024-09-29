#include "lexer.h"
#include "pipeline.h"
#include "commands.h"
#include "env_utils.h"
#include "exec_utils.h"
#include "job_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

	tokenlist* command_history = new_tokenlist();

	int num_jobs = 0;

	job_t jobs[10];
	for (int i = 0; i < 10; i++) {
		jobs[i].job_number = 0;
		jobs[i].pid = 0;
		jobs[i].command = NULL;
		jobs[i].done = false;
	}

	int next_job_number = 1;

	fd_set readfds;
	struct timeval timeout;
	bool show_prompt = true;

	while (1) {

		int process_finished = check_jobs(jobs, 10, &num_jobs);

		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);

		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		char *user = getenv("USER");
		char *machine = getenv("MACHINE");
		char *pwd = getenv("PWD");

		if (process_finished == 1 || show_prompt) {
			show_prompt = false;
			printf("%s@%s:%s> ", user, machine, pwd);
			fflush(stdout);
		}

		int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

		if (ret == -1) {
			perror("select");
			break;
		}

		if (ret == 0) {
			continue;
		}

		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			show_prompt = true;
			/* input contains the whole command
			* tokens contains substrings from input split by spaces
			*/

			check_jobs(jobs, 10, &num_jobs);

			char *input = get_input();
			//printf("whole input: %s\n", input);

			tokenlist *tokens = get_tokens(input);
			for (int i = 0; i < tokens->size; i++) {
				if (tokens->items[i][0] == '$') {
					get_env_variable(tokens->items[i] + 1, &tokens->items[i]);
				}
				if (tokens->items[i][0] == '~') {
					replace_tilde(&tokens->items[i]);
				}
				//printf("token %d: (%s)\n", i, tokens->items[i]);
			}
			
			int num_cmds;
			tokenlist **commands = parse_pipes(tokens, &num_cmds);

			// insert valid commands in command history
			for (int i = 0; i < num_cmds; ++i) {
				char* cmd = commands[i]->items[0];
				if (is_valid_command(cmd)) {
					add_to_history(command_history, cmd);
				}
			}

			execute_commands(commands, num_cmds, command_history, jobs, &num_jobs, &next_job_number);

			free(input);
			free_tokens(tokens);
		}

		check_jobs(jobs, 10, &num_jobs);

	}

	free_tokens(command_history);

	return 0;
}

char *get_input(void) {
	char *buffer = NULL;
	int bufsize = 0;
	char line[5];
	while (fgets(line, 5, stdin) != NULL)
	{
		int addby = 0;
		char *newln = strchr(line, '\n');
		if (newln != NULL)
			addby = newln - line;
		else
			addby = 5 - 1;
		buffer = (char *)realloc(buffer, bufsize + addby);
		memcpy(&buffer[bufsize], line, addby);
		bufsize += addby;
		if (newln != NULL)
			break;
	}
	buffer = (char *)realloc(buffer, bufsize + 1);
	buffer[bufsize] = 0;
	return buffer;
}

tokenlist *new_tokenlist(void) {
	tokenlist *tokens = (tokenlist *)malloc(sizeof(tokenlist));
	tokens->size = 0;
	tokens->background = false;
	tokens->items = (char **)malloc(sizeof(char *));
	tokens->items[0] = NULL; /* make NULL terminated */
	return tokens;
}

void add_token(tokenlist *tokens, char *item) {
	int i = tokens->size;

	tokens->items = (char **)realloc(tokens->items, (i + 2) * sizeof(char *));
	tokens->items[i] = (char *)malloc(strlen(item) + 1);
	tokens->items[i + 1] = NULL;
	strcpy(tokens->items[i], item);

	tokens->size += 1;
}

void print_tokens(const tokenlist *tokens) {
	if (tokens == NULL) {
		printf("NULL");
		return;
	}

	for (size_t i = 0; i < tokens->size - 1; ++i) {
		printf("%s, ", tokens->items[i]);
	}

	printf("%s", tokens->items[tokens->size - 1]);
}

tokenlist *get_tokens(char *input) {
	char *buf = (char *)malloc(strlen(input) + 1);
	strcpy(buf, input);
	tokenlist *tokens = new_tokenlist();
	char *tok = strtok(buf, " ");
	while (tok != NULL)
	{
		add_token(tokens, tok);
		tok = strtok(NULL, " ");
	}
	free(buf);
	return tokens;
}

void free_tokens(tokenlist *tokens) {
	if (tokens == NULL) {
		return;
	}

	for (int i = 0; i < tokens->size; i++)
		free(tokens->items[i]);
	free(tokens->items);
	free(tokens);

	tokens = NULL;
}
