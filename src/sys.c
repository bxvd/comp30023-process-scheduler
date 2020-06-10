/*
 * sys.c
 * 
 * An operating system consisting of only process scheduling and
 * memory allocation procedures. Written for project 2 of
 * COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */ 

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "sys.h"

int compare(const void *a, const void *b) {

    const Process *p1 = a, *p2 = b;
    
    if (p1->time.arrived < p2->time.arrived) return -1;
    if (p1->time.arrived == p2->time.arrived) {
        if (p1->id < p2->id) return -1;
        if (p1->id == p2->id) return 0;
    }
    return 1;
}

Process *create_process(int id, int mem, int t_arrived, int t_job) {

    Process *p = (Process*)calloc(1, sizeof(Process));
    
    p->status = INIT;
    p->id = id;

    p->mem = mem;
    p->n_pages = mem / PAGE_SIZE;
    p->pages = (Page**)calloc(1, p->n_pages * sizeof(void*));

    p->time.arrived = t_arrived;
    p->time.job = p->time.remaining = t_job;
    p->time.last = p->time.started = p->time.finished = UNDEF;

    return p;
}

PTable *create_table(Process *p, int n) {

    // Sort processes in increasing order of arrival time and ID
    qsort(p, n, sizeof(Process), compare);

    PTable *table = (PTable*)calloc(1, sizeof(PTable));

    table->status = INIT;
    table->p = p;
    table->n = table->n_alive = n;

    return table;
}

/*
 * Receives a newly arrived process.
 * 
 * Process *p: Pointer to process.
 * int t:      Time.
 */
void activate(Process *p, int t) {
    p->status = START;
    p->time.last = t;
}

/*
 * Checks for newly arrived processes.
 * 
 * System *sys: Pointer to the OS.
 */
void get_processes(System *sys) {

    // Shorthand
    Process *p = sys->table.p;

    for (int i = 0; i < sys->table.n; i++) {
        if (p[i].time.arrived == sys->time) {
            activate(&p[i], sys->time);
        }
    }
}

/*
 * Updates the current context in the process table.
 * 
 * System *sys: Pointer to an OS.
 * 
 * returns Status: Enumerated status flag.
 */
Status context(System *sys) {
    
    // Shorthand
    Process *p = sys->table.p;

    // Set context to be the next immediately available process
    for (int i = 0; i < sys->table.n; i++) {
        if (p[i].status == INIT || p[i].status == READY) {
            sys->table.context = i;
            return READY;
        }
    }

    // No valid process found
    return TERMINATED;
}

System *start(Process *p, int n, Scheduler s, Allocator a, int m, int q) {

    /* DEBUG */
    if (1) {
        char *scheduler, *allocator;

        switch (s) {
            case FF: scheduler = "First-Come-First-Served"; break;
            case RR: scheduler = "Round-Robin"; break;
            case CS: scheduler = "Custom"; break;
        }

        switch (a) {
            case U: allocator = "Unlimited"; break;
            case SWP: allocator = "Swapping"; break;
            case V: allocator = "Virtual"; break;
            case CM: allocator = "Custom"; break;
        }

        fprintf(stderr,
                "Scheduler: %s\nAllocator: %s\nMem size: %d\nQuantum: %d\nProcesses: %d\n",
                scheduler, allocator, m, q, n);

        for (int i = 0; i < n; i++) {
            fprintf(stderr, "%d, %d, %d, %d\n", p[i].id, p[i].time.arrived, p[i].time.job, p[i].mem);
        }
    }
    
    System *sys = (System*)calloc(1, sizeof(System));

    // Setup process table
    PTable *table = create_table(p, n);
    memmove(&sys->table, table, sizeof(PTable));
    free(table);

    // Setup system variables
    sys->scheduler = s;
    sys->allocator = a;
    sys->time = 0;

    // We are go for launch
    sys->status = READY;

    // Cycle clock until all processes have been terminated
    while (sys->status != TERMINATED) {
        ff_step(sys);
    }

    return sys;
}
