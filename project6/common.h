#ifndef COMMON_H
#define COMMON_H

#include <sys/time.h>
#include <stdio.h>

#define NUM_CHILDREN 5
#define LIFETIME 30
#define BUFFER_SIZE 256

extern struct timeval start_time;

void get_elapsed_time(char *time_str);

void init_start_time(void);

#endif // COMMON_H
