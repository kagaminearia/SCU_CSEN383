// Shortest Job First (Non-preemptive)

#include "sjf.h"

int sjf(Process** process_ptr, const char* output_filename) {
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

  fprintf(output_file, "SJF\n******************\nSimulated processes BEFORE execution:\n");
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
      if (p->is_completed) continue;
      if (p->arrival_time <= current_time && current_time < END_TIME) {
        ready_processes[ready_count++] = p;
      }
    }

    // If no processes are ready, increment time
    if (ready_count == 0) {
      current_time += TIME_QUANTUM;
      continue;
    }

    // Sort ready processes by their full burst time (service_time)
    qsort(ready_processes, ready_count, sizeof(Process*), compare_by_burst_time);

    // Choose the shortest job among ready ones
    Process* shortest_job = ready_processes[0];

    // If it hasn’t started yet, mark start time
    if (shortest_job->start_time == -1)
      shortest_job->start_time = current_time;

    // Execute it non-preemptively: run until finished
    current_time += shortest_job->burst_time;
    fprintf(output_file, "%s ", shortest_job->name);

    // Mark as completed
    shortest_job->completion_time = current_time;
    shortest_job->remaining_time = 0.0;
    shortest_job->is_completed = true;
    process_completed++;

    // Stop when all are done or time limit reached
    if (process_completed == PROCESS_COUNT || current_time >= END_TIME)
      completed = true;
  }

  // Print after execution
  fprintf(output_file, "\n******************\nSimulated processes AFTER execution:\n");
  for (int i = 0; i < PROCESS_COUNT; i++) {
    print_process_to_file(&processes[i], output_file);
  }
  compute_and_print_statistics_to_file(processes, PROCESS_COUNT, output_file);
  fclose(output_file);
  return 0;
}
