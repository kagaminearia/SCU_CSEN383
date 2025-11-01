#include "page_def.h"
#include <limits.h>

void lfu(pages_list* pageList) {
    if (pageList == NULL || pageList->head == NULL) {
        return;
    }

    page* curr = pageList->head;
    page* evict = NULL;
    int min_usage = INT_MAX;     // LFU: smallest counter wins
    float oldest_last_access = 0;// tie-breaker: oldest loadTime (interpreted as last access)

    while (curr != NULL) {
        if (curr->pid >= 0) { // only consider resident (valid) pages
            if (curr->counter < min_usage) {
                min_usage = curr->counter;
                evict = curr;
                oldest_last_access = curr->loadTime;
            } else if (curr->counter == min_usage) {
                // Tie-break by oldest last access time (LRU within LFU)
                if (curr->loadTime < oldest_last_access) {
                    evict = curr;
                    oldest_last_access = curr->loadTime;
                }
            }
        }
        curr = curr->next;
    }

    if (evict != NULL) {
        if (DEBUG == 1) {
            printf("EVICTED (LFU) :: p[%03d] c:%02d l:%02f tb:%f\n",
                   evict->pid, evict->counter, evict->loadTime, evict->timeBought);
        }
        evict->pid = -1;
        evict->page_num = -1;
    }
}
