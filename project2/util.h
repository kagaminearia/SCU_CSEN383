#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include "process.h"

// Function declarations
int compare_by_arrival_time(const void* a, const void* b);
int compare_by_burst_time(const void* a, const void* b);
int compare_by_priority(const void* a, const void* b);
int compare_by_remaining_time(const void* a, const void* b);

void print_process(const Process *p);

#endif // UTIL_H
