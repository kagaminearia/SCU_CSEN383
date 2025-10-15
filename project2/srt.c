// Shortest reaemining time

#include "srt.h"

int srt(Process** process_ptr, const char* output_filename) {
  Process* processes = *process_ptr;

  // Sort processes by arrival time
  qsort(processes, PROCESS_COUNT, sizeof(Process), compare_by_arrival_time);

  FILE *output_file = fopen(output_filename, "a");
  if (output_file == NULL) {
      perror("Error opening output file");
      return 1;
  }

  int current_time = START_TIME;
  bool completed = false;
  int process_completed = 0;

  // Track how many processes have started but not yet completed
  int process_started = 0;

  // Print the results
  fprintf(output_file, "SRT\n******************\nSimulated processes BEFORE execution:\n");
  for (int i = 0; i < PROCESS_COUNT; i++) {
    print_process_to_file(&processes[i], output_file);
  }

  fprintf(output_file, "\n******************\nTime chart:\n");

  while (!completed) {
    // Create a temporary array to hold the processes that have arrived and are not completed
    Process* ready_processes[PROCESS_COUNT];
    int ready_count = 0;

    for (int i = 0; i < PROCESS_COUNT; i++) {
      Process* p = &processes[i];
      // Ignore completed processes
      if (p->is_completed) continue;

      // Include processes that have started
      if (p->start_time != -1) {
        ready_processes[ready_count++] = p;
        continue;
      }

      // Include processes that have arrived and current time is less than END_TIME
      if (p->arrival_time <= current_time && current_time < END_TIME) {
        ready_processes[ready_count++] = p;
        continue;
      }

      // if arrival time is greater than current time, break
      if (p->arrival_time > current_time) {
        break;
      }
    }

    // If no processes are ready, increment the current time and continue
    if (ready_count == 0) {
      current_time += TIME_QUANTUM;
      continue;
    }

    // Sort the ready processes by remaining time
    qsort(ready_processes, ready_count, sizeof(Process*), compare_by_remaining_time);

    // Run the process with the shortest remaining time for one time unit
    Process* shortest_job = ready_processes[0];
    if (shortest_job->start_time == -1) {
      shortest_job->start_time = current_time;

      // Increment the count of started processes if this is the first time the process is starting
      process_started++;
    }
    shortest_job->remaining_time -= TIME_QUANTUM;
    current_time += TIME_QUANTUM;
    fprintf(output_file, "%s ", shortest_job->name);

    // If the process is completed, update its completion time and mark it as completed
    if (shortest_job->remaining_time <= 0.0) {
      shortest_job->completion_time = current_time;
      shortest_job->is_completed = true;
      shortest_job->remaining_time = 0.0;
      process_completed++;

      // Decrement the count of started processes as this process has now completed
      process_started--;
    }

    // Check if all processes are completed
    // Conditions:
    //   1. All processes have completed
    //   2. Current time has reached END_TIME and all processes that have started before END_TIME have completed
    if (process_completed == PROCESS_COUNT || (current_time >= END_TIME && process_started == 0)) {
      completed = true;
    }
  }

  // Print the results after execution
  fprintf(output_file, "\n******************\nSimulated processes AFTER execution:\n");
  for (int i = 0; i < PROCESS_COUNT; i++) {
    print_process_to_file(&processes[i], output_file);
  }

  // Print statistics for SRT
  compute_and_print_statistics_to_file(processes, PROCESS_COUNT, output_file);
  fclose(output_file);

  return 0;
}
