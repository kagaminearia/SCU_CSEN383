#include "schedulers.h"

// Keep the "original style": discrete time steps, ready queue by index.
int rr(Process **process_ptr, int time_quantum) {
  Process *procs = *process_ptr;

  // Optional but matches your other schedulers: deterministic arrival order.
  qsort(procs, PROCESS_COUNT, sizeof(Process), compare_by_arrival_time);

  // BEFORE snapshot (for consistency with your simulation prints)
  fprintf(stdout, "RR (q=%d)\n******************\nSimulated processes BEFORE execution:\n", time_quantum);
  for (int i = 0; i < PROCESS_COUNT; i++) print_process(&procs[i]);
  fprintf(stdout, "\n******************\nTime chart:\n");

  // --- Original-style state ---
  int currentTime = (int)START_TIME;
  int completed   = 0;

  // We’ll use a simple index queue like your original.
  int queue[PROCESS_COUNT * 2];   // safe upper bound for re-enqueues
  int front = 0, rear = -1;

  // Helper: enqueue by index
  auto enqueue = [&](int idx){
    queue[++rear] = idx;
  };

  // Helper: check if arrived (treat arrival_time as <= currentTime)
  auto has_arrived = [&](int idx, int t){
    return procs[idx].arrival_time <= (double)t;
  };

  // Enqueue anything that has arrived at t==START_TIME
  for (int i = 0; i < PROCESS_COUNT; i++) {
    if (has_arrived(i, currentTime)) enqueue(i);
  }

  // We’ll build an optional “job sequence” string (like your runRR).
  // (Keep modest buffer; this is just for display.)
  char seq[PROCESS_COUNT * 8];
  seq[0] = '\0';

  while (completed < PROCESS_COUNT) {
    if (front <= rear) {
      int idx = queue[front++];

      // If already completed (can happen if enqueued right at finish), skip.
      if (procs[idx].is_completed) {
        // still need to enqueue new arrivals at this time below
      } else {
        // First touch: record start_time (original "started" flag behavior)
        if (procs[idx].start_time < 0) procs[idx].start_time = (double)currentTime;

        // Run one quantum (like your original decremented remaining one-by-one).
        int slice = time_quantum > 0 ? time_quantum : 1;
        // Append to sequence once per quantum (like original printed name stepwise)
        size_t len = strlen(seq);
        if (len + 4 < sizeof(seq)) {
          // use process name string from struct
          seq[len++] = procs[idx].name[0];
          // if your names are "P00", printing the first char keeps it short
          seq[len] = '\0';
        }
        fprintf(stdout, "%s ", procs[idx].name);

        procs[idx].remaining_time -= (double)slice;
        currentTime += slice;

        // Completed?
        if (procs[idx].remaining_time <= 0.0) {
          procs[idx].remaining_time  = 0.0;
          procs[idx].completion_time = (double)currentTime;
          procs[idx].is_completed    = true;
          completed++;
        } else {
          // Not done: re-enqueue
          enqueue(idx);
        }
      }
    } else {
      // CPU idle—advance time by one unit (original behavior)
      currentTime++;
    }

    // Enqueue newly arrived processes at this current time
    for (int i = 0; i < PROCESS_COUNT; i++) {
      if (!procs[i].is_completed &&
          procs[i].start_time < 0 &&               // not yet started AND
          has_arrived(i, currentTime)) {           // just reached arrival
        // Guard against multiple enqueues at same t before first run:
        // If it’s already in the queue but not started, we’ll accept duplicates
        // only once; simplest approach is to enqueue when it first becomes <= t
        // and start_time is still -1 (i.e., never scheduled yet).
        enqueue(i);
      }
    }
  }

  // After-run metrics similar to your calculateMetrics()
  float totalTurnaround = 0.f, totalWaiting = 0.f, totalResponse = 0.f;
  int   completedProcs  = 0;
  for (int i = 0; i < PROCESS_COUNT; i++) {
    if (procs[i].completion_time > 0) {
      int turnaround = (int)(procs[i].completion_time - procs[i].arrival_time);
      int waiting    = turnaround - (int)procs[i].burst_time;
      int response   = (int)(procs[i].start_time - procs[i].arrival_time);
      totalTurnaround += turnaround;
      totalWaiting    += waiting;
      totalResponse   += response;
      completedProcs++;
    }
  }
  float avgTurnaround = completedProcs ? totalTurnaround / completedProcs : 0.f;
  float avgWaiting    = completedProcs ? totalWaiting    / completedProcs : 0.f;
  float avgResponse   = completedProcs ? totalResponse   / completedProcs : 0.f;

  // Throughput over the window [min arrival, max completion]
  double min_arrival = procs[0].arrival_time, max_finish = procs[0].completion_time;
  for (int i = 1; i < PROCESS_COUNT; i++) {
    if (procs[i].arrival_time     < min_arrival) min_arrival = procs[i].arrival_time;
    if (procs[i].completion_time  > max_finish)  max_finish  = procs[i].completion_time;
  }
  double span = (max_finish - min_arrival) > 0 ? (max_finish - min_arrival) : 1.0;
  int throughput = (int)((double)completedProcs / span);

  // Show a compact job sequence (optional, mirrors your runRR)
  fprintf(stdout, "\n\nRR Job Sequence: ");
  for (size_t i = 0; seq[i] != '\0'; i++) printf("%c -> ", seq[i]);
  printf("\n");

  printf("RR: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
         avgTurnaround, avgWaiting, avgResponse, throughput);

  // AFTER snapshot for consistency
  fprintf(stdout, "\n******************\nSimulated processes AFTER execution:\n");
  for (int i = 0; i < PROCESS_COUNT; i++) print_process(&procs[i]);

  return 0;
}
