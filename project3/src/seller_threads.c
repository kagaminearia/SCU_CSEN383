#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include "seller.h"
#include "clock.h"
#include "seating.h"
#include "metrics.h"

static pthread_mutex_t seller_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t seller_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
static int wakeup_flag = 0;

static void safe_printf(const char *format, ...) {
    pthread_mutex_lock(&print_mutex);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    pthread_mutex_unlock(&print_mutex);
}

static int rand_in_range(int lo, int hi) { return lo + (rand() % (hi - lo + 1)); }

static int service_time_for_type(seller_type_t t) {
    if (t == H) return rand_in_range(1,2);
    if (t == M) return rand_in_range(2,4);
    return rand_in_range(4,7);
}

static customer_t *insert_sorted(customer_t *head, customer_t *node) {
    if (!head || node->arrival_min < head->arrival_min) {
        node->next = head;
        return node;
    }
    customer_t *p = head;
    while (p->next && p->next->arrival_min <= node->arrival_min) p = p->next;
    node->next = p->next;
    p->next = node;
    return head;
}

customer_t *generate_customer_queue(const char *id_prefix, seller_type_t type, int N) {
    customer_t *head = NULL;
    for (int i = 0; i < N; ++i) {
        customer_t *c = malloc(sizeof(customer_t));
        c->seq = i;
        c->arrival_min = rand_in_range(0,59);
        c->service_min = service_time_for_type(type);
        snprintf(c->ticket_id, sizeof(c->ticket_id), "%s-%03d", id_prefix, i+1);
        c->next = NULL;
        head = insert_sorted(head, c);
    }
    return head;
}

void create_seller_threads(int N, pthread_t out_threads[10], seller_args_t out_args[10]) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }

    struct { const char *id; seller_type_t t; char c; } spec[10] = {
        {"H1", H, 'H'},
        {"M1", M, 'M'}, {"M2", M, 'M'}, {"M3", M, 'M'},
        {"L1", L, 'L'}, {"L2", L, 'L'}, {"L3", L, 'L'},
        {"L4", L, 'L'}, {"L5", L, 'L'}, {"L6", L, 'L'}
    };

    for (int i = 0; i < 10; ++i) {
        seller_args_t *a = &out_args[i];
        memset(a, 0, sizeof(*a));
        strncpy(a->id_prefix, spec[i].id, sizeof(a->id_prefix)-1);
        a->type = spec[i].t;
        a->type_char = spec[i].c;
        a->seller_index = i;
        a->q_head = generate_customer_queue(a->id_prefix, a->type, N);

        int rc = pthread_create(&out_threads[i], NULL, sell, a);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed for %s (%d)\n", a->id_prefix, rc);
            exit(1);
        }
    }
}

void wakeup_all_seller_threads(void) {
    pthread_mutex_lock(&seller_mutex);
    wakeup_flag = 1;
    pthread_cond_broadcast(&seller_cond);
    pthread_mutex_unlock(&seller_mutex);
}

void notify_sellers_of_time_change(void) {
    pthread_mutex_lock(&seller_mutex);
    pthread_cond_broadcast(&seller_cond);
    pthread_mutex_unlock(&seller_mutex);
}

void *sell(void *arg) {
    seller_args_t *args = (seller_args_t *)arg;
    customer_t *customer = args->q_head;
    while (customer != NULL) {
        // Wait for customer to arrive
        pthread_mutex_lock(&seller_mutex);
        while (get_clock_time() < customer->arrival_min && !wakeup_flag) {
            pthread_cond_wait(&seller_cond, &seller_mutex);
        }
        pthread_mutex_unlock(&seller_mutex);

        // If the simulation is over, turn away this and all subsequent customers.
        if (wakeup_flag) {
            while(customer != NULL) {
                safe_printf("%02d:%02d Customer %s leaves queue for %s (window closed).\n", get_clock_time() / 60, get_clock_time() % 60, customer->ticket_id, args->id_prefix);
                cust_metrics_t m = {args->type_char, 0, 0, 0};
                metrics_record(&m);
                customer = customer->next;
            }
            break; // End of this seller's work.
        }

        safe_printf("%02d:%02d Customer %s arrives at queue for %s.\n", get_clock_time()/60, get_clock_time()%60, customer->ticket_id, args->id_prefix);

        int row, col;
        if (seating_assign(args->type_char, customer->ticket_id, &row, &col)) {
            int start_time = get_clock_time();
            safe_printf("%02d:%02d Customer %s assigned seat %d-%d\n", start_time/60, start_time%60, customer->ticket_id, row + 1, col + 1);
            
            // Wait for service to complete. This is interruptible by wakeup_flag.
            int completion_time_target = start_time + customer->service_min;
            pthread_mutex_lock(&seller_mutex);
            while (get_clock_time() < completion_time_target && !wakeup_flag) {
                pthread_cond_wait(&seller_cond, &seller_mutex);
            }
            pthread_mutex_unlock(&seller_mutex);

            // If the simulation ended during service time, don't complete the sale.
            if (wakeup_flag) {
                // The customer has a seat but the sale could not complete. 
                // In a real system, we'd need to handle this (e.g., void the ticket).
                // For this simulation, we'll just have the seller thread exit.
                break;
            }

            int completion_time = get_clock_time();
            safe_printf("%02d:%02d Customer %s purchase complete.\n", completion_time/60, completion_time%60, customer->ticket_id);
            pthread_mutex_lock(&print_mutex);
            print_seat_map();
            pthread_mutex_unlock(&print_mutex);
            cust_metrics_t m = {args->type_char, start_time - customer->arrival_min, completion_time - customer->arrival_min, 1};
            metrics_record(&m);
        } else {
            safe_printf("%02d:%02d Customer %s turned away, concert sold out.\n", get_clock_time() / 60, get_clock_time() % 60, customer->ticket_id);
            cust_metrics_t m = {args->type_char, 0, 0, 0};
            metrics_record(&m);
        }

        customer = customer->next;
    }
    return NULL;
}