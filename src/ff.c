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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ff.h"

/*
 * Handles starting the process and printing output for the
 * FF scheduling algorithm.
 * 
 * ProcTable *proc_table: Pointer to process table.
 * Memory *memory:        Pointer to memory struct.
 * int t:                 Time.
 */
void ff_start(ProcTable *proc_table, Memory *memory, int t) {

    // Shorthand variable
    Process *proc = &proc_table->procs[proc_table->current];

    // Process is about to begin loading
    if (proc->status != LOADING) fprintf(stdout, "%d, RUNNING, id=%d, remaining-time=%d", t, proc->id, proc->tr);

    start_process(proc_table, memory, t);

    // Process has finished loading during this cycle
    if (proc->status == RUNNING) {
        
        // Check if memory allocation is happening
        if (proc->n_pages) {

            fprintf(stdout,
                    ", load-time=%d, mem-usage=%d%%, mem-addresses=[",
                    proc->tm,
                    ceil(proc->n_pages / (memory->size / memory->page_size)) * 100);

            // Print process' memory addresses
            for (int i = 0; i < proc->n_pages - 1; i++) {
                fprintf(stdout, "%d,", proc->pages[i]);
            }
            fprintf(stdout, "%d]\n", proc->pages[proc->n_pages - 1]);
        } else {
            fprintf(stdout, "\n");
        }
    }
}

/*
 * Handles finishing the process and printing output for the
 * FF scheduling algorithm.
 * 
 * ProcTable *proc_table: Pointer to process table.
 * Memory *memory:        Pointer to memory struct.
 * int t:                 Time.
 */
void ff_finish(ProcTable *proc_table, Memory *memory, int t) {

    // Shorthand variable
    Process *proc = &proc_table->procs[proc_table->current];

    // Check if memory allocation is happening
    if (proc->n_pages) {

        fprintf(stdout, "%d, EVICTED, mem-addresses=[", t);

        for (int i = 0; i < proc->n_pages - 1; i++) {
            fprintf(stdout, "%d,", proc->pages[i]);
        }
        fprintf(stdout, "%d]\n", proc->pages[proc->n_pages - 1]);
    }

    finish_process(proc_table, memory, t);

    proc_table->current++;

    fprintf(stdout, "%d, FINISHED, id=%d, proc-remaining=%d\n", t, proc->id, proc_table->n_procs - proc_table->current);
}

/*
 * Sets the current process in a process table according to
 * First-Come-First-Served scheduling.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * 
 * Returns int: Enumerated status code indicating if a valid next process
 *              is available.
 */
int ff_next_process(ProcTable *proc_table) {

    if (proc_table->current < proc_table->n_procs - 1) {

        proc_table->current++;
        return READY;
    }

    return FINISHED;
}

/*
 * First-Come-Fisrt-Served process scheduling algorithm.
 * 
 * ProcTable *proc_table: Pointer to process table.
 * Memory *memory:        Pointer to memory struct.
 * int t:                 Time.
 * 
 * Returns int: Enumerated status flag.
 */
int ff_run(ProcTable *proc_table, Memory *memory, int t) {

    // Shorthand variable
    Process *proc = &proc_table->procs[proc_table->current];

    if (proc->status != RUNNING) {
        simulate_start(proc_table, memory, t);
    } else if (!proc->tr) {
        simulate_finish(proc_table, memory, t);
    }

    // Use this clock cycle for memory loading
    if (proc->status == LOADING) return LOADING;
    
    if (proc->status == RUNNING) {
        // Reduce remaining time for the running process by one cycle
        proc->tr--;
        return RUNNING;

    } else {
        // Give CPU to next process if one is available
        return ff_next_process(proc_table) == FINISHED ? READY : ff_run(proc_table, memory, t);
    }
}
