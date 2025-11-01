#include "page_def.h"
#include <time.h>
#include <stdarg.h>

#define OUTPUT_BUFFER_SIZE 5000000  // 5MB buffer per algorithm

// Structure to store output for each algorithm
typedef struct {
    char* buffer;
    size_t size;
    size_t pos;
    char* name;
    int swapProcessing;
    int total_page_references;
    int total_hits;
    int total_misses;
} algo_output;

// Function to append formatted string to buffer
void append_to_buffer(algo_output* out, const char* format, ...) {
    if (out->pos >= out->size - 1) {
        // Buffer full, don't append
        return;
    }
    
    va_list args;
    va_start(args, format);
    int written = vsnprintf(out->buffer + out->pos, out->size - out->pos, format, args);
    va_end(args);
    
    if (written > 0 && (size_t)written < (out->size - out->pos)) {
        out->pos += written;
    }
}

// Global variables for statistics
int total_page_references = 0;
int total_hits = 0;
int total_misses = 0;

int main() {

    int timeStamp = 0;                      
    int *pageCountOPT = malloc(sizeof(int)*4); 
    pageCountOPT[0] = 5;
    pageCountOPT[1] = 11;
    pageCountOPT[2] = 17;
    pageCountOPT[3] = 31;

    page *pagePointer;

    // Array of all algorithms
    struct {
        char* name;
        void (*func)(pages_list*);
    } algorithms[] = {
        {"FIFO", fifo},
        {"LRU", lru},
        {"LFU", lfu},
        {"MFU", mfu},
        {"Random", random_algo}
    };
    
    srand(time(NULL));
    
    // Allocate output buffers for all algorithms
    algo_output algo_outputs[5];
    int algo_idx;
    for(algo_idx = 0; algo_idx < 5; algo_idx++) {
        algo_outputs[algo_idx].buffer = malloc(OUTPUT_BUFFER_SIZE);
        algo_outputs[algo_idx].size = OUTPUT_BUFFER_SIZE;
        algo_outputs[algo_idx].pos = 0;
        algo_outputs[algo_idx].name = algorithms[algo_idx].name;
        algo_outputs[algo_idx].buffer[0] = '\0';
        algo_outputs[algo_idx].swapProcessing = 0;
        algo_outputs[algo_idx].total_page_references = 0;
        algo_outputs[algo_idx].total_hits = 0;
        algo_outputs[algo_idx].total_misses = 0;
    }
    
    // Run each algorithm and store output
    for(algo_idx = 0; algo_idx < 5; algo_idx++) {
        void (*algoFunct)(pages_list*) = algorithms[algo_idx].func;
        char* algo_name = algorithms[algo_idx].name;
        algo_output* current_output = &algo_outputs[algo_idx];
        
        // Reset statistics per algorithm
        total_page_references = 0;
        total_hits = 0;
        total_misses = 0;
        int swapProcessing = 0;
        
        append_to_buffer(current_output, "\n\n########################################################################\n");
        append_to_buffer(current_output, "########## Running %s Algorithm ##########\n", algo_name);
        append_to_buffer(current_output, "########################################################################\n\n");

        pages_list pl;
        initialize_pages(&pl);
        process Q[TOTAL_PROCESS];
        int i;
        for(i=0;i<TOTAL_PROCESS;i++) {
            Q[i].pid = i;
            Q[i].pageCounter = pageCountOPT[rand()%4];
            Q[i].arrivalTime = rand()%60;
            Q[i].serviceDuration = (rand() % PROCESS_DURATION) + 1; 
            Q[i].pageReference = 0; 
        }
        qsort(Q,TOTAL_PROCESS,sizeof(process),compare_arrival);

        int index = 0; 
        for(timeStamp = 0; timeStamp < TOTAL_DURATION; timeStamp++) {

            while(index < TOTAL_PROCESS && Q[index].arrivalTime <= timeStamp) {

                
                if(pagesWhichAreFree(&pl,4)) {

                    page* p = pageFree(&pl);
                    p->pid = Q[index].pid;
                    p->page_num = Q[index].pageReference;
                    p->timeBought = 1.0*timeStamp;
                    p->counter = 1;
                    p->loadTime = timeStamp;
                    
                    // Print process entry record as required by assignment
                    char process_name[2];
                    process_name[0] = 'A' + (Q[index].pid % 26);
                    process_name[1] = '\0';
                    // Store to buffer - create the output string manually
                    char mem_map[101];
                    page* p_map = pl.head;
                    int map_idx = 0;
                    while(p_map && map_idx < 100) {
                        mem_map[map_idx++] = (p_map->pid >= 0) ? ('A' + (p_map->pid % 26)) : '.';
                        p_map = p_map->next;
                    }
                    mem_map[map_idx] = '\0';
                    append_to_buffer(current_output, "<%d, %s, Enter, %d pages, %d seconds, Memory Map: %s\n>\n", 
                                     timeStamp, process_name, Q[index].pageCounter, Q[index].serviceDuration, mem_map);
                    
                    swapProcessing++;
                    index++;
                } else break;
            }
            int i;
            
            for(i=0;i<10;i++) {
            
                int j;
                for(j=0;j<index;j++) if(Q[j].serviceDuration > 0) {
                    Q[j].pageReference = generate_next_pagenum(Q[j].pageReference,Q[j].pageCounter);
                    if(pagesInMemory(&pl,Q[j].pid,Q[j].pageReference)) {
                        // Page hit - update statistics
                        pagePointer = pageIDFree(&pl,Q[j].pid,Q[j].pageReference);
                        if (pagePointer == NULL){
                            append_to_buffer(current_output, "There is an error, got null : pid %d page:: %d\n", Q[j].pid, Q[j].pageReference);
                            return -1;
                        }

                        pagePointer->counter++;
                        pagePointer->loadTime = timeStamp;
                        
                        // Print page reference trace for first 100 references
                        if (total_page_references < 100) {
                            char proc_name = 'A' + (Q[j].pid % 26);
                            append_to_buffer(current_output, "<%.1f, %c, page-%d, HIT, evict X-page%d>\n", 
                                             timeStamp + (0.1 * i), proc_name, Q[j].pageReference, -1);
                        }
                        total_page_references++;
                        total_hits++;
                        continue;
                    }

                    

                    page* pge = pageFree(&pl);
                    int evicted_pid = -1;
                    int evicted_page = -1;
                    
                    if(!pge) {
                        // Find page to evict before eviction by simulating what the algorithm would choose
                        page* candidate = NULL;
                        if (algoFunct == fifo) {
                            float oldest = 1e9;
                            page* temp = pl.head;
                            while(temp) {
                                if(temp->pid >= 0 && temp->timeBought < oldest) {
                                    oldest = temp->timeBought;
                                    candidate = temp;
                                }
                                temp = temp->next;
                            }
                        } else if (algoFunct == lru) {
                            float oldest = 1e9;
                            page* temp = pl.head;
                            while(temp) {
                                if(temp->pid >= 0 && temp->loadTime < oldest) {
                                    oldest = temp->loadTime;
                                    candidate = temp;
                                }
                                temp = temp->next;
                            }
                        } else if (algoFunct == lfu) {
                            int min_count = 1e9;
                            float oldest = 1e9;
                            page* temp = pl.head;
                            while(temp) {
                                if(temp->pid >= 0) {
                                    if(temp->counter < min_count || (temp->counter == min_count && temp->loadTime < oldest)) {
                                        min_count = temp->counter;
                                        oldest = temp->loadTime;
                                        candidate = temp;
                                    }
                                }
                                temp = temp->next;
                            }
                        } else if (algoFunct == mfu) {
                            int max_count = -1;
                            page* temp = pl.head;
                            while(temp) {
                                if(temp->pid >= 0 && temp->counter > max_count) {
                                    max_count = temp->counter;
                                    candidate = temp;
                                }
                                temp = temp->next;
                            }
                        } else {
                            // Random - just pick first valid
                            candidate = pl.head;
                            while(candidate && candidate->pid < 0) candidate = candidate->next;
                        }
                        
                        if(candidate) {
                            evicted_pid = candidate->pid;
                            evicted_page = candidate->page_num;
                        }
                        
                        if(DEBUG) {
                            append_to_buffer(current_output, "---------------------------------------------------------------------\n");
                            append_to_buffer(current_output, "Memory is full, Page list:\n");
                            // Note: display() output would need to be captured separately if needed
                        }

                        algoFunct(&pl);
                        
                        if(DEBUG) {
                            // Note: display() output would need to be captured separately if needed
                        }

                        pge = pageFree(&pl);
                    }
                    pge->pid = Q[j].pid;
                    pge->page_num = Q[j].pageReference;
                    pge->timeBought = timeStamp+(0.1*i);
                    pge->loadTime = timeStamp+(0.1*i);
                    pge->counter = 1;
                    
                    // Print page reference trace for first 100 references
                    if (total_page_references < 100) {
                        char proc_name = 'A' + (Q[j].pid % 26);
                        char evict_name = (evicted_pid >= 0) ? ('A' + (evicted_pid % 26)) : 'X';
                        append_to_buffer(current_output, "<%.1f, %c, page-%d, MISS, evict %c-page%d>\n", 
                                         timeStamp + (0.1 * i), proc_name, Q[j].pageReference, evict_name, evicted_page);
                    }
                    total_page_references++;
                    total_misses++;
                    
                    append_to_buffer(current_output, "Page:: %d for process id:: %d brought in at:: %.1f\n",
                                     Q[j].pageReference, Q[j].pid, pge->timeBought);
                    swapProcessing++;
                }
            }
            int j;

            for(j=0;j<index;j++) if(Q[j].serviceDuration > 0) {
                Q[j].serviceDuration--;
                if(Q[j].serviceDuration == 0) {
                    // Print process exit record as required by assignment
                    char process_name[2];
                    process_name[0] = 'A' + (Q[j].pid % 26);
                    process_name[1] = '\0';
                    // Store to buffer
                    char mem_map[101];
                    page* p_map = pl.head;
                    int map_idx = 0;
                    while(p_map && map_idx < 100) {
                        mem_map[map_idx++] = (p_map->pid >= 0) ? ('A' + (p_map->pid % 26)) : '.';
                        p_map = p_map->next;
                    }
                    mem_map[map_idx] = '\0';
                    append_to_buffer(current_output, "<%d, %s, Exit, %d pages, 0 seconds, Memory Map: %s\n>\n", 
                                     timeStamp, process_name, Q[j].pageCounter, mem_map);
                    
                    if(DEBUG) {
                        append_to_buffer(current_output, "--------------------------------------------------------------------\n");
                        append_to_buffer(current_output, "Process id %d is done. Memory is getting free .... \n", Q[j].pid);
                        append_to_buffer(current_output, "---------------------------------------------------------------------\n");
                    }
                    free_mem(&pl,Q[j].pid);
                }
            }
            usleep(1000);
        }
        
        // Store statistics for this algorithm (but don't add to buffer yet)
        current_output->swapProcessing = swapProcessing;
        current_output->total_page_references = total_page_references;
        current_output->total_hits = total_hits;
        current_output->total_misses = total_misses;
    }

    // Print all algorithm outputs first
    printf("\n");
    printf("########################################################################\n");
    printf("########## ALL SIMULATION RESULTS ##########\n");
    printf("########################################################################\n\n");
    
    for(algo_idx = 0; algo_idx < 5; algo_idx++) {
        printf("%s", algo_outputs[algo_idx].buffer);
    }
    
    // Print all statistics at the end
    printf("\n\n");
    printf("########################################################################\n");
    printf("########## ALL ALGORITHM STATISTICS SUMMARY ##########\n");
    printf("########################################################################\n\n");
    
    for(algo_idx = 0; algo_idx < 5; algo_idx++) {
        algo_output* out = &algo_outputs[algo_idx];
        printf("========== %s Algorithm - Statistics ==========\n", out->name);
        printf("Number of processes that were successfully swapped in: %d\n", out->swapProcessing);
        if(out->total_page_references > 0) {
            double hit_ratio = (double)out->total_hits / out->total_page_references;
            double miss_ratio = (double)out->total_misses / out->total_page_references;
            printf("Total Page References: %d\n", out->total_page_references);
            printf("Hits: %d\n", out->total_hits);
            printf("Misses: %d\n", out->total_misses);
            printf("Hit Ratio: %.4f (%.2f%%)\n", hit_ratio, hit_ratio * 100);
            printf("Miss Ratio: %.4f (%.2f%%)\n", miss_ratio, miss_ratio * 100);
        }
        printf("================================================================\n\n");
        free(out->buffer);
    }
    
    printf("\n########## All simulations completed! ##########\n");
    
    free(pageCountOPT);
    return 0;
}
