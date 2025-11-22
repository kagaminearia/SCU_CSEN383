#ifndef UTILS_H
#define UTILS_H

// Initialize the start time (call this once at program start)
void init_time(void);

// Get current elapsed time in seconds (with decimal precision for milliseconds)
double now(void);

// Format time as M:SS.mmm (minutes:seconds.milliseconds)
void format_timestamp(double seconds, char *buffer);

// Write message to output.txt with parent timestamp prepended
void handle_parent_output(double parent_ts, const char *line);

#endif // UTILS_H

