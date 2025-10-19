#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <stdio.h>

#include "constants.h"
#include "util.h"

typedef struct Customer {
    char name[CUSTOMER_NAME_LEN];
    int arrival_time;
    int start_time;
    int completion_time;
} Customer;

void initialize_customer(
  Customer* customer, int customer_idx, const char* seller_name);

void print_customer(const Customer* customer);

#endif // CUSTOMER_H
