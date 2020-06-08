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
#include <limits.h>

#include "rr.h"

/*
 * Sets the current process in a process table according to
 * Round-Robin scheduling.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * 
 * Returns int: Enumerated status code indicating if a valid next process
 *              is available.
 */
int rr_next_process(ProcTable *proc_table) {

    // Get process with longest elapsed time since last state change
    int min_tl = INT_MAX;

    for (int i = 0; i < proc_table->n_procs; i++) {

        if (proc_table->procs[i].status == READY || proc_table->procs[i].status == WAITING) {

            if (proc_table->procs[i].tl < min_tl) {

                // Track process with the lowest timestamp for last state change
                min_tl = proc_table->procs[i].tl;
                proc_table->current = i;

            } else if (proc_table->procs[i].tl == min_tl) {

                /* If two or more processes have the same elapsed time since their last
                 * state change, choose the one with the earliest arrival time */
                proc_table->current = proc_table->procs[proc_table->current].ta < proc_table->procs[i].ta
                                      ? i
                                      : proc_table->current;
            }
        }
    }

    return min_tl == INT_MAX ? FINISHED : READY;
}

int rr_run(ProcTable *proc_table, Memory *memory, int t) {

    // Shorthand variables
    Process *proc = &proc_table->procs[proc_table->current];
    int q = proc_table->quantum;

    // Start process
    if (proc->status != RUNNING) {
        simulate_start(proc_table, memory, t);
    } else if (!proc->tr) {
        simulate_finish(proc_table, memory, t);
    }

    // Use this clock cycle for memory loading
    if (proc->status == LOADING) return LOADING;

    // Boolean
    int in_quantum = t < (proc->tl + q);

    // Process is still running but out of quantum time
    if (proc->status == RUNNING && !in_quantum) pause_process(proc, t);

    if (proc->status == RUNNING) {
        // Reduce remaining time for the running process by one cycle
        proc->tr--;
        return RUNNING;
        
    } else {
        // Give CPU to next process if one is available
        return rr_next_process(proc_table) == FINISHED ? READY : rr_run(proc_table, memory, t);
    }
}
