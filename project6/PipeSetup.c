#include "PipeSetup.h"
#include "child1to4.h"
#include "child_interactive.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

// Setup pipes and fork children
int setup_pipes_and_children(int pipes[][2], pid_t pids[]) {
    // Create all pipes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return -1;
        }
    }

    // Fork 5 children
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            return -1;
        }

        if (pids[i] == 0) {
            // CHILD PROCESS

            // Close unused read ends
            for (int j = 0; j < NUM_CHILDREN; j++) {
                close(pipes[j][0]);      // children never read
                if (j != i) close(pipes[j][1]); // close write ends of other children
            }

            int write_fd = pipes[i][1];

            // --- CHILD WORK GOES HERE ---
            // write() to write_fd
            // generate timestamps
            // sleep random intervals
            // child 5 reads stdin and writes to write_fd
            if (i < 4) {
                // Children 1-4: Auto-generate messages
                run_child_1_to_4(i + 1, write_fd);
            } else {
                // Child 5: Interactive input
                child_interactive_process(5, write_fd);
            }
            // -------------------------------------------

            exit(0);
        }
    }

    // PARENT PROCESS

    // Parent only reads from pipes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        close(pipes[i][1]); // close write ends
    }

    return 0;
}

// Get read file descriptors for parent to use with select()
void get_read_fds(int pipes[][2], int read_fds[]) {
    for (int i = 0; i < NUM_CHILDREN; i++) {
        read_fds[i] = pipes[i][0]; // parent reads these using select()
    }
}
