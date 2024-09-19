#include "pipeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// pipe_t functions
//---------------

// Returns a newly allocated pipe_t struct. 
// The created pipe shallow copies the tokenlist parameter.
pipe_t* new_pipe(tokenlist* tokens) {
    
    // allocate pipe_t struct
    pipe_t* new_pipe = (pipe_t*)malloc(sizeof(pipe_t));

    // validate pipe_t struct
    if (new_pipe == NULL) {
        fprintf(stderr, "[ERROR] failed to create pipe struct\n");
        exit(1);   
    }

    new_pipe->tokens = tokens;
    new_pipe->next = NULL;

    return new_pipe;
}

// Deallocates the pipe and any allocated data inside.
void free_pipe(pipe_t* pipe) {
    
    if (pipe == NULL) {
        return;
    }
    
    free_tokens(pipe->tokens);
    free(pipe);
    pipe = NULL;
}

void execute_pipe_commands(pipe_t* pip) {

    // validate pipe
    if (pip == NULL) {
        printf("[ERROR] attempted to execute commands on null pipe, ignoring\n");
        return;
    }

    // keeps track of input fd for next command in pipeline
    int in_fd = 0;

    // access and execute the commands stored in the pipe struct's tokens
    for (int i = 0; i < pip->tokens->size; ++i) {

        // create read and write end of pipe
        int pipe_result = pipe(pip->fd);

        // check for errors after calling pipe()
        if (pipe_result == -1) {
            fprintf(stderr, "[ERROR] failed to create read/write ends of pipe\n");
            exit(1);
        }

        // create child process
        pid_t pid = fork();

        // check for errors in process creation
        if (pid == -1) {
            fprintf(stderr, "[ERROR] failed to create child process\n");
            exit(1);   
        }

        // if child process created successfully
        if (pid == 0) {

            // redirect input
            if (in_fd != 0) {
                dup2(in_fd, 0);
                close(in_fd);
            }

            // redirect output
            if (pip->next != NULL) {
                dup2(pip->fd[1], 1);
                close(pip->fd[1]);
            }

            // close read end of pipe
            close(pip->fd[0]);

            // execute the command
            execv(pip->tokens->items[i], pip->tokens->items);
        }
        // otherwise parent process
        else {

            // wait for child process to finish
            wait(NULL);
            // close write end of pipe
            close(pip->fd[1]);
            // save input for next command
            in_fd = pip->fd[0];
        }
    }
}

// pipeline_t functions
//-------------------

// Returns a newly allocated pipeline_t struct.
pipeline_t* new_pipeline() {
    
    // allocate pipeline and head
    pipeline_t* new_pipeline = (pipeline_t*)malloc(sizeof(pipeline_t));
    new_pipeline->start = (pipe_t*)malloc(sizeof(pipe_t)); // create a dummy pipe with malloc instead of create_pipe()
    new_pipeline->start->next = NULL;
    new_pipeline->end = new_pipeline->start;

    if (new_pipeline == NULL) {
        fprintf(stderr, "[ERROR] failed to create pipeline\n");
        exit(1);   
    }

    return new_pipeline;
}

// Deallocates the pipeline and any allocated data inside.
void free_pipeline(pipeline_t* pipeline) {
    
    // validate pipeline
    if (pipeline == NULL) {
        return;
    }

    // free any allocated data in pipeline
    while (!pipeline_empty(pipeline)) {
        pop_pipe(pipeline);
    }

    // free pipeline
    free_pipe(pipeline->start);
    free(pipeline);
    pipeline = NULL;
}

// Removes and deallocates the first pipe in the pipeline.
void pop_pipe(pipeline_t* pipeline)
{
    // validate pipeline
    if (pipeline == NULL) {
        printf("[ERROR] attempting to pop back pipe from null pipeline, ignoring\n");
        return;   
    }

    // check if pipeline empty
    if (pipeline_empty(pipeline)) {
        printf("[ERROR] attempting to pop back pipe from empty pipeline, ignoring\n");
        return;
    }

    // remove front pipe and swap pointers
    pipe_t* dead_pipe = pipeline->start->next;
    pipeline->start->next = dead_pipe->next;

    // reset end pointer if pipeline empty
    if (pipeline_empty(pipeline)) {
        pipeline->end = pipeline->start;
    }

    // destroy pipe and decrement pipeline size
    free_pipe(dead_pipe);
}

// Inserts to the end of the pipeline.
void push_pipe(pipeline_t* pipeline, pipe_t* pipe)
{
    // validate pipeline
    if (pipeline == NULL) {
        printf("[ERROR] attempting to push pipe to null pipeline, ignoring\n");
        return;
    }

    // validate pipe
    if (pipe == NULL) {
        printf("[ERROR] attempting to push null pipe to pipeline, ignoring\n");
        return;    
    }

    // add pipe to pipeline and increment pipeline size
    pipeline->end->next = pipe;
    pipeline->end = pipe;
}

pipe_t* pipeline_front(pipeline_t *pipeline)
{
    return pipeline->start->next;
}

bool pipeline_empty(pipeline_t* pipeline) {
    
    if (pipeline == NULL) {
        printf("[ERROR] attempted empty check on null pipeline, returning true\n");
        return true;
    }

    return pipeline->start->next == NULL;
}

// Executes the commands of, and pops, every pipe in the pipeline.
void execute_pipeline_commands(pipeline_t* pipeline) {

    if (pipeline == NULL) {
        printf("[ERROR] attempted to execute pipeline commands on null pipeline, ignoring\n");
        return;
    }

    while (!pipeline_empty(pipeline)) {
        execute_pipe_commands(pipeline_front(pipeline));
        pop_pipe(pipeline);
    }
}

void test_pipeline() {

    // create pipeline
	pipeline_t* pipeline = new_pipeline();
	const int NUM_PIPES = 3;

	// get user input x times for x number of pipes,
	// then push pipes into pipeline
	for (int i = 0; i < NUM_PIPES; ++i) {
		printf("Enter string for pipeline[%d]: ", i);
		char* input = get_input();
		tokenlist* tokens = get_tokens(input);
		pipe_t* pipe = new_pipe(tokens);
		push_pipe(pipeline, pipe);
	}

	// print pipeline contents while popping pipes from pipeline
	printf("\n[PIPELINE QUEUE]\n");

	for (int i = 0; !pipeline_empty(pipeline); ++i) {
		printf("Pipe [%d] ", i);
		print_tokens(pipeline_front(pipeline)->tokens);
		printf("\n");
		pop_pipe(pipeline);
	}

	printf("\nClearing pipeline...\n\n");

	// check if pop_pipe() and pipeline_empty() work
	printf("[PIPELINE QUEUE]\n");

	if (pipeline_empty(pipeline)) {
		printf("empty\n");
	}

	// destroy pipeline and all remaining pipes when no longer needed
	free_pipeline(pipeline);
}
