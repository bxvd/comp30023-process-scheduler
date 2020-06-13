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

/*
 * qsort comparison function for process arrival time.
 */
int compare(const void *a, const void *b) {

    const Process *p1 = a, *p2 = b;
    
    if (p1->time.arrived < p2->time.arrived) return -1;
    if (p1->time.arrived == p2->time.arrived) {
        if (p1->id < p2->id) return -1;
        if (p1->id == p2->id) return 0;
    }
    return 1;
}

/*
 * Comparison function for qsort that compares integer values.
 */
int compare_int(const void *a, const void *b) {

    return *((int*)a) == *((int*)b) ? 0 : *((int*)a) < *((int*)b) ? -1 : 1;
}

/*
 * Creates and allocated memory for a new process with 
 * initialised values.
 * 
 * int id:
 * int mem:
 * int t_arrived: The time that the process arrived.
 * int t_job:     Total CPU time required to run the process.
 * 
 * Returns Process*: Pointer to the new Process struct.
 */
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
            sys->status = sys->status == TERMINATED ? READY : sys->status;
        }
    }
}

/*
 * Begins running the process in the current context and evitcts
 * memory to allow it to run.
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
        case CM: smallswap(sys); break;
        default: break;
    }

    p->time.started = p->time.last = sys->time;
    p->status = RUNNING;

    notify(RUN, *sys, 0);

    sys->time += p->time.load;
}

/*
 * Pauses the currently running process.
 * 
 * System *sys: Pointer to an OS struct.
 */
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

/*
 * Resumes a paused process.
 * 
 * System *sys: Pointer to an OS struct.
 */
void process_resume(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Handle memory
    switch (sys->allocator) {

        case SWP: swap(sys); break;
        case V: virtual(sys); break;
        case CM: smallswap(sys); break;
        default: break;
    }

    p->time.started = p->time.last = sys->time;
    p->status = RUNNING;

    notify(RUN, *sys, 0);

    sys->time += p->time.load;
}

/*
 * Performs termination of a process and evicts its memory.
 * 
 * System *sys: Pointer to an OS struct.
 */
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

/* Determines whether to keep the system running, i.e.
 * if not all processes have been received.
 * 
 * System sys: An OS struct.
 * 
 * Returns int: Evaluates to true if system should be kept alive,
 *              false otherwise.
 */
int keep_alive(System sys) {

    // Check status flags of processes
    for (int i = 0; i < sys.table.n; i++) {
        if (sys.table.p[i].status != TERMINATED) return 1;
    }

    return 0;
}

/*
 * Begins running and handles all dispatch to continue running
 * the OS.
 * 
 * Process *p:  Pointer to array of Processes for the process table.
 * int n:       Number of processes.
 * Scheduler s: Enumerated value for which scheduling algorithm to use.
 * Allocator a: Enumerated value for which memory allocation algorithm to use.
 * int m:       System memory size.
 * int q:       Quantam time for scheduling.
 * 
 * Returns System*: Pointer to the OS struct in its final state.
 */
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

    // We are go for launch
    sys->status = READY;

    // Cycle clock until all processes have been terminated
    while (sys->status != TERMINATED || keep_alive(*sys)) {

        // Check if any processes are ready
        get_processes(sys);

        if (sys->status == TERMINATED) sys->time++;

        switch (sys->scheduler) {
            case FF: ff_step(sys); break;
            case RR: rr_step(sys); break;
            case CS: cs_step(sys); break;
            default: break;
        }
    }

    free(sys->pages);

    return sys;
}
