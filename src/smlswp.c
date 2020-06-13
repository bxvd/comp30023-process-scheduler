/*
 * smlswp.c
 * 
 * A set of methods for performing memory swapping prioritising
 * smaller footprint processes. Written for project 2 of
 * COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */

#include <stdlib.h>
#include <string.h>

#include "smlswp.h"

/*
 * qsort comparison function for process memory size.
 */
int compare_size(const void *a, const void *b) {

    const Process *p1 = a, *p2 = b;

    if (p1->mem < p2->mem) return 1;
    if (p1->mem == p2->mem) {
        if (p1->id < p2->id) return 1;
        if (p1->id == p2->id) return 0;
    }
    return -1;
}

/*
 * Performs memory swapping with the goal of keeping the entirety of
 * smaller processes in memory and only forcing the largest processes
 * to use virtual memory.
 * 
 * System *sys: Pointer to an OS struct.
 */
void smallswap(System *sys) {

    int *candidates = NULL, n_candidates = 0, target = 0;

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Copy of processes sorted by size
    Process *sorted = (Process*)malloc(sys->table.n * sizeof(Process));
    memmove(sorted, sys->table.p, sys->table.n * sizeof(Process));
    qsort(sorted, sys->table.n, sizeof(Process), compare_size);

    // Get surplus page count
    for (int i = 0; i < sys->table.n && target < (p->mem / sys->page_size); i++) {

        if (sorted[i].status != READY) continue;

        // Only count processes larger than the one in context
        if (sorted[i].id == p->id) break;

        target += sorted[i].n_pages - MIN_PAGES;
    }

    p->time.load = 0;

    candidates = (int*)calloc(1, (p->mem / sys->page_size) * sizeof(int));

    target = min(max(target, MIN_PAGES), (p->mem / sys->page_size));


    while (p->n_pages < target) {
        
        // Attempt to allocate the whole process to memory
        allocate(sys, p->mem / sys->page_size);

        if (p->n_pages < target) {
            
            // Iterate on sorted array
            for (int i = 0; i < sys->table.n; i++) {

                // Iterate on pages
                for (int j = 0; j < sys->n_pages && n_candidates < (target - p->n_pages); j++) {
                    if (sys->pages[j].pid == sorted[i].id && sys->pages[j].pid != p->id) {
                        candidates[n_candidates] = j;
                        n_candidates++;
                    }
                }
            }

            // Sort in increasing order of memory address
            qsort(candidates, n_candidates, sizeof(int), compare_int);

            evict_pages(sys, candidates, n_candidates);
        }
    }

    // Increase remaining time for page fault
    p->time.remaining += (p->mem / PAGE_SIZE) - p->n_pages;

    free(candidates);
    free(sorted);
}
