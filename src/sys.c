#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys.h"
#include "ff.h"
#include "rr.h"

int scheduler, mem_allocator, verbosity, sys_mem, quantum = DEFAULT_QUANTUM;

ProcTable *new_proc_table() {

    ProcTable *proc_table = (ProcTable*)calloc(1, sizeof(ProcTable));
    proc_table->n_procs = 0;
    proc_table->current = 0;

    return proc_table;
}

int add_process(ProcTable *proc_table, Process new_proc) {

    // Increase process table size
    proc_table->n_procs++;
    proc_table->procs = (Process*)realloc(proc_table->procs, proc_table->n_procs * sizeof(Process));

    // Copy process details to the process table
    memmove(&proc_table->procs[proc_table->n_procs - 1], &new_proc, sizeof(Process));

    return proc_table->procs == NULL ? ERROR : OK;
}

void set(int variable, int value) {

    switch (variable) {
        case SCHEDULER: scheduler = value; break;
        case MEM_ALLOCATOR: mem_allocator = value; break;
        case MEM_SIZE: sys_mem = value; break;
        case QUANTUM: quantum = value; break;
        case VERBOSITY: verbosity = value; break; 
    }
}

void start_process(Process *proc, int t) {

    proc->status = RUNNING;
    proc->ts = t;
}

void finish_process(Process *proc, int t) {

    proc->status = FINISHED;
    proc->tf = t;
}

int run(ProcTable *proc_table, int t) {

    /* VERBOSE */
    //if (verbosity == VERBOSE || verbosity == DEBUG) fprintf(stderr, "System running\n\n");

    if (!proc_table->n_procs) return READY;

    switch (scheduler) {
        case FF_SCHEDULING: return ff_run(proc_table, t, verbosity);
        case RR_SCHEDULING: return rr_run(proc_table, t, quantum, verbosity);
    }

    return ERROR;
}
