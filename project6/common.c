#include "common.h"

struct timeval start_time;

void init_start_time(void) {
    gettimeofday(&start_time, NULL);
}

void get_elapsed_time(char *time_str) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    
    long sec = current_time.tv_sec - start_time.tv_sec;
    long usec = current_time.tv_usec - start_time.tv_usec;
    
    if (usec < 0) {
        sec--;
        usec += 1000000;
    }
    
    // Format as M:SS.mmm (minutes:seconds.milliseconds)
    int total_ms = (sec * 1000) + (usec / 1000);
    int minutes = total_ms / 60000;
    int seconds = (total_ms % 60000) / 1000;
    int milliseconds = total_ms % 1000;
    
    sprintf(time_str, "%d:%02d.%03d", minutes, seconds, milliseconds);
}
