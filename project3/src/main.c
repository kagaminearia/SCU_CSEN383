#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "seller.h"
#include "clock.h"
#include "seating.h"
#include "metrics.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_customers_per_seller>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Number of customers must be a positive integer.\n");
        return 1;
    }

    printf("Starting simulation with %d customers per seller.\n", N);

    seating_init();
    metrics_init();

    pthread_t seller_threads[10];
    seller_args_t seller_args[10];

    create_seller_threads(N, seller_threads, seller_args);
    
    printf("Main thread: Starting simulation clock.\n");
    // Let the simulation run for a minute (60 seconds)
    while (get_clock_time() < 60 && !seating_sold_out()) {
        usleep(100); // 0.1 millisecond
        increment_clock_time();
        notify_sellers_of_time_change();
    }
    printf("Main thread: Simulation clock stopped.\n");
    
    // Wake up all threads so they can terminate
    printf("Main thread: Waking up all seller threads to terminate.\n");
    wakeup_all_seller_threads();

    // Wait for all seller threads to finish
    for (int i = 0; i < 10; i++) {
        pthread_join(seller_threads[i], NULL);
        printf("Main thread: Joined seller thread %d.\n", i);
    }

    printf("Simulation finished.\n");
    print_seat_map();
    metrics_summary();

    return 0;
}
