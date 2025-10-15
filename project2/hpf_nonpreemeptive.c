#include "hpf.h"
#include <stdbool.h>
#include <stdio.h>

// Find the best process to start. In non-preemptive HPF, once a process starts, it runs to completion.
// A process is a candidate if it has not started, has arrived, and is not completed.
static Process* find_next_to_start(Process *processes, int current_time) {
    Process *best_p = NULL;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        Process *p = &processes[i];
        // Must not be completed, must not have started, and must have arrived.
        if (!p->is_completed && p->start_time == -1 && p->arrival_time <= current_time) {
            if (best_p == NULL || p->priority < best_p->priority ||
                (p->priority == best_p->priority && p->arrival_time < best_p->arrival_time)) {
                best_p = p;
            }
        }
    }
    return best_p;
}

int hpf_np(Process **process_ptr, const char* output_filename) {
    Process *processes = *process_ptr;
    qsort(processes, PROCESS_COUNT, sizeof(Process), compare_by_arrival_time);

    FILE *output_file = fopen(output_filename, "a");
    if (output_file == NULL) {
        perror("Error opening output file");
        return 1;
    }

    fprintf(output_file, "HPF (non-preemptive)\n******************\nSimulated processes BEFORE execution:\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        print_process_to_file(&processes[i], output_file);
    }
    fprintf(output_file, "\n******************\nTime chart:\n");

    int current_time = 0;
    int completed_count = 0;
    int wait_time[PROCESS_COUNT] = {0};

    while (completed_count < PROCESS_COUNT && current_time < END_TIME) {
        // --- Aging ---
        // Before selecting a new process, apply aging to all waiting processes.
        if (current_time < END_TIME) {
            for (int i = 0; i < PROCESS_COUNT; i++) {
                Process *p = &processes[i];
                // A process is waiting if it has arrived but has not yet started.
                if (p->arrival_time <= current_time && p->start_time == -1 && !p->is_completed) {
                    wait_time[i]++;
                    if (wait_time[i] >= 5 && p->priority > 1) {
                        p->priority--;
                        wait_time[i] = 0; // Reset wait time after promotion.
                    }
                }
            }
        }

        // Find the best process to start from the pool of waiting processes.
        Process *process_to_run = find_next_to_start(processes, current_time);

        if (process_to_run == NULL) {
            // No process is ready, advance time.
            current_time++;
            continue;
        }

        // A process cannot start at or after the end time.
        if (current_time >= END_TIME) {
            break;
        }

        // Execute the chosen process non-preemptively (to completion).
        process_to_run->start_time = current_time;
        fprintf(output_file, "%s ", process_to_run->name);
        
        // Advance time by the burst time of the process.
        current_time += process_to_run->burst_time;

        process_to_run->completion_time = current_time;
        process_to_run->remaining_time = 0;
        process_to_run->is_completed = true;
        completed_count++;
    }

    fprintf(output_file, "\n******************\nSimulated processes AFTER execution:\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        print_process_to_file(&processes[i], output_file);
    }

    compute_hpf_statistics_to_file(processes, PROCESS_COUNT, output_file);
    fclose(output_file);

    return 0;
}
