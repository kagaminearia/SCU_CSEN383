#include "metrics.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// One bucket per seller type: H, M, L
typedef struct {
    int total_response;
    int total_turnaround;
    int served;
    int turned_away;
} metrics_bucket_t;

static metrics_bucket_t buckets[3];
static pthread_mutex_t metrics_mutex = PTHREAD_MUTEX_INITIALIZER;

static int index_for_type(char t) {
    if (t == 'H') return 0;
    if (t == 'M') return 1;
    return 2; // 'L'
}

void metrics_init(void) {
    pthread_mutex_lock(&metrics_mutex);
    memset(buckets, 0, sizeof(buckets));
    pthread_mutex_unlock(&metrics_mutex);
}

void metrics_record(const cust_metrics_t* m) {
    pthread_mutex_lock(&metrics_mutex);
    int idx = index_for_type(m->seller_type);
    if (m->served) {
        buckets[idx].served++;
        buckets[idx].total_response += m->response_time;
        buckets[idx].total_turnaround += m->turnaround_time;
    } else {
        buckets[idx].turned_away++;
    }
    pthread_mutex_unlock(&metrics_mutex);
}

void metrics_summary(void) {
    pthread_mutex_lock(&metrics_mutex);

    char types[3] = {'H', 'M', 'L'};
    printf("\n========= METRICS SUMMARY =========\n");
    for (int i = 0; i < 3; i++) {
        int served = buckets[i].served;
        int turned = buckets[i].turned_away;
        double avg_resp = served ? (double)buckets[i].total_response / served : 0.0;
        double avg_turn = served ? (double)buckets[i].total_turnaround / served : 0.0;

        printf("Seller Type %c:\n", types[i]);
        printf("  Served         : %d\n", served);
        printf("  Turned Away    : %d\n", turned);
        printf("  Avg Response   : %.2f min\n", avg_resp);
        printf("  Avg Turnaround : %.2f min\n", avg_turn);
        printf("  Throughput     : %.2f customers/hour\n", (double)served);
        printf("-----------------------------------\n");
    }
    printf("====================================\n\n");

    pthread_mutex_unlock(&metrics_mutex);
}
