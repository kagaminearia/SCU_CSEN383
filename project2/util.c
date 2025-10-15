#include "util.h"

// Compare by arrival time
int compare_by_arrival_time(const void* a, const void* b) {
    const Process* p1 = (const Process*)a;
    const Process* p2 = (const Process*)b;
    if (p1->arrival_time < p2->arrival_time) return -1;
    if (p1->arrival_time > p2->arrival_time) return 1;
    return 0;
}

// Compare by burst time
int compare_by_burst_time(const void* a, const void* b) {
    const Process* p1 = (const Process*)a;
    const Process* p2 = (const Process*)b;
    if (p1->burst_time < p2->burst_time) return -1;
    if (p1->burst_time > p2->burst_time) return 1;
    return 0;
}

// Compare by priority
int compare_by_priority(const void* a, const void* b) {
    const Process* p1 = (const Process*)a;
    const Process* p2 = (const Process*)b;
    if (p1->priority < p2->priority) return -1;
    if (p1->priority > p2->priority) return 1;
    return 0;
}

// Compare by remaining time
int compare_by_remaining_time(const void* a, const void* b) {
  const Process* p1 = *(const Process**)a;
  const Process* p2 = *(const Process**)b;
  if (p1->remaining_time < p2->remaining_time) return -1;
  if (p1->remaining_time > p2->remaining_time) return 1;
  return 0;
}

void print_process(const struct Process* p) {
  printf("Name: %s, Arrival Time: %d, Burst Time: %d, Remaining Time: %d, Priority: %d, Start Time: %d, Completion Time: %d, Completed: %s\n",
         p->name, p->arrival_time, p->burst_time, p->remaining_time, p->priority, p->start_time, p->completion_time, p->is_completed ? "true" : "false");
}

void print_process_to_file(const struct Process* p, FILE *fp) {
  fprintf(fp, "Name: %s, Arrival Time: %d, Burst Time: %d, Remaining Time: %d, Priority: %d, Start Time: %d, Completion Time: %d, Completed: %s\n",
         p->name, p->arrival_time, p->burst_time, p->remaining_time, p->priority, p->start_time, p->completion_time, p->is_completed ? "true" : "false");
}

// Compute and print statistics for processes
void compute_and_print_statistics(Process *processes, int process_count) {
    compute_and_print_statistics_to_file(processes, process_count, stdout);
}

void compute_and_print_statistics_to_file(Process *processes, int process_count, FILE *fp) {
    double total_turnaround = 0.0, total_waiting = 0.0, total_response = 0.0;
    int first_arrival = 1e9, last_completion = -1e9;
    int valid_count = 0;

    for (int i = 0; i < process_count; i++) {
        if (processes[i].is_completed && processes[i].completion_time >= 0 && processes[i].start_time >= 0) {
            int turnaround = processes[i].completion_time - processes[i].arrival_time;
            int waiting = turnaround - processes[i].burst_time;
            int response = processes[i].start_time - processes[i].arrival_time;

            total_turnaround += turnaround;
            total_waiting += waiting;
            total_response += response;
            valid_count++;

            if (processes[i].arrival_time < first_arrival) first_arrival = processes[i].arrival_time;
            if (processes[i].completion_time > last_completion) last_completion = processes[i].completion_time;
        }
    }

    if (valid_count == 0 || last_completion <= first_arrival) {
        fprintf(fp, "\n=== Simulation Statistics ===\n");
        fprintf(fp, "No valid processes completed — cannot compute metrics.\n");
        fprintf(fp, "==============================\n");
        return;
    }

    double avg_turnaround = total_turnaround / valid_count;
    double avg_waiting = total_waiting / valid_count;
    double avg_response = total_response / valid_count;
    double throughput = (double)valid_count / (last_completion - first_arrival);

    fprintf(fp, "\n=== Simulation Statistics ===\n");
    fprintf(fp, "Average Turnaround Time : %.2f\n", avg_turnaround);
    fprintf(fp, "Average Waiting Time    : %.2f\n", avg_waiting);
    fprintf(fp, "Average Response Time   : %.2f\n", avg_response);
    fprintf(fp, "Throughput              : %.2f processes/unit time\n", throughput);
    fprintf(fp, "==============================\n");
}

void reset_processes(Process* processes, int process_count) {
  for (int i = 0; i < process_count; i++) {
    processes[i].remaining_time = processes[i].burst_time;
    processes[i].start_time = -1;
    processes[i].completion_time = -1;
    processes[i].is_completed = false;
    processes[i].priority = processes[i].original_priority;
  }
}

// Compute and print statistics for HPF schedulers, broken down by priority.
void compute_hpf_statistics_to_file(Process *processes, int process_count, FILE *fp) {
    double total_turnaround[5] = {0}, total_waiting[5] = {0}, total_response[5] = {0};
    int count[5] = {0};
    int first_arrival = 1e9, last_completion = -1e9;

    fprintf(fp, "\n--- HPF Statistics ---\n");

    for (int i = 0; i < process_count; i++) {
        // Only include processes that have actually started and completed.
        if (processes[i].is_completed && processes[i].start_time != -1) {
            int priority = processes[i].original_priority;
            int turnaround = processes[i].completion_time - processes[i].arrival_time;
            int waiting = turnaround - processes[i].burst_time;
            int response = processes[i].start_time - processes[i].arrival_time;

            // Accumulate totals for the specific priority.
            total_turnaround[priority] += turnaround;
            total_waiting[priority] += waiting;
            total_response[priority] += response;
            count[priority]++;

            // Accumulate totals for the overall calculation.
            total_turnaround[0] += turnaround;
            total_waiting[0] += waiting;
            total_response[0] += response;
            count[0]++;

            if (processes[i].arrival_time < first_arrival) first_arrival = processes[i].arrival_time;
            if (processes[i].completion_time > last_completion) last_completion = processes[i].completion_time;
        }
    }

    double duration = (last_completion > first_arrival) ? (double)(last_completion - first_arrival) : 1.0;

    // Print per-priority statistics.
    for (int i = 1; i <= 4; i++) {
        fprintf(fp, "\nPriority %d:\n", i);
        if (count[i] > 0) {
            fprintf(fp, "  Average Turnaround Time: %.2f\n", total_turnaround[i] / count[i]);
            fprintf(fp, "  Average Waiting Time   : %.2f\n", total_waiting[i] / count[i]);
            fprintf(fp, "  Average Response Time  : %.2f\n", total_response[i] / count[i]);
            fprintf(fp, "  Throughput             : %.2f processes/unit time\n", count[i] / duration);
        } else {
            fprintf(fp, "  No processes of this priority ran.\n");
        }
    }

    // Print overall statistics.
    fprintf(fp, "\nOverall:\n");
    if (count[0] > 0) {
        fprintf(fp, "  Average Turnaround Time: %.2f\n", total_turnaround[0] / count[0]);
        fprintf(fp, "  Average Waiting Time   : %.2f\n", total_waiting[0] / count[0]);
        fprintf(fp, "  Average Response Time  : %.2f\n", total_response[0] / count[0]);
        fprintf(fp, "  Throughput             : %.2f processes/unit time\n", count[0] / duration);
    } else {
        fprintf(fp, "  No processes ran.\n");
    }
    fprintf(fp, "==============================\n");
}
