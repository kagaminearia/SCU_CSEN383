#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"

// Process struct definition
typedef struct Process {
  char name[5];
  int arrival_time;
  int burst_time;
  int remaining_time;
  int priority;
  int original_priority;
  int start_time;
  int completion_time;
  bool is_completed;
} Process;

#endif // PROCESS_H
