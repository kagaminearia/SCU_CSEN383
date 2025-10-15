#include "util.h"

int compare_by_remaining_time(const void* a, const void* b);
int srt(Process** process_ptr);
int sjf(Process **process_ptr);
int rr(Process **process_ptr, int time_quantum);