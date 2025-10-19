#include "seatmap.h"

void initialize_seat_map(SeatMap* map) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            strcpy(map->seats[i][j], "-");
        }
    }
    map->remaining_seat_count = ROW * COL;
    pthread_mutex_init(&map->mutex, NULL);
}

void _lock_seat_map(SeatMap* map) {
    pthread_mutex_lock(&map->mutex);
}

void _unlock_seat_map(SeatMap* map) {
    pthread_mutex_unlock(&map->mutex);
}

bool assign_seat_in_row(SeatMap* map, int row, const char* customer_name) {
    _lock_seat_map(map);

    for (int j = 0; j < COL; j++) {
        if (strcmp(map->seats[row][j], "-") == 0) {
            strcpy(map->seats[row][j], customer_name);
            map->remaining_seat_count--;
            _unlock_seat_map(map);
            return true;
        }
    }

    _unlock_seat_map(map);
    return false;
}

int get_remaining_seat_count(SeatMap* map) {
    _lock_seat_map(map);
    int count = map->remaining_seat_count;
    _unlock_seat_map(map);
    return count;
}

void print_seat_map(SeatMap* map) {
    printf("Seat Map:\n");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            printf("%s ", map->seats[i][j]);
        }
        printf("\n");
    }
    printf("Remaining Seats: %d\n", map->remaining_seat_count);
}
