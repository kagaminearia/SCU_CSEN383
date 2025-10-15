#include "simulation.h"

void generate_name(int idx, char *name, int name_size) {
  // Calculate the maximum index based on the name size just in case
  int max_idx = 1;
  for (int i = 0; i < name_size - 1; i++) max_idx *= 10;

  // Check input idx against the maximum index
  if (idx < 0 || idx >= PROCESS_COUNT || idx >= max_idx) {
    fprintf(stderr, "[ERROR] index out of bounds: %d\n", idx);
    exit(EXIT_FAILURE);
  }

  // Generate the actual name
  snprintf(name, name_size, "P%0*d", NAME_LENGTH - 2, idx);
}

double generate_arrival_time(void) {
  return (double)rand() / (double)RAND_MAX * MAX_ARRIVIAL_TIME;
}

double generate_burst_time(void) {
  return (double)rand() / (double)RAND_MAX * (MAX_BURST_TIME - MIN_BURST_TIME) + MIN_BURST_TIME;
}

int generate_priority(void) {
  return rand() % (MIN_PRIORITY - MAX_PRIORITY + 1) + MAX_PRIORITY;
}

Process* generate_processes(void) {
  // Allocate memory for the array of processes
  Process *processes = malloc(PROCESS_COUNT * sizeof(Process));
  if (!processes) {
    fprintf(stderr, "[ERROR] memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  // Generate processes
  for (int i = 0; i < PROCESS_COUNT; i++) {
    generate_name(i, processes[i].name, NAME_LENGTH);
    processes[i].arrival_time = generate_arrival_time();
    processes[i].burst_time = generate_burst_time();
    processes[i].remaining_time = processes[i].burst_time;
    processes[i].priority = generate_priority();

    // Set explicit default values for start_time and completion_time
    processes[i].start_time = -1.0;
    processes[i].completion_time = -1.0;

    processes[i].is_completed = false;
  }

  return processes;
}

#ifdef SIMULATION
int main() {
  Process* processes = generate_processes();
  srt(&processes);

  for (int i = 0; i < PROCESS_COUNT; i++) {
    print_process(&processes[i]);
  }

  free(processes);
  return 0;
}
#endif  // SIMULATION
