#include "schedulers.h"

int sjf(Process **process_ptr) {
  Process *procs = *process_ptr;

  // Optional: sort by arrival for deterministic behavior (like srt does)
  qsort(procs, PROCESS_COUNT, sizeof(Process), compare_by_arrival_time);

  // --- same variables/flow as your original SJF ---
  int completed[PROCESS_COUNT];
  memset(completed, 0, sizeof(completed));
  int waiting[PROCESS_COUNT], turnaround[PROCESS_COUNT];
  int cur_time = (int)START_TIME, completed_count = 0;
  float total_waiting = 0, total_turnaround = 0;

  // (Optional) mirror your prints
  fprintf(stdout, "SJF\n******************\nSimulated processes BEFORE execution:\n");
  for (int i = 0; i < PROCESS_COUNT; i++) print_process(&procs[i]);
  fprintf(stdout, "\n******************\nTime chart:\n");

  while (completed_count < PROCESS_COUNT) {
    int proc = -1;
    int min_service = INT_MAX;

    // choose among ARRIVED and NOT COMPLETED, the one with MIN burst_time
    for (int i = 0; i < PROCESS_COUNT; i++) {
      if (!completed[i] && procs[i].arrival_time <= cur_time) {
        if ((int)procs[i].burst_time < min_service) {
          min_service = (int)procs[i].burst_time;
          proc = i;
        }
      }
    }

    if (proc == -1) {
      cur_time++;            // idle: advance time like your original code
      continue;
    }

    // record first start_time if not set (for response/waiting metrics)
    if (procs[proc].start_time < 0) procs[proc].start_time = cur_time;

    waiting[proc]    = cur_time - (int)procs[proc].arrival_time;
    turnaround[proc] = waiting[proc] + (int)procs[proc].burst_time;
    total_waiting   += waiting[proc];
    total_turnaround+= turnaround[proc];

    // run to completion (non-preemptive)
    fprintf(stdout, "%s ", procs[proc].name);
    cur_time += (int)procs[proc].burst_time;

    procs[proc].remaining_time  = 0.0;
    procs[proc].completion_time = cur_time;
    procs[proc].is_completed    = true;

    completed[proc] = 1;
    completed_count++;

    // same per-process line you printed before
    printf("process number %d - arrival time: %d, service time: %d, priority: %d, "
           "waiting time: %d, turnaround time: %d\n",
           proc,
           (int)procs[proc].arrival_time,
           (int)procs[proc].burst_time,
           procs[proc].priority,
           waiting[proc],
           turnaround[proc]);
  }

  // same averages/throughput calculation
  float min_arrival = (float)procs[0].arrival_time;
  int   max_finish  = (int)procs[0].completion_time;
  for (int i = 0; i < PROCESS_COUNT; i++) {
    if (procs[i].arrival_time < min_arrival)   min_arrival = (float)procs[i].arrival_time;
    if ((int)procs[i].completion_time > max_finish) max_finish = (int)procs[i].completion_time;
  }
  float total_time   = (float)max_finish - min_arrival;
  if (total_time <= 0) total_time = 1.0f; // avoid div-by-zero in degenerate cases
  float throughput   = (float)PROCESS_COUNT / total_time;
  float avg_waiting  = total_waiting    / (float)PROCESS_COUNT;
  float avg_turnaround = total_turnaround / (float)PROCESS_COUNT;

  printf("average turnaround time: %.2f\n", avg_turnaround);
  printf("average waiting/response time: %.2f\n", avg_waiting);
  printf("total throughput: %.2f processes per quantum\n", throughput);
  printf("\n");

  // (Optional) AFTER snapshot like your other schedulers
  fprintf(stdout, "******************\nSimulated processes AFTER execution:\n");
  for (int i = 0; i < PROCESS_COUNT; i++) print_process(&procs[i]);

  return 0;
}
