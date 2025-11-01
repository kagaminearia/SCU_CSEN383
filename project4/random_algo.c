#include "page_def.h"

void random_algo(pages_list* pageList) {
    if (pageList == NULL || pageList->head == NULL) {
        return;
    }

    int r = rand() % TOTAL_PAGE;
    int counter = 0;

    page* rand_page = NULL;
    page* curr_page = pageList->head;

    // Find a valid page to start from
    while (curr_page != NULL && curr_page->pid < 0) {
        curr_page = curr_page->next;
    }

    // If no valid page is found, return
    if (curr_page == NULL) {
        return;
    }
    
    rand_page = curr_page;

    for (; curr_page != NULL && counter <= r; curr_page = curr_page->next, counter++) {
        if (curr_page->pid > 0) {
            rand_page = curr_page;
        }
    }

    if (rand_page != NULL) {
        if (DEBUG == 1) {
            printf("EVICTED ones :: p[%03d] c:%02d l:%02f\n", rand_page->pid, rand_page->counter, rand_page->loadTime);
        }
        rand_page->pid = -1;
        rand_page->page_num = -1;
    }
}