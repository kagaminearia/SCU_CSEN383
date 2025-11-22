#ifndef PIPE_SETUP_H
#define PIPE_SETUP_H

#include <sys/types.h>

#define NUM_CHILDREN 5

// Setup pipes and fork children
// Returns 0 on success, -1 on error
int setup_pipes_and_children(int pipes[][2], pid_t pids[]);

// Get read file descriptors for parent to use with select()
void get_read_fds(int pipes[][2], int read_fds[]);

#endif // PIPE_SETUP_H

