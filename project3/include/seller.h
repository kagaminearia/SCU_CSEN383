#ifndef SELL_H
#define SELL_H

#include <pthread.h>
#include <memory.h>

#include "seatmap.h"
#include "customer.h"
#include "util.h"

typedef enum SellerType {
    HIGH_SELLER = 'H',
    MID_SELLER = 'M',
    LOW_SELLER = 'L'
} SellerType;

typedef struct SellerArgs {
    SellerType seller_type;
    char seller_name[3];
    SeatMap* seat_map;
    Customer* customer_queue;
    int customer_count;

    // Random service time for seller
    // Initialized to 0 to indicate no current service
    int remaining_service_time;

    pthread_mutex_t* time_mtx;
    pthread_cond_t* time_cond;
    pthread_cond_t* wait_cond;

    int* current_time;
    int* finished_seller_count;
    bool* time_out;
    int* active_seller_count;
} SellerArgs;

void initialize_seller(SellerArgs* args, SellerType type, SeatMap* seat_map, int customer_count,
                       pthread_mutex_t* time_mtx, pthread_cond_t* time_cond, pthread_cond_t* wait_cond,
                       int* current_time, int* finished_seller_count,  bool* time_out, int* active_seller_count);

void* sell(void* seller_args);

#endif // SELL_H
