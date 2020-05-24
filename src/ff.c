#include <stdio.h>

#include "sys.h"

int ff_run(ProcTable *proc_table, int t, int verbosity) {

    // Shorthand variable
    Process *proc = &proc_table->procs[proc_table->current];

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

    return (proc_table->n_procs - proc_table->current) ? RUNNING : READY;
}
