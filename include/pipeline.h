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

pipe_t* new_pipe(tokenlist* tokens);
void    free_pipe(pipe_t* pipe);

pipeline_t* new_pipeline();
void        free_pipeline(pipeline_t* pipeline);
void        pop_pipe(pipeline_t* pipeline);
void        push_pipe(pipeline_t* pipeline, pipe_t* pipe);
pipe_t*     pipeline_front(pipeline_t* pipeline);
bool        pipeline_empty(pipeline_t* pipeline);

// pipeline test function
void test_pipeline();

#endif