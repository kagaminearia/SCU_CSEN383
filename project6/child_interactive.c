#include "child_interactive.h"
#include "common.h"

void child_interactive_process(int child_num, int write_fd) {
    int message_count = 1;
    struct timeval child_start;
    fd_set readfds;
    struct timeval timeout;
    int prompt_printed = 0;

    gettimeofday(&child_start, NULL);
  
    while (1) {
        struct timeval current;
        gettimeofday(&current, NULL);
        long elapsed = current.tv_sec - child_start.tv_sec;
        
        if (elapsed >= LIFETIME) {
            break;
        }
        
        // Only print prompt if we haven't printed it yet (waiting for new input)
        if (!prompt_printed) {
            printf("Child %d: Enter message: ", child_num);
            fflush(stdout);
            prompt_printed = 1;
        }

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        // Calculate remaining time
        long remaining = LIFETIME - elapsed;
        if (remaining <= 0) {
            break;
        }
        
        timeout.tv_sec = (remaining < 1) ? remaining : 1;
        timeout.tv_usec = 0;
        
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
        
        // Check time again after select
        gettimeofday(&current, NULL);
        elapsed = current.tv_sec - child_start.tv_sec;
        if (elapsed >= LIFETIME) {
            break;
        }
        
        // If input available
        if (ret > 0) {
            char input[128];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = 0;
                
                // Check time one more time before processing
                gettimeofday(&current, NULL);
                elapsed = current.tv_sec - child_start.tv_sec;
                if (elapsed >= LIFETIME) {
                    break;
                }
                
                char time_str[64];
                get_elapsed_time(time_str);
                
                char message[BUFFER_SIZE];
                snprintf(message, BUFFER_SIZE, "%s: Child %d: %s (message %d from terminal)\n", 
                         time_str, child_num, input, message_count);
                
                write(write_fd, message, strlen(message));
                
                message_count++;
                prompt_printed = 0;  // Reset so we print prompt again for next input
            } else {
                // EOF or error, reset prompt
                prompt_printed = 0;
            }
        }
        // If timeout (ret == 0), loop continues without re-printing prompt
    }
    
    // Close pipe and exit
    close(write_fd);
    exit(0);
}
