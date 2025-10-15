#include "fcfs.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void fcfs(Process **process_ptr, const char* output_filename) {
    Process *processes = *process_ptr;
    qsort(processes, PROCESS_COUNT, sizeof(Process), compare_by_arrival_time);

    FILE *output_file = fopen(output_filename, "a");
    if (output_file == NULL) {
        perror("Error opening output file");
        return;
    }

    fprintf(output_file, "FCFS Scheduling\n******************\nSimulated processes BEFORE execution:\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        print_process_to_file(&processes[i], output_file);
    }

    fprintf(output_file, "\n******************\nTime chart:\n");

    double current_time = 0.0;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        Process *p = &processes[i];

        if (current_time < p->arrival_time) {
            current_time = p->arrival_time;
        }

        if (current_time >= END_TIME) {
            continue;
        }

        p->start_time = current_time;
        fprintf(output_file, "%s ", p->name);
        current_time += p->burst_time;
        p->completion_time = current_time;
        p->remaining_time = 0.0;
        p->is_completed = true;
    }

    fprintf(output_file, "\n\n******************\nSimulated processes AFTER execution:\n");
    for (int i = 0; i < PROCESS_COUNT; i++) {
        print_process_to_file(&processes[i], output_file);
    }

    compute_and_print_statistics_to_file(processes, PROCESS_COUNT, output_file);
    fclose(output_file);
}