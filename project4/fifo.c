#include "page_def.h"
#include <float.h>

void fifo(pages_list* pageList) {
    if (pageList == NULL || pageList->head == NULL) {
        return;
    }

    page* curr = pageList->head;
    page* evict = NULL;
    float oldest_time = FLT_MAX;

    while (curr != NULL) {
        if (curr->pid >= 0) {
            if (curr->timeBought < oldest_time) {
                oldest_time = curr->timeBought;
                evict = curr;
            }
        }
        curr = curr->next;
    }

    if (evict != NULL) {
        if (DEBUG == 1) {
            printf("EVICTED (FIFO) :: p[%03d] c:%02d l:%02f tb:%f\n", evict->pid, evict->counter, evict->loadTime, evict->timeBought);
        }
        evict->pid = -1;
        evict->page_num = -1;
    }
}
