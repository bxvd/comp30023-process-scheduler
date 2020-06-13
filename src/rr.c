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

    // Ensure we're not including terminated processes
    if (p[sys->table.context].status == TERMINATED) {
        for (int i = 0; i < sys->table.n; i++) {
            if (p[i].status != TERMINATED) {
                sys->table.context = i;
                break;
            }
        }
    }

    // Set context to be the least recently executed or received process
    for (int i = 0; i < sys->table.n; i++) {
        if (p[i].status == START || p[i].status == READY) {
            
            if (p[i].time.last < p[sys->table.context].time.last && p[sys->table.context].status != TERMINATED) {
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
 * Handles a clock cycle for the OS according to
 * Round-Robin scheduling.
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
                process_start(sys);
            } else {
                process_resume(sys);
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
                process_pause(sys);
            } else {
                process_finish(sys);
            }

            sys->status = READY;

            break;
        
        default: break;
    }
}
