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
#include <stdio.h>
#include "rr.h"

/*
 * Updates the current context in the process table.
 * 
 * System *sys: Pointer to an OS.
 * 
 * returns Status: Enumerated status flag.
 */
Status rr_context(System *sys) {

    Status flag = TERMINATED;
    
    // Shorthand
    Process *p = sys->table.p;

    // Initialise to a value to compare with
    if (sys->table.context == UNDEF) {
        sys->table.context = 0;
        flag = READY;
    }

    // Set context to be the least recently executed or received process
    for (int i = 0; i < sys->table.n; i++) {
        if (p[i].status == START || p[i].status == READY) {
            
            if (p[i].time.last < p[sys->table.context].time.last) {
                sys->table.context = i;
            } else if (p[i].time.last == p[sys->table.context].time.last) {
                if (p[i].time.arrived > p[sys->table.context].time.arrived) sys->table.context = i;
            }

            flag = READY;
        }
    }

    // Flag if no valid process found
    return flag;
}

/*
 * Begins running the process in the current context.
 * 
 * System *sys: Pointer to an OS.
 */
void rr_start(System *sys) {

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

void rr_pause(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    // Reduce time remaining by time spent processing
    p->time.remaining -= sys->time - p->time.last;
    p->time.last = sys->time;

    p->status = READY;
}

void rr_resume(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    p->time.last = sys->time;

    p->status = RUNNING;

    notify(RUN, *sys);
}

void rr_finish(System *sys) {

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

    int runtime;

    switch (sys->status) {

        // New process
        case READY:

            // Update current context, or stop running if no processes available
            if (rr_context(sys) == TERMINATED) {
                sys->status = TERMINATED;
                break;
            } 

            if (sys->table.p[sys->table.context].status == START) {
                rr_start(sys);
            } else {
                rr_resume(sys);
            }

            sys->status = RUNNING;

            break;
        
        case RUNNING:
        
            // Run only for quantum time limit or time remaining
            
            runtime = sys->quantum > sys->table.p[sys->table.context].time.remaining ?
                      sys->table.p[sys->table.context].time.remaining :
                      sys->quantum;

            sys->time += runtime;
            
            // Check if process has finished
            if ((sys->table.p[sys->table.context].time.remaining - runtime)) {
                rr_pause(sys);
            } else {
                rr_finish(sys);
            }

            sys->status = READY;

            break;
        
        default: break;
    }
}
