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
  printf("Name: %s, Arrival Time: %.2f, Burst Time: %.2f, Remaining Time: %.2f, Priority: %d, Start Time: %.2f, Completion Time: %.2f, Completed: %s\n",
         p->name, p->arrival_time, p->burst_time, p->remaining_time, p->priority, p->start_time, p->completion_time, p->is_completed ? "true" : "false");
}
