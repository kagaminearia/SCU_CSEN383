#ifndef SELLER_H
#define SELLER_H

#include <pthread.h>
#include <stdbool.h>

#define CUSTOMER_NAME_LEN 16

typedef enum {
    H,
    M,
    L
} seller_type_t;

typedef struct customer {
    int  seq;
    int  arrival_min;
    int  service_min;
    char ticket_id[CUSTOMER_NAME_LEN];
    struct customer *next;
} customer_t;

typedef struct {
    char          id_prefix[4];
    seller_type_t type;
    char          type_char;
    int           seller_index;
    customer_t   *q_head;
    int           served_count;
    long          sum_response;
    long          sum_turnaround;
} seller_args_t;

void *sell(void *arg);

void create_seller_threads(int N, pthread_t out_threads[10], seller_args_t out_args[10]);
customer_t *generate_customer_queue(const char *id_prefix, seller_type_t type, int N);
void wakeup_all_seller_threads(void);
void notify_sellers_of_time_change(void);

#endif
