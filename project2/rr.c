#include "rr.h"
#include "util.h"
#include "process.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void rr(Process **process_ptr, const char* output_filename) {
    Process *processes = *process_ptr;
    qsort(processes, PROCESS_COUNT, sizeof(Process), compare_by_arrival_time);  // arrival order baseline

    FILE *output_file = fopen(output_filename, "a");
    if (output_file == NULL) {
        perror("Error opening output file");
        return;
    }

    fprintf(output_file, "RR Scheduling\n******************\nSimulated processes BEFORE execution:\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        print_process_to_file(&processes[i], output_file);
    }

    fprintf(output_file, "\n******************\nTime chart:\n");

    int quantum = TIME_QUANTUM;
    double current_time = START_TIME;
    int completed = 0;
    int queue[PROCESS_COUNT];
    int front = 0, rear = 0;
    bool in_queue[PROCESS_COUNT];
    for (int i = 0; i < PROCESS_COUNT; i++) in_queue[i] = false;

    // Initially enqueue arrivals at start
    for (int i = 0; i < PROCESS_COUNT; i++) {
        if (processes[i].arrival_time <= current_time) {
            queue[rear++] = i;
            in_queue[i] = true;
        }
    }

    // reinitialize process state (to handle reused process array in simulation)
    for (int i = 0; i < PROCESS_COUNT; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].is_completed = false;
        processes[i].start_time = -1;
        processes[i].completion_time = -1;
    }

    while (completed < PROCESS_COUNT && current_time < END_TIME) {
        // Add new arrivals before processing, to prevent false idle cycles
        for (int i = 0; i < PROCESS_COUNT; i++) {
            if (!in_queue[i] && !processes[i].is_completed && processes[i].arrival_time <= current_time) {
                queue[rear++] = i;
                if (rear == PROCESS_COUNT) rear = 0;
                in_queue[i] = true;
            }
        }

        // If still no ready process, mark as idle
        if (front == rear) {
            // Skip idle printing when no processes have arrived yet; advance until first arrival
            double next_arrival = END_TIME;
            for (int i = 0; i < PROCESS_COUNT; i++) {
                if (!processes[i].is_completed && processes[i].arrival_time > current_time) {
                    if (processes[i].arrival_time < next_arrival)
                        next_arrival = processes[i].arrival_time;
                }
            }
            if (next_arrival < END_TIME)
                current_time = next_arrival;
            else
                break;
            continue;
        }

        int idx = queue[front++];
        if (front >= PROCESS_COUNT) front = 0;
        in_queue[idx] = false;
        Process *p = &processes[idx];

        if (p->start_time == -1) p->start_time = current_time;

        double run_for = (p->remaining_time < quantum) ? p->remaining_time : quantum;

        fprintf(output_file, "%s ", p->name);

        p->remaining_time -= run_for;
        current_time += run_for;

        if (p->remaining_time <= 0.0) {
            p->completion_time = current_time;
            p->is_completed = true;
            p->remaining_time = 0.0;
            completed++;
        } else {
            // Enqueue any newly arrived, never-started processes up to current_time
            for (int i = 0; i < PROCESS_COUNT; i++) {
                if (!in_queue[i] && !processes[i].is_completed &&
                    processes[i].arrival_time <= current_time &&
                    processes[i].start_time == -1) {
                    queue[rear++] = i;
                    if (rear == PROCESS_COUNT) rear = 0;
                    in_queue[i] = true;
                }
            }
            // Re-enqueue the current process
            queue[rear++] = idx;
            if (rear == PROCESS_COUNT) rear = 0;
            in_queue[idx] = true;
        }
    }

    fprintf(output_file, "\n\n******************\nSimulated processes AFTER execution:\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        print_process_to_file(&processes[i], output_file);
    }

    compute_and_print_statistics_to_file(processes, PROCESS_COUNT, output_file);
    fclose(output_file);
}
