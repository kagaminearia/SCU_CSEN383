#include "hpf.h"
#include <stdbool.h>
#include <stdio.h>

// Find the best process to run at the current time.
// A process is a candidate if it has arrived, is not completed, and has remaining time.
// The best process is the one with the highest priority (lowest priority value).
// Ties are broken by FCFS (earlier arrival time).
static Process* find_best_process(Process *processes, int current_time) {
    Process *best_p = NULL;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        Process *p = &processes[i];
        if (!p->is_completed && p->arrival_time <= current_time) {
            if (best_p == NULL || p->priority < best_p->priority ||
                (p->priority == best_p->priority && p->arrival_time < best_p->arrival_time)) {
                best_p = p;
            }
        }
    }
    return best_p;
}

int hpf_p(Process **process_ptr, const char* output_filename) {
    Process *processes = *process_ptr;
    qsort(processes, PROCESS_COUNT, sizeof(Process), compare_by_arrival_time);

    FILE *output_file = fopen(output_filename, "a");
    if (output_file == NULL) {
        perror("Error opening output file");
        return 1;
    }

    fprintf(output_file, "HPF (preemptive)\n******************\nSimulated processes BEFORE execution:\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        print_process_to_file(&processes[i], output_file);
    }
    fprintf(output_file, "\n******************\nTime chart:\n");

    int current_time = 0;
    int completed_count = 0;
    Process *running_process = NULL;
    int wait_time[PROCESS_COUNT] = {0};

    while (completed_count < PROCESS_COUNT) {
        // --- Aging ---
        // At the beginning of each time step, check for waiting processes.
        if (current_time < END_TIME) {
            for (int i = 0; i < PROCESS_COUNT; i++) {
                Process *p = &processes[i];
                // A process is waiting if it has arrived, is not completed, and is not the currently running process.
                if (p->arrival_time <= current_time && !p->is_completed && p != running_process) {
                    wait_time[i]++;
                    if (wait_time[i] >= 5 && p->priority > 1) {
                        p->priority--;
                        wait_time[i] = 0; // Reset wait time after promotion.
                    }
                }
            }
        }

        // Find the highest priority process that is ready to run.
        Process *best_process = find_best_process(processes, current_time);

        // If no process is ready, advance time.
        if (best_process == NULL) {
            // If we are past the end time and nothing is running, we are done.
            if (current_time >= END_TIME) {
                break;
            }
            current_time++;
            continue;
        }

        // A process that arrives now cannot start if we are at the end time.
        if (current_time >= END_TIME && best_process->start_time == -1) {
            // If there's another process that was already running, it can continue.
            // But this new one cannot start. To handle this, we effectively end the simulation
            // for new processes by breaking the loop.
            break;
        }

        running_process = best_process;

        // Mark start time if it's the first time this process is running.
        if (running_process->start_time == -1) {
            running_process->start_time = current_time;
        }

        // Run the process for one time unit.
        fprintf(output_file, "%s ", running_process->name);
        running_process->remaining_time--;
        current_time++;

        // Check if the process has completed.
        if (running_process->remaining_time <= 0) {
            running_process->is_completed = true;
            running_process->completion_time = current_time;
            completed_count++;
        }

        // If all processes are done, we can exit early.
        if (completed_count == PROCESS_COUNT) {
            break;
        }
    }

    fprintf(output_file, "\n******************\nSimulated processes AFTER execution:\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        print_process_to_file(&processes[i], output_file);
    }

    compute_hpf_statistics_to_file(processes, PROCESS_COUNT, output_file);
    fclose(output_file);

    return 0;
}
