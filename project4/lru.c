#include "page_def.h"

void lru(pages_list* pageList) {
    if (pageList == NULL || pageList->head == NULL) {
        return;
    }

    page* curr_page = pageList->head;
    page* lru_page = NULL;
    float oldest_load_time = -1;

    // Find the page with the oldest loadTime
    while (curr_page != NULL) {
        if (curr_page->pid >= 0) {
            // Check if this is the first valid page found
            // If not, check if it has an older loadTime
            if (lru_page == NULL) {
                lru_page = curr_page;
                oldest_load_time = curr_page->loadTime;
            } else if (curr_page->loadTime < oldest_load_time) {
                lru_page = curr_page;
                oldest_load_time = curr_page->loadTime;
            }
        }
        curr_page = curr_page->next;
    }

    // Evict the least recently used page
    if (lru_page != NULL) {
        lru_page->pid = -1;
        lru_page->page_num = -1;
    }
}

