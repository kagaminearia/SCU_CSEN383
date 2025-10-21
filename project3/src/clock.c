#include "clock.h"

static int current_time = 0;

void increment_clock_time(void) {
    current_time++;
}

int get_clock_time(void) {
    return current_time;
}
