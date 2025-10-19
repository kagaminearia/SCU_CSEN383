#ifndef SEATMAP_H
#define SEATMAP_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "constants.h"

typedef struct SeatMap {
    char seats[ROW][COL][CUSTOMER_NAME_LEN];
    int remaining_seat_count;
    pthread_mutex_t mutex;
} SeatMap;

void initialize_seat_map(SeatMap* map);
void _lock_seat_map(SeatMap* map);
void _unlock_seat_map(SeatMap* map);
bool assign_seat_in_row(SeatMap* map, int row, const char* customer_name);
void print_seat_map(SeatMap* map);
int get_remaining_seat_count(SeatMap* map);

#endif
