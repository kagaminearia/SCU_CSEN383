#ifndef SIMULATION_H
#define SIMULATION_H

#include <string.h>
#include <time.h>

#include "process.h"
#include "util.h"
#include "srt.h"

void generate_name(int idx, char *name, int name_size);
double generate_arrival_time(void);
double generate_burst_time(void);
int generate_priority(void);
Process* generate_processes(void);

#endif // SIMULATION_H
