/*
 * rr.c
 * 
 * A Round-Robin process scheduling algorithm to run for a
 * process scheduling simulator. Written for project 2 of
 * COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */

#include "rr.h"

/*
 * Begins running the process in the current context.
 * 
 * System *sys: Pointer to an OS.
 */
void rr_start_process(System *sys) {

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

void rr_finish_process(System *sys) {

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
void rr_step(System *sys) {

    switch (sys->status) {

        // New process
        case READY:

            // Update current context, or stop running if no processes available
            if (context(sys) == TERMINATED) {
                sys->status = TERMINATED;
                break;
            } 

            rr_start_process(sys);

            sys->status = RUNNING;

            break;
        
        case RUNNING:

            // Only one process will run at a time during FF scheduling
            sys->time += sys->table.p[sys->table.context].time.job;

            rr_finish_process(sys);

            sys->status = READY;

            break;
        
        default: break;
    }
    
    // Load process
    // Begin process

    // Run process

    // Unload process
    // Finish process

    // Next process
    
}
