#include "page_def.h"
#include <float.h>

void mfu(pages_list* pageList) {
    if (pageList == NULL || pageList->head == NULL) {
        return;
    }

    page* curr = pageList->head;
    page* evict = NULL;
    int max_usage = -1;   // Track the most frequently used page

    while (curr != NULL) {
        if (curr->pid >= 0) { // Valid page in memory
            if (curr->counter > max_usage) {
                max_usage = curr->counter;
                evict = curr;
            }
        }
        curr = curr->next;
    }

    if (evict != NULL) {
        if (DEBUG == 1) {
            printf("EVICTED (MFU) :: p[%03d] c:%02d l:%02f tb:%f\n",
                   evict->pid, evict->counter, evict->loadTime, evict->timeBought);
        }
        evict->pid = -1;
        evict->page_num = -1;
    }
}
