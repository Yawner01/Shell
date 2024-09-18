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
        fprintf(stderr, "[ERROR] failed to create pipe\n");
        exit(1);   
    }

    // create read and write end of pipe
    int pipe_result = 0; //pipe(new_pipe->fd); TODO: uncomment and replace '= 0;' with this line

    // validate pipe creation
    // result < 0: creation failed
    // result = 0: returned to newly created child process
    // result > 0: returned to parent or caller and the value contains the
	// process ID of the newly created child process
    if (pipe_result < 0) {
        fprintf(stderr, "[ERROR] failed pipe file descriptors\n");
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

// Removes the first pipe in the pipeline.
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

	// destroy pipeline when no longer needed
	free_pipeline(pipeline);
}
