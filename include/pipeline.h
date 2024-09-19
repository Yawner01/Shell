#ifndef PIPELINE_H
#define PIPELINE_H

#include "lexer.h"
#include <stdint.h>

// Component for pipeline_t struct.
typedef struct pipe_t {
    int             fd[2]; // read/write file descriptors, fd[0] = read, fd[1] = write
    tokenlist*      tokens;
    struct pipe_t*  next; // pointer to next pipe in a pipeline
} pipe_t; 

// Queue container for pipe_t structs.
typedef struct {
    pipe_t* start;
    pipe_t* end;
} pipeline_t;

pipe_t* new_pipe(tokenlist* tokens);                        // Returns a newly allocated pipe_t struct. The created pipe shallow copies the tokenlist parameter.
void    free_pipe(pipe_t* pipe);                            // Deallocates the pipe and any allocated data inside.
void    execute_pipe_commands(pipe_t* pip);

pipeline_t* new_pipeline();                                 // Returns a newly allocated pipeline_t struct.
void        free_pipeline(pipeline_t* pipeline);            // Deallocates the pipeline and any allocated data inside.
void        pop_pipe(pipeline_t* pipeline);                 // Removes and deallocates the first pipe in the pipeline.
void        push_pipe(pipeline_t* pipeline, pipe_t* pipe);  // Inserts to the end of the pipeline.
pipe_t*     pipeline_front(pipeline_t* pipeline);
bool        pipeline_empty(pipeline_t* pipeline);
void        execute_pipeline_commands(pipeline_t* pipeline); // Executes the commands of, and pops, every pipe in the pipeline.

// pipeline test function
void test_pipeline();

#endif