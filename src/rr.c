#include <stdio.h>
#include <limits.h>

#include "sys.h"

int next_process(ProcTable *proc_table) {

    // Get process with longest elapsed time since last state change
    int min_tl = INT_MAX;

    for (int i = 0; i < proc_table->n_procs; i++) {

        if (proc_table->procs[i].status == READY) {

            if (proc_table->procs[i].tl < min_tl) {

                min_tl = proc_table->procs[i].tl;
                proc_table->current = i;
            } else if (proc_table->procs[i].tl == min_tl) {

                // Choose latest arrival time for processes with same elapsed time since state change
                proc_table->current = proc_table->procs[proc_table->current].ta < proc_table->procs[i].ta ? i : proc_table->current;
            }
        }
    }

    return min_tl == INT_MAX ? FINISHED : READY;
}

int rr_run(ProcTable *proc_table, int t, int q, int verbosity) {

    // Shorthand variable
    Process *proc = &proc_table->procs[proc_table->current];

    // Start process
    if (proc->status == READY) {

        start_process(proc, t);
        fprintf(stdout, "%d, RUNNING, id=%d, remaining-time=%d\n", t, proc->id, proc->tr);
    }

    // Boolean
    int in_quantum = t < (proc->tl + q);

    // Process has completed job time
    if (!proc->tr) {

        finish_process(proc, t);
        proc_table->n_alive--;

        fprintf(stdout, "%d, FINISHED, id=%d, proc-remaining=%d\n", t, proc->id, proc_table->n_alive);
    }

    // Process is still running but out of quantum time
    if (proc->status == RUNNING && !in_quantum) pause_process(proc, t);

    // Give CPU to next process if one is available
    if (proc->status != RUNNING) {
        return next_process(proc_table) == FINISHED ? READY : rr_run(proc_table, t, q, verbosity);
    }

    // Give CPU to current process
    proc->tr--;

    return RUNNING;
}
