#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"
#include "utils.h"
#include "PipeSetup.h"
#include "child_monitor.h"

int main() {
    int pipes[NUM_CHILDREN][2];
    pid_t pids[NUM_CHILDREN];

    // Initialize time and open output file
    init_time();
    init_start_time();  // For child processes

    // Setup pipes and fork children (from PipeSetup.c)
    if (setup_pipes_and_children(pipes, pids) == -1) {
        exit(1);
    }

    // Get read file descriptors for parent to use with select()
    int read_fds[NUM_CHILDREN];
    get_read_fds(pipes, read_fds);

    // --- PARENT WORK GOES HERE ---
    // Use select() on read_fds[]
    // Read child messages
    // Prepend parent timestamp
    // Write to output.txt
    monitor_children(read_fds);
    // --------------------------------

    // Wait for all children to finish
    for (int i = 0; i < NUM_CHILDREN; i++) {
        waitpid(pids[i], NULL, 0);
    }

    printf("All children have terminated. Program completed.\n");
    return 0;
}

