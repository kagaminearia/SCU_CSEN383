#ifndef PAGE_H
#define PAGE_H

#define DEBUG 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_PROCESS 150
#define TOTAL_DURATION 60
#define PROCESS_DURATION 5
#define TOTAL_PAGE 100


typedef struct {
    int pid, pageCounter, arrivalTime, serviceDuration, pageReference;
} process;

typedef struct page {
    int pid;
    int page_num;
    struct page* next;
    float timeBought;
    float loadTime;
    int counter;
} page;

typedef struct {
    page* head;
} pages_list;


int pagesWhichAreFree(pages_list*,int);


int pagesInMemory(pages_list*,int,int);

page* pageFree(pages_list*);


void free_mem(pages_list*,int);


void initialize_pages(pages_list*);


void display(pages_list*);


int generate_next_pagenum(int,int);


int compare_arrival(const void* ,const void*);


page* pageIDFree(pages_list*,int,int);

void fifo(pages_list*);
void lru(pages_list*);
void lfu(pages_list*);
void mfu(pages_list*);
void random_algo(pages_list*);

// Statistics and display functions
void print_memory_map(pages_list*);
void print_process_record(int, char*, char*, int, int, pages_list*);
void print_page_reference_trace(float, int, int, int, int, int);
void calculate_and_print_statistics(int, int, int);



#endif