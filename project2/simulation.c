#include "simulation.h"
#include "hpf.h"
#include "srt.h"
#include "rr.h"
#include "sjf.h"
#include "fcfs.h"
#include "util.h"

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
  snprintf(name, name_size, "P%d", idx);
}

int generate_arrival_time(void) {
  return rand() % (MAX_ARRIVIAL_TIME + 1);
}

int generate_burst_time(void) {
  return rand() % (MAX_BURST_TIME - MIN_BURST_TIME + 1) + MIN_BURST_TIME;
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

  // Generate processes
  for (int i = 0; i < PROCESS_COUNT; i++) {
    generate_name(i, processes[i].name, NAME_LENGTH);
    processes[i].arrival_time = generate_arrival_time();
    processes[i].burst_time = generate_burst_time();
    processes[i].remaining_time = processes[i].burst_time;
    processes[i].priority = generate_priority();
    processes[i].original_priority = processes[i].priority;

    // Set explicit default values for start_time and completion_time
    processes[i].start_time = -1;
    processes[i].completion_time = -1;

    processes[i].is_completed = false;
  }

  return processes;
}

#ifdef SIMULATION
int main() {
    // Generate random processes
    Process* processes;
    char output_filename[20];

    srand(time(NULL));
    for (int i = 1; i <= 5; i++) {
        processes = generate_processes();
        snprintf(output_filename, sizeof(output_filename), "output%d.txt", i);

        // Clear the file at the beginning of each simulation run
        FILE *fp = fopen(output_filename, "w");
        if (fp) {
            fclose(fp);
        }

        fcfs(&processes, output_filename);
        reset_processes(processes, PROCESS_COUNT);
        sjf(&processes, output_filename);
        reset_processes(processes, PROCESS_COUNT);
        srt(&processes, output_filename);
        reset_processes(processes, PROCESS_COUNT);
        rr(&processes, output_filename);
        reset_processes(processes, PROCESS_COUNT);
        hpf_np(&processes, output_filename);
        reset_processes(processes, PROCESS_COUNT);
        hpf_p(&processes, output_filename);
        reset_processes(processes, PROCESS_COUNT);
        
        

        if (i < 5) {
            reset_processes(processes, PROCESS_COUNT);
        }
        free(processes);
    }

    return 0;
}
#endif  // SIMULATION
