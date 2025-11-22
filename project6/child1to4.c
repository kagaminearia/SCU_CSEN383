#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void run_child_1_to_4(int child_id, int write_fd)
{
    struct timeval start, now;
    gettimeofday(&start, NULL);

    int msg_count = 1;

    while (1) {
        gettimeofday(&now, NULL);

        double elapsed =
            (now.tv_sec - start.tv_sec) +
            (now.tv_usec - start.tv_usec) / 1e6;

        if (elapsed >= 30.0)
            break;

        int sleep_time = rand() % 3;  // 0, 1, or 2
        sleep(sleep_time);

        gettimeofday(&now, NULL);

        long sec = now.tv_sec - start.tv_sec;
        long usec = now.tv_usec - start.tv_usec;
        
        if (usec < 0) {
            sec--;
            usec += 1000000;
        }
        
        // Format as M:SS.mmm (minutes:seconds.milliseconds)
        int total_ms = (sec * 1000) + (usec / 1000);
        int minutes = total_ms / 60000;
        int seconds = (total_ms % 60000) / 1000;
        int milliseconds = total_ms % 1000;

        char msg[256];
        snprintf(msg, sizeof(msg),
                 "%d:%02d.%03d: Child %d message %d\n",
                 minutes, seconds, milliseconds, child_id, msg_count++);

        write(write_fd, msg, strlen(msg));
    }

    close(write_fd);
}
