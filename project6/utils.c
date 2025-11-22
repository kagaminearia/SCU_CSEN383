#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

static struct timeval program_start_time = {0, 0};
static int output_fd = -1;

// Initialize the start time (call this once at program start)
void init_time(void) {
    gettimeofday(&program_start_time, NULL);
    
    // Open output file for writing
    output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("open output.txt");
        exit(1);
    }
}

// Get current elapsed time in seconds (with decimal precision for milliseconds)
double now(void) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    
    long seconds = current_time.tv_sec - program_start_time.tv_sec;
    long microseconds = current_time.tv_usec - program_start_time.tv_usec;
    
    if (microseconds < 0) {
        seconds--;
        microseconds += 1000000;
    }
    
    // Return time in seconds with millisecond precision
    return seconds + (microseconds / 1000000.0);
}

// Format time as M:SS.mmm (minutes:seconds.milliseconds)
void format_timestamp(double seconds, char *buffer) {
    int total_ms = (int)(seconds * 1000.0);
    int minutes = total_ms / 60000;
    int sec = (total_ms % 60000) / 1000;
    int milliseconds = total_ms % 1000;
    
    sprintf(buffer, "%d:%02d.%03d", minutes, sec, milliseconds);
}

// Write message to output.txt with parent timestamp prepended
void handle_parent_output(double parent_ts, const char *line) {
    if (output_fd == -1) {
        // File not initialized, try to open it
        output_fd = open("output.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (output_fd == -1) {
            perror("open output.txt");
            return;
        }
    }
    
    // Format parent timestamp
    char parent_time_str[16];
    format_timestamp(parent_ts, parent_time_str);
    
    // Create output line: "parent_timestamp: child_message"
    char output_line[512];
    snprintf(output_line, sizeof(output_line), "%s: %s", parent_time_str, line);
    
    // Write to file
    write(output_fd, output_line, strlen(output_line));
    fsync(output_fd);  // Ensure data is written to disk
}

