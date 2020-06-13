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

/*
 * Comparison function for qsort that compares time last allocated
 * for processes.
 */
int compare_last(const void *a, const void *b) {

    const Process *p1 = a, *p2 = b;
    
    if (p1->time.last < p2->time.last) return -1;
    if (p1->time.last == p2->time.last) {
        if (p1->id < p2->id) return -1;
        if (p1->id == p2->id) return 0;
    }
    return 1;
}

/*
 * Allocates memory for an array of Page structures
 * representing the memory in the scheduling simulator.
 * 
 * int size:      Total size in KB of the memory.
 * int page_size: Size in KB of each page.
 * 
 * Returns Page*: Pointer to new array of Pages.
 */
Page *create_memory(int size, int page_size) {

    Page *m = (Page*)calloc(1, (size / page_size) * sizeof(Page));

    for (int i = 0; i < (size / page_size); i++) {
        m[i].pid = m[i].pix = UNDEF;
    }

    return m;
}

/*
 * Allocates pages to the process in the current context.
 * Only allocates pages that are free, does not create
 * free pages.
 * 
 * System *sys: Pointer to an OS struct.
 * int target:  Target number of pages to allocated to the process.
 */
void allocate(System *sys, int target) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    for (int i = 0; (p->n_pages < target) && (i < sys->n_pages); i++) {
        if (sys->pages[i].pid == UNDEF) {

            // Update OS struct to reflect changes
            sys->pages[i].pid = p->id;
            sys->pages[i].pix = sys->table.context;
            p->n_pages++;
            p->time.load += PAGE_LOAD_TIME;
        }
    }
}

/*
 * Evicts pages currently allocated to a process.
 * 
 * System *sys: Pointer to an OS struct.
 * int pid:     Process ID to evict pages for.
 * int n:       Number of pages to evict.
 */
void evict_process(System *sys, int pid, int n) {

    // Track successfully evicted pages
    int n_evicted = 0, *evicted = (int*)calloc(1, n * sizeof(int));

    for (int i = 0; i < sys->n_pages && (n_evicted < n); i++) {
        if (sys->pages[i].pid == pid) {

            // Update OS struct to reflect changes
            sys->table.p[sys->pages[i].pix].n_pages--;
            sys->pages[i].pid = UNDEF;
            sys->pages[i].pix = UNDEF;
            evicted[n_evicted] = i;
            n_evicted++;
        }
    }

    notify(EVICT, *sys, 2, evicted, n_evicted);
}

/*
 * Evicts pages from memory irregardless of which process they belong to.
 * 
 * System *sys: Pointer to an OS struct.
 * int *pages:  Array of memory addresses to evict from.
 * int n:       Number of pages to evict.
 */
void evict_pages(System *sys, int *pages, int n) {

    // Track successfully evicted pages
    int n_evicted = 0, *evicted = (int*)calloc(1, n * sizeof(int));
    Page *page = NULL;

    for (int i = 0; i < n; i++) {

        // Current page in loop
        page = &sys->pages[pages[i]];

        // Update OS struct to reflect changes
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
 * System sys: An OS struct.
 * 
 * Returns int: index in the process table for the oldest process.
 */
int oldest(System sys) {

    // Shorthand
    Process *p = sys.table.p;

    int candidate = UNDEF;

    // Set context to be the least recently executed or received process
    for (int i = 0; i < sys.table.n; i++) {

        if (candidate == UNDEF && p[i].n_pages && p[i].status != LOADING) {
            candidate = i;
            continue;
        }

        // If this candidate is current and has pages allocated
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

/*
 * Performs memory swaps based on the Swapping-X algorithm.
 * Chooses the oldest allocated processes and evicts all of
 * their pages as necessary, allocating them to the current
 * context.
 * 
 * System *sys: Pointer to an OS struct.
 */
void swap(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    int candidate, target = (p->mem / sys->page_size) - p->n_pages;

    p->time.load = 0;
    p->status = LOADING;

    while (p->n_pages < target) {

        allocate(sys, target);

        if (p->n_pages < target) {
            
            candidate = oldest(*sys);
            evict_process(sys, sys->table.p[candidate].id, sys->table.p[candidate].n_pages);
        }
    }
}

/*
 * Performs memory swaps based on the virtual memory algorithm.
 * Chooses the oldest allocated processes and evicts only the
 * number of pages needed to allocated to and begin running the
 * current context.
 * 
 * System *sys: Pointer to an OS struct.
 */
void virtual(System *sys) {

    int *candidates = NULL, n_candidates = 0, target;

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Copy of processes sorted by time last allocated
    Process *sorted = (Process*)malloc(sys->table.n * sizeof(Process));
    memmove(sorted, sys->table.p, sys->table.n * sizeof(Process));
    qsort(sorted, sys->table.n, sizeof(Process), compare_last);

    p->time.load = 0;

    candidates = (int*)calloc(1, (p->mem / sys->page_size) * sizeof(int));

    target = min((p->mem / sys->page_size), MIN_PAGES);

    while (p->n_pages < target) {
        
        // Attempt to allocate the whole process to memory
        allocate(sys, p->mem / sys->page_size);

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
    p->time.remaining += (p->mem / sys->page_size) - p->n_pages;

    free(candidates);
    free(sorted);
}
