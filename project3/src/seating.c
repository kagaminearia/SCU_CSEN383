#include "seating.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#define ROW 10
#define COL 10
#define CUSTOMER_NAME_LEN 16

static char seats[ROW][COL][CUSTOMER_NAME_LEN];
static int sold_count = 0;
static pthread_mutex_t seat_mutex = PTHREAD_MUTEX_INITIALIZER;

// H seller: starts row 1 (idx 0) to 10 (idx 9)
const int HIGH_SELLER_ORDER[ROW] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
// M seller: starts row 5, 6, 4, 7, 3, 8, 2, 9, 1, 10
const int MID_SELLER_ORDER[ROW] = {4, 5, 3, 6, 2, 7, 1, 8, 0, 9};
// L seller: starts row 10 (idx 9) to 1 (idx 0)
const int LOW_SELLER_ORDER[ROW] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

void seating_init(void) {
    pthread_mutex_lock(&seat_mutex);
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            strcpy(seats[i][j], "---");
        }
    }
    sold_count = 0;
    pthread_mutex_unlock(&seat_mutex);
}

bool seating_assign(char seller_type, const char* customer_id, int* out_row, int* out_col) {
    pthread_mutex_lock(&seat_mutex);
    if (sold_count >= ROW * COL) {
        pthread_mutex_unlock(&seat_mutex);
        return false; // Sold out
    }

    const int* row_order;
    switch (seller_type) {
        case 'H':
            row_order = HIGH_SELLER_ORDER;
            break;
        case 'M':
            row_order = MID_SELLER_ORDER;
            break;
        case 'L':
            row_order = LOW_SELLER_ORDER;
            break;
        default:
            pthread_mutex_unlock(&seat_mutex);
            return false; // Should not happen
    }

    for (int i = 0; i < ROW; i++) {
        int row = row_order[i];
        for (int col = 0; col < COL; col++) {
            if (strcmp(seats[row][col], "---") == 0) {
                strncpy(seats[row][col], customer_id, CUSTOMER_NAME_LEN - 1);
                seats[row][col][CUSTOMER_NAME_LEN - 1] = '\0';
                sold_count++;
                if (out_row) *out_row = row;
                if (out_col) *out_col = col;
                pthread_mutex_unlock(&seat_mutex);
                return true;
            }
        }
    }

    pthread_mutex_unlock(&seat_mutex);
    return false; // No seat found, should be caught by sold_count check
}

int seating_sold_count(void) {
    pthread_mutex_lock(&seat_mutex);
    int count = sold_count;
    pthread_mutex_unlock(&seat_mutex);
    return count;
}

bool seating_sold_out(void) {
    pthread_mutex_lock(&seat_mutex);
    bool sold_out = (sold_count >= ROW * COL);
    pthread_mutex_unlock(&seat_mutex);
    return sold_out;
}

const char* seating_at(int row, int col) {
    // This is for reading, less critical to lock, but good practice
    pthread_mutex_lock(&seat_mutex);
    const char* customer = seats[row][col];
    pthread_mutex_unlock(&seat_mutex);
    return customer;
}

void print_seat_map(void) {
    pthread_mutex_lock(&seat_mutex);
    printf("\n================================== Seating Chart ==================================\n");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            printf("%-8s", seats[i][j]);
        }
        printf("\n");
    }
    printf("===================================================================================\n\n");
    pthread_mutex_unlock(&seat_mutex);
}