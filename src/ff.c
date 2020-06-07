/*
 * ff.c
 * 
 * An First-Come-First-Served process scheduling algorithm
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

#define ceil(x) (x > (float)((int)x) ? (int)x + 1 : (int)x)

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
int ff_finish(ProcTable *proc_table, Memory *memory, int t) {

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

        ff_start(proc_table, memory, t);

    } else if (proc->tr == 0) {

        ff_finish(proc_table, memory, t);

        // Start next process if one is available
        if (proc_table->n_procs - proc_table->current) return ff_run(proc_table, memory, t);
    }

    // Reduce remaining time for the running process by one cycle
    if (proc->status == RUNNING) proc->tr--;

    return (proc_table->n_procs - proc_table->current) ? RUNNING : READY;
}
