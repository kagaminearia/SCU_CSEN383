#ifndef SEATING_H
#define SEATING_H

#include <stdbool.h>

void seating_init(void);
bool seating_assign(char seller_type, const char* customer_id, int* out_row, int* out_col);
int seating_sold_count(void);
bool seating_sold_out(void);
const char* seating_at(int row, int col);
void print_seat_map(void);

#endif // SEATING_H