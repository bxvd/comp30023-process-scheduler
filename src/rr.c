#include <stdlib.h>
#include <stdio.h>

#include "sys.h"

/*
 * Runs all processes in a process table on a first-come-first-served basis.
 * Runs until all processes in proc_table are finished.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * 
 * Returns int: Evaluates to true if there is an error, false otherwise.
 */
int rr_run(ProcTable *proc_table) {

    // Time
    int t = proc_table->procs[0].ta;

    /* DEBUG */
    if (verbosity == DEBUG) {

        fprintf(stderr, "Sorted process table (%d processes):\n(id, memory, time arrived, time required)\n", proc_table->n_procs);
        for (int i = 0; i < proc_table->n_procs; i++) {
            fprintf(stderr, "%d %d %d %d\n", proc_table->procs[i].id, proc_table->procs[i].mem, proc_table->procs[i].ta, proc_table->procs[i].tr);
        }
        fprintf(stderr, "\n");
    }

    // Iterate on each process
    for (int i = 0; i < proc_table->n_procs; i++) {

        // Start process
        fprintf(stdout, "%d, RUNNING, id=%d, remaining-time=%d", t, proc_table->procs[i].id, proc_table->procs[i].tr);

        // Allocate memory
        if (mem_allocator != UNLIMITED_MEMORY);

        fprintf(stdout, "\n");

        // Update clock
        t += proc_table->procs[i].tr;

        fprintf(stdout, "%d, FINISHED, id=%d, proc-remaining=%d\n", t, proc_table->procs[i].id, proc_table->n_procs - i - 1);

        proc_table->procs[i].status = FINISHED;
        proc_table->procs[i].tf = t;
    }

    return OK;
}
