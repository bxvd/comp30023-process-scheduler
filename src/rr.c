#include <stdio.h>

#include "sys.h"

int rr_run(ProcTable *proc_table, int t, int q, int verbosity) {

    // Shorthand variables
    Process *proc = &proc_table->procs[proc_table->current];

    // Current process is within the quantum
    if (t < (proc->tl + q)) {

        if (proc->status == READY) {

            start_process(proc, t);

            fprintf(stdout, "%d, RUNNING, id=%d, remaining-time=%d\n", t, proc->id, proc->tr);
        }

        proc->tr--;
    } else {

        proc->tl = t;
        proc->status = proc->tr ? READY : FINISHED;






        // Start next process if one is available
        if (proc_table->n_procs - proc_table->current) {

            proc_table->current++;

            return rr_run(proc_table, t, verbosity);
        }
    }










    //fprintf(stderr, "t: %d, pid: %d, pstatus: %d, nprocs: %d, current: %d\r", t, proc->id, proc->status, proc_table->n_procs, proc_table->current);

    if (proc->status == READY) {

        start_process(proc, t);

        fprintf(stdout, "%d, RUNNING, id=%d, remaining-time=%d\n", t, proc->id, proc->tr);

    } else if (proc->tr == 0 && proc->status == RUNNING) {

        finish_process(proc, t);

        proc_table->current++;

        fprintf(stdout, "%d, FINISHED, id=%d, proc-remaining=%d\n", t, proc->id, proc_table->n_procs - proc_table->current);

        // Start next process if one is available
        if (proc_table->n_procs - proc_table->current) return ff_run(proc_table, t, verbosity);
    }

    // Reduce remaining time for the running process by one cycle
    if (proc->status == RUNNING) proc->tr--;

    return (proc_table->n_procs - proc_table->current) ? RUNNING : FINISHED;
}
