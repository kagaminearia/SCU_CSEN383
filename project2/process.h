#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"

// Process struct definition
typedef struct Process {
  char name[4];
  double arrival_time;
  double burst_time;
  double remaining_time;
  int priority;
  double start_time;
  double completion_time;
  bool is_completed;
} Process;

#endif // PROCESS_H
