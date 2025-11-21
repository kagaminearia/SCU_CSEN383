#include "child_interactive.h"
#include "common.h"

void child_interactive_process(int child_num, int write_fd) {
    int message_count = 1;
    struct timeval child_start;
    fd_set readfds;
    struct timeval timeout;

    gettimeofday(&child_start, NULL);
  
    while (1) {
        struct timeval current;
        gettimeofday(&current, NULL);
        long elapsed = current.tv_sec - child_start.tv_sec;
        
        if (elapsed >= LIFETIME) {
            break;
        }
        
        // Prompt for input
        printf("Child %d: Enter message: ", child_num);
        fflush(stdout);

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
        
        // If input
        if (ret > 0) {
            char input[128];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = 0;
                
                char time_str[64];
                get_elapsed_time(time_str);
                
                char message[BUFFER_SIZE];
                snprintf(message, BUFFER_SIZE, "%s: Child %d: %s (message %d from terminal)\n", 
                         time_str, child_num, input, message_count);
                
                write(write_fd, message, strlen(message));
                
                message_count++;
            }
        }
    }
    
    // Close pipe and exit
    close(write_fd);
    exit(0);
}
