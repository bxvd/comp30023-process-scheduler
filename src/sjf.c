/*
 * sjf.c
 * 
 * A Shortest-Job-First process scheduling algorithm to run
 * for a process scheduling simulator. Written for project
 * 2 of COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */
#include <stdlib.h>

#include "sjf.h"

/*
 * qsort comparison function for process job time.
 */
int compare_job(const void* a, const void* b) {

    const Process *p1 = a, *p2 = b;

    if (p1->time.job < p2->time.job) return -1;
    if (p1->time.job == p2->time.job) {
        if (p1->id < p2->id) return -1;
        if (p1->id == p2->id) return 0;
    }
    return 1;
}

/*
 * Updates the current context for the system based
 * on sorting of the process table.
 * 
 * System *sys: Pointer to an OS struct.
 * 
 * returns Status: Enumerated status flag.
 */
Status cs_context(System *sys) {
    
    // Shorthand
    Process *p = sys->table.p;

    // Set context to be the next immediately available process
    for (int i = 0; i < sys->table.n; i++) {
        if (p[i].status == START || p[i].status == READY) {
            sys->table.context = i;
            return READY;
        }
    }

    // No valid process found
    return TERMINATED;
}

/*
 * Handles a clock cycle for the OS according to
 * Shortest-Job-First scheduling.
 * 
 * System *sys: Pointer to the OS.
 */
void cs_step(System *sys) {

    // Sort processes by job time
    qsort(sys->table.p, sys->table.n, sizeof(Process), compare_job);
    
    switch (sys->status) {

        // New process
        case READY:

            // Update current context, or stop running if no processes available
            if (cs_context(sys) == TERMINATED) {
                sys->status = TERMINATED;
                break;
            } 

            process_start(sys);

            sys->status = RUNNING;

            break;
        
        case RUNNING:

            // Advance clock by job time as it will run to completion
            sys->time += sys->table.p[sys->table.context].time.job;

            process_finish(sys);

            sys->status = READY;

            break;
        
        default: break;
    }
}
