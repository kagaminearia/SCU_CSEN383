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
    
    double msec = (double)(usec / 1000.0);
    sprintf(time_str, "%ld:%06.3f", sec, msec);
}
