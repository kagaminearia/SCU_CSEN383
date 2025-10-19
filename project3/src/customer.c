#include "customer.h"

void initialize_customer(
  Customer* customer, int customer_idx, const char* seller_name) {
    snprintf(customer->name, sizeof(customer->name), "%s-%d", seller_name, customer_idx);
    customer->arrival_time = generate_arrival_time();
    // Use -1 to indicate that this customer has not been served yet
    customer->start_time = -1;
    // Use -1 to indicate that this customer has not completed service yet
    customer->completion_time = -1;
}

void print_customer(const Customer* customer) {
    printf("Customer Name: %s, arrival time: %d, ", customer->name, customer->arrival_time);
    if (customer->start_time == -1) {
        printf("and has not been served yet\n");
        return;
    }
    if (customer->completion_time == -1) {
        printf("service time: %d, and is currently being served\n", customer->start_time);
        return;
    }
    printf("service time: %d, completion time: %d\n", customer->start_time, customer->completion_time);
}
