/*
 * mem.c
 * 
 * A set of methods and data structures for handling memory
 * allocation for an operating system. Written for project 2
 * of COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */

#include <stdlib.h>
#include <string.h>

#include "mem.h"

int compare_last(const void *a, const void *b) {

    const Process *p1 = a, *p2 = b;
    
    if (p1->time.last < p2->time.last) return -1;
    if (p1->time.last == p2->time.last) {
        if (p1->id < p2->id) return -1;
        if (p1->id == p2->id) return 0;
    }
    return 1;
}

int compare_int(const void *a, const void *b) {

    return *((int*)a) == *((int*)b) ? 0 : *((int*)a) < *((int*)b) ? -1 : 1;
}

Page *create_memory(int size, int page_size) {

    Page *m = (Page*)calloc(1, (size / page_size) * sizeof(Page));

    for (int i = 0; i < (size / page_size); i++) {
        m[i].pid = m[i].pix = UNDEF;
    }

    return m;
}

void allocate(System *sys, int target) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    for (int i = 0; (p->n_pages < target) && (i < sys->n_pages); i++) {
        if (sys->pages[i].pid == UNDEF) {
            sys->pages[i].pid = p->id;
            sys->pages[i].pix = sys->table.context;
            p->n_pages++;
            p->time.load += PAGE_LOAD_TIME;
        }
    }

    //fprintf(stderr, "Allocated %d\n", p->n_pages);
}

void evict_process(System *sys, int pid, int n) {

    int n_evicted = 0, *evicted = (int*)calloc(1, n * sizeof(int));

    for (int i = 0; i < sys->n_pages && (n_evicted < n); i++) {
        if (sys->pages[i].pid == pid) {

            sys->table.p[sys->pages[i].pix].n_pages--;
            sys->pages[i].pid = UNDEF;
            sys->pages[i].pix = UNDEF;
            evicted[n_evicted] = i;
            n_evicted++;
        }
    }

    notify(EVICT, *sys, 2, evicted, n_evicted);
}

void evict_pages(System *sys, int *pages, int n) {

    int n_evicted = 0, *evicted = (int*)calloc(1, n * sizeof(int));
    Page *page = NULL;

    for (int i = 0; i < n; i++) {

        // Current page in loop
        page = &sys->pages[pages[i]];

        sys->table.p[page->pix].n_pages--;
        page->pid = UNDEF;
        page->pix = UNDEF;
        evicted[n_evicted] = pages[i];
        n_evicted++;
    }

    notify(EVICT, *sys, 2, evicted, n_evicted);
}

/*
 * Finds the least recently allocated process in the
 * process table.
 * 
 * System sys: OS data structure.
 * 
 * Returns int: index in the process table for the oldest process.
 */
int oldest(System sys) {

    // Shorthand
    Process *p = sys.table.p;

    int candidate = UNDEF;

    // Set context to be the least recently executed or received process
    for (int i = 0; i < sys.table.n; i++) {

        if (candidate == UNDEF && p[i].n_pages) {
            candidate = i;
            continue;
        }

        if ((p[i].status == START || p[i].status == READY) && p[i].n_pages) {
            
            if (p[i].time.last < p[candidate].time.last) {
                candidate = i;
            } else if (p[i].time.last == p[candidate].time.last) {
                if (p[i].time.arrived > p[candidate].time.arrived) candidate = i;
            }
        }
    }

    return candidate;
}

void swap(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    int candidate, target = (p->mem / sys->page_size) - p->n_pages;

    p->time.load = 0;

    while (p->n_pages < target) {

        allocate(sys, target);

        if (p->n_pages < target) {
            
            candidate = oldest(*sys);
            evict_process(sys, sys->table.p[candidate].id, sys->table.p[candidate].n_pages);
        }
    }
}

void virtual(System *sys) {

    int *candidates = NULL, n_candidates = 0, target = MIN_PAGES;

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Copy of processes sorted by time last allocated
    Process *sorted = (Process*)malloc(sys->table.n * sizeof(Process));
    memmove(sorted, sys->table.p, sys->table.n * sizeof(Process));
    qsort(sorted, sys->table.n, sizeof(Process), compare_last);

    p->time.load = 0;

    candidates = (int*)calloc(1, (p->mem / sys->page_size) * sizeof(int));

    while (p->n_pages < target) {
    
        // Attempt to allocate the whole process to memory
        allocate(sys, p->mem / PAGE_SIZE);

        if (p->n_pages < target) {
            // Find evictable pages in sorted array
            for (int i = 0; i < sys->table.n; i++) {
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
