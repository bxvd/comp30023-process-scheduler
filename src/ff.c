/*
 * ff.c
 * 
 * A First-Come-First-Served process scheduling algorithm
 * to run for a process scheduling simulator. Written for
 * project 2 of COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */

#include "ff.h"

/*
 * Updates the current context in the process table.
 * 
 * System *sys: Pointer to an OS.
 * 
 * returns Status: Enumerated status flag.
 */
Status ff_context(System *sys) {
    
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
 * Begins running the process in the current context.
 * 
 * System *sys: Pointer to an OS.
 */
void ff_start(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Handle memory
    switch (sys->allocator) {
        default: break;
    }

    p->time.started = p->time.last = sys->time;

    p->status = RUNNING;

    notify(RUN, *sys);
}

void ff_finish(System *sys) {

    Process *p = &sys->table.p[sys->table.context];

    p->time.remaining = 0;
    p->time.finished = p->time.last = sys->time;

    p->status = TERMINATED;

    sys->table.n_alive--;

    notify(FINISH, *sys);
}

/*
 * Handles a clock cycle for the OS.
 * 
 * System *sys: Pointer to the OS.
 */
void ff_step(System *sys) {

    switch (sys->status) {

        // New process
        case READY:

            // Update current context, or stop running if no processes available
            if (ff_context(sys) == TERMINATED) {
                sys->status = TERMINATED;
                break;
            } 

            ff_start(sys);

            sys->status = RUNNING;

            break;
        
        case RUNNING:

            // Only one process will run at a time during FF scheduling
            sys->time += sys->table.p[sys->table.context].time.job;

            ff_finish(sys);

            sys->status = READY;

            break;
        
        default: break;
    }
}
