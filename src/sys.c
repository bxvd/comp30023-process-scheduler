/*
 * sys.c
 * 
 * An operating system consisting of only process scheduling and
 * memory allocation procedures. Written for project 2 of
 * COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys.h"

ProcTable *create_proc_table() {

    ProcTable *proc_table = (ProcTable*)calloc(1, sizeof(ProcTable));
    proc_table->n_procs = 0;
    proc_table->current = 0;
    proc_table->quantum = DEFAULT_QUANTUM;

    return proc_table;
}

Process *create_process(int id, int mem, int ta, int tj) {

    Process *proc = (Process*)calloc(1, sizeof(Process));
    proc->id = id;
    proc->mem = mem;
    proc->ts = -1;
    proc->tl = proc->ta = ta;
    proc->tr = proc->tj = tj;
    proc->tm = proc->load_s = 0;
    proc->pages = NULL;
    proc->n_pages = 0;
    proc->status = WAITING;

    return proc;
}

int add_process(ProcTable *proc_table, Process new_proc) {

    // Increase process table size
    proc_table->n_procs++;
    proc_table->n_alive++;
    proc_table->procs = (Process*)realloc(proc_table->procs, proc_table->n_procs * sizeof(Process));

    // Copy process details to the process table
    memmove(&proc_table->procs[proc_table->n_procs - 1], &new_proc, sizeof(Process));

    return proc_table->procs == NULL ? ERROR : OK;
}

void start_process(ProcTable *proc_table, Memory *memory, int t) {

    // Shorthand
    Process *proc = &proc_table->procs[proc_table->current];
    int _status = proc->status;

    // Initialise memory
    if (proc->status == WAITING) {
        proc->pages = (int*)calloc(1, (proc->mem / memory->page_size) * sizeof(int));
    }

    // Use clock cycle for memory management
    proc->status = allocate_memory(proc_table, memory);

    // Check if process status has changed
    proc->tl = _status == proc->status ? proc->ts : t;

    // Indicate process initialisation using the start time
    proc->ts = proc->ts == -1 ? t : proc->ts; /* FLAG NEEDED */
}

void pause_process(Process *proc, int t) {

    proc->status = READY;
    proc->tl = t;
}

void finish_process(ProcTable *proc_table, Memory *memory, int t) {

    // Shorthand
    Process *proc = &proc_table->procs[proc_table->current];

    free_memory(proc->pages, proc->n_pages, memory);
    proc->status = FINISHED;
    proc->tf = t;
    proc->tl = t;
}

void print_proc_table(ProcTable *proc_table, int t) {

    fprintf(stderr, "Process table at time %d -> %d process(es), %d alive, currently running process %d\n", t, proc_table->n_procs, proc_table->n_alive, proc_table->procs[proc_table->current].id);
    fprintf(stderr, "PID | TR | TA | TJ | TL | TF | Status\n----|----|----|----|----|----|-------\n");

    for (int i = 0; i < proc_table->n_procs; i++) {
        Process proc = proc_table->procs[i];
        fprintf(stderr, "%3d |%3d |%3d |%3d |%3d |%3d |%3d\n", proc.id, proc.tr, proc.ta, proc.tj, proc.tl, proc.tf, proc.status);
    }
}

int run(ProcTable *proc_table, Memory *memory, int t) {

    //print_proc_table(proc_table, t);

    // Waiting for processes
    if (!proc_table->n_procs) return READY;

    switch (proc_table->scheduler) {
        case FF_SCHEDULING: return ff_run(proc_table, memory, t);
        case RR_SCHEDULING: return rr_run(proc_table, memory, t);
    }

    return ERROR;
}
