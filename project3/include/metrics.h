#ifndef METRICS_H
#define METRICS_H

typedef struct {
    char seller_type;       // 'H', 'M', or 'L'
    int response_time;      // start - arrival
    int turnaround_time;    // completion - arrival
    int served;             // 1 if served, 0 if turned away
} cust_metrics_t;

void metrics_init(void);
void metrics_record(const cust_metrics_t* m);
void metrics_summary(void);

#endif // METRICS_H
