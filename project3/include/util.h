#ifndef UTIL_H
#define UTIL_H

#include <time.h>
#include <stdlib.h>

#include "customer.h"

// Debug macros
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) \
    fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) \
    do { } while (0)
#endif

int generate_arrival_time();
int generate_high_seller_service_time();
int generate_mid_seller_service_time();
int generate_low_seller_service_time();
int compare_by_customer_arrival_time(const void* a, const void* b);

#endif
