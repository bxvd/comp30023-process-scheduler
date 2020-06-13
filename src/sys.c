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
    p->n_pages = 0;
    p->pages = (Page**)calloc(1, (mem / PAGE_SIZE) * sizeof(void*));

    p->time.arrived = t_arrived;
    p->time.job = p->time.remaining = t_job;
    p->time.last = p->time.started = p->time.finished = UNDEF;
    p->time.load = 0;

    return p;
}

PTable *create_table(Process *p, int n) {

    // Sort processes in increasing order of arrival time and ID
    qsort(p, n, sizeof(Process), compare);

    PTable *table = (PTable*)calloc(1, sizeof(PTable));

    table->status = INIT;
    table->context = UNDEF;
    table->p = p;
    table->n = n;
    table->n_alive = 0;

    return table;
}

/*
 * Receives a newly arrived process.
 * 
 * Process *p: Pointer to process.
 */
void activate(Process *p) {

    p->status = START;
    p->time.last = p->time.arrived;
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
        if (p[i].status == INIT && p[i].time.arrived <= sys->time) {
            activate(&p[i]);
            sys->table.n_alive++;
        }
    }
}

/*
 * Begins running the process in the current context.
 * 
 * System *sys: Pointer to an OS.
 */
void process_start(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Handle memory
    switch (sys->allocator) {

        case SWP: swap(sys); break;
        case V: virtual(sys); break;
        case CM: break;
        default: break;
    }

    p->time.started = p->time.last = sys->time;
    p->status = RUNNING;

    notify(RUN, *sys, 0);

    sys->time += p->time.load;
}

void process_pause(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Reduce time remaining by time spent processing
    p->time.remaining -= sys->time - p->time.last - p->time.load;
    p->time.last = sys->time;

    p->status = READY;

    // Check if any new processes have arrived
    get_processes(sys);
}

void process_resume(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Handle memory
    switch (sys->allocator) {

        case SWP: swap(sys); break;
        case V: virtual(sys); break;
        case CM: break;
        default: break;
    }

    p->time.started = p->time.last = sys->time;
    p->status = RUNNING;

    notify(RUN, *sys, 0);

    sys->time += p->time.load;
}

void process_finish(System *sys) {

    Process *p = &sys->table.p[sys->table.context];

    p->time.remaining = 0;
    p->time.finished = p->time.last = sys->time;
    p->status = TERMINATED;

    sys->table.n_alive--;

    if (sys->allocator != U) evict_process(sys, p->id, p->n_pages);

    // Check if any new processes have arrived
    get_processes(sys);

    notify(FINISH, *sys, 0);
}

System *start(Process *p, int n, Scheduler s, Allocator a, int m, int q) {

    System *sys = (System*)calloc(1, sizeof(System));

    // Setup process table
    PTable *table = create_table(p, n);
    memmove(&sys->table, table, sizeof(PTable));
    free(table);

    // Setup memory
    sys->pages = create_memory(m, PAGE_SIZE);

    // Setup system variables
    sys->scheduler = s;
    sys->allocator = a;
    sys->quantum = q == UNDEF ? DEFAULT_QUANTUM : q;
    sys->mem_size = m;
    sys->page_size = PAGE_SIZE;
    sys->n_pages = m / PAGE_SIZE;
    sys->time = 0;

    // Check if any processes are ready
    get_processes(sys);

    // We are go for launch
    sys->status = READY;

    // Cycle clock until all processes have been terminated
    while (sys->status != TERMINATED) {

        switch (sys->scheduler) {
            case FF: ff_step(sys); break;
            case RR: rr_step(sys); break;
            case CS: break;
            default: break;
        }
    }

    free(sys->pages);

    return sys;
}
