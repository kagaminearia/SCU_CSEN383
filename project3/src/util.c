#include "util.h"

int generate_arrival_time() {
    return rand() % (END_TIME + 1);
}

int generate_high_seller_service_time() {
    return (rand() % 2) + 1;
}

int generate_mid_seller_service_time() {
    return (rand() % 3) + 2;
}

int generate_low_seller_service_time() {
    return (rand() % 4) + 4;
}

int compare_by_customer_arrival_time(const void* a, const void* b) {
    const Customer* c1 = (const Customer*)a;
    const Customer* c2 = (const Customer*)b;
    if (c1->arrival_time < c2->arrival_time) return -1;
    if (c1->arrival_time > c2->arrival_time) return 1;
    return 0;
}
