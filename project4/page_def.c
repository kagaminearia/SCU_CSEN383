#include "page_def.h"

void initialize_pages(pages_list* pgList) 
{
    pgList->head = malloc(sizeof(page));
    page* p = pgList->head;
    int i;
    for(i = 0; i < TOTAL_PAGE; i++) 
    {
        p->pid = -1; 
        p->page_num = -1;
        p->next = NULL;
        if(i < 99)
        {
            p->next = malloc(sizeof(page));
            p = p->next;
        }
    }
}

void display(pages_list* pgList) 
{
    page* p = pgList->head;
    int count = 0;
    while(p) 
    {
        printf(p->pid > 0 ? "|*| p[%03d] c:%02d l:%02f |*|" : "|*|",p->pid, p->counter, p->loadTime);
        count++;
        if((count % 10) == 0) 
            printf("\n");
        p = p->next;
    }
    printf("\n");
}

int pagesWhichAreFree(pages_list* pgList,int counter) 
{
    page* p = pgList->head;
    while(p) 
    {
        if(p->pid == -1)  
            counter--;

        if(!counter) 
            return 1;
        p = p->next;
    }
    return 0;
}


int pagesInMemory(pages_list* pgList,int pid,int page_num) 
{
    page* p = pgList->head;
    while(p) 
    {
        if(p->pid == pid && p->page_num == page_num) 
            return 1;
        p = p->next;
    }
    return 0;
}

page* pageFree(pages_list* pgList) 
{
    page* p = pgList->head;
    while(p) 
    {
        if(p->pid == -1) 
            return p;
        p = p->next;
    }
    return NULL;
}


void free_mem(pages_list* pgList,int pid) 
{
    page* p = pgList->head;
    while(p) 
    {
        if(p->pid == pid) 
        {
            p->pid = -1;
            p->page_num = -1;
        }
        p = p->next;
    }
}


int generate_next_pagenum(int curr_page_no,int max_page_size) 
{
    int num;
    // Generate random number r from 0 through 10 (inclusive)
    int r = rand() % 11; // 0, 1, 2, ..., 10 (11 possible values)
    
    if(0 <= r && r < 7) 
    {
        // Generate a random Δi to be -1, 0, or +1
        int delta = (rand() % 3) - 1; // -1, 0, or +1
        num = curr_page_no + delta;
        
        // Handle wrap-around: i wraps around from max to 0
        if(num < 0) 
            num = max_page_size - 1; // Wrap from 0 to last page
        else if(num >= max_page_size)
            num = 0; // Wrap from last page to 0
    } 
    else 
    {
        // For 7 ≤ r ≤ 10: randomly generate j where 2 ≤ |Δi| ≤ 9
        // j must satisfy: 0 ≤ j ≤ i-2 OR i+2 ≤ j ≤ max_size-1
        do {
            if(curr_page_no >= 2 && curr_page_no + 2 < max_page_size) {
                // Can choose from either range
                if(rand() % 2 == 0) {
                    // Choose from 0 to i-2
                    num = rand() % (curr_page_no - 1); // 0 to i-2
                } else {
                    // Choose from i+2 to max_size-1
                    num = (curr_page_no + 2) + rand() % (max_page_size - curr_page_no - 2);
                }
            } else if(curr_page_no >= 2) {
                // Only can choose from 0 to i-2
                num = rand() % (curr_page_no - 1); // 0 to i-2
            } else {
                // Only can choose from i+2 to max_size-1
                num = (curr_page_no + 2) + rand() % (max_page_size - curr_page_no - 2);
            }
        } while(abs(num - curr_page_no) < 2 || abs(num - curr_page_no) > 9);
    }

    return num;
}


page* pageIDFree(pages_list* pgList,int pid,int page_num) 
{
    page* p = pgList->head;
    while(p) 
    {
        if(p->pid == pid && p->page_num == page_num) 
            return p;
        p = p->next;
    }
    return NULL;
}


int compare_arrival(const void* a,const void* b) 
{
    return ((process*)a)->arrivalTime - ((process*)b)->arrivalTime;
}

// Print memory map as required by assignment
void print_memory_map(pages_list* pgList) 
{
    page* p = pgList->head;
    int count = 0;
    printf("Memory Map: ");
    while(p) 
    {
        if(p->pid >= 0) 
        {
            printf("%c", 'A' + (p->pid % 26)); // Use letters A-Z for process names
        } 
        else 
        {
            printf("."); // Dot for free pages
        }
        count++;
        p = p->next;
    }
    printf("\n");
}

// Print process entry/exit record as required by assignment
void print_process_record(int timestamp, char* process_name, char* action, int size_pages, int service_duration, pages_list* pgList) 
{
    printf("<%d, %s, %s, %d pages, %d seconds, ", timestamp, process_name, action, size_pages, service_duration);
    print_memory_map(pgList);
    printf(">\n");
}

// Print page reference trace as required by assignment
void print_page_reference_trace(float timestamp, int process_id, int page_referenced, int page_in_memory, int evicted_pid, int evicted_page) 
{
    char process_name = 'A' + (process_id % 26);
    char evicted_name = (evicted_pid >= 0) ? ('A' + (evicted_pid % 26)) : 'X';
    
    if (page_in_memory) {
        printf("<%.1f, %c, page-%d, HIT, evict %c-page%d>\n", 
               timestamp, process_name, page_referenced, evicted_name, evicted_page);
    } else {
        printf("<%.1f, %c, page-%d, MISS, evict %c-page%d>\n", 
               timestamp, process_name, page_referenced, evicted_name, evicted_page);
    }
}

// Calculate and print statistics
void calculate_and_print_statistics(int total_references, int hits, int misses) 
{
    double hit_ratio = (double)hits / total_references;
    double miss_ratio = (double)misses / total_references;
    
    printf("\n=== STATISTICS ===\n");
    printf("Total Page References: %d\n", total_references);
    printf("Hits: %d\n", hits);
    printf("Misses: %d\n", misses);
    printf("Hit Ratio: %.4f (%.2f%%)\n", hit_ratio, hit_ratio * 100);
    printf("Miss Ratio: %.4f (%.2f%%)\n", miss_ratio, miss_ratio * 100);
    printf("==================\n\n");
}