#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys.h"

/*
 * Allocates memory for a new ProcTable and initialises its values.
 * 
 * Returns ProcTable*: Pointer to the new ProcTable.
 */
ProcTable *new_proc_table() {

    ProcTable *proc_table = (ProcTable*)calloc(1, sizeof(ProcTable));
    proc_table->quantum = DEFAULT_QUANTUM;

    return proc_table;
}

/*
 * Adds a process to the process table.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Process new_proc:      Details of process to be added.
 * 
 * Returns int: Evaluates to true if there is an error, false otherwise.
 */
int add_process(ProcTable *proc_table, Process new_proc) {

    // Increase process count and dynamically allocate memory to store them
    proc_table->n_procs++;
    proc_table->procs = (Process*)realloc(proc_table->procs, proc_table->n_procs * sizeof(Process));

    // Copy process details to the process table
    memmove(&proc_table->procs[proc_table->n_procs - 1], &new_proc, sizeof(Process));

    return proc_table->procs == NULL ? 1 : 0;
}

/*
 * Loads processes from a file to a process table.
 * 
 * char *filename:        File containing process metadata.
 * ProcTable *proc_table: Pointer to a process table.
 * 
 * Returns int: Evaluates to true if there is an error, false otherwise.
 */
int get_procs_from_file(char *filename, ProcTable *proc_table) {

    Process new_proc;
    FILE *file = fopen(filename, "r");

    if (file == NULL) return 1;

    while (fscanf(file, "%d %d %d %d\n", &new_proc.ta, &new_proc.id, &new_proc.mem, &new_proc.tr) == 4) {
        if (add_process(proc_table, new_proc)) return 1;
    }

    fclose(file);
    return 0;
}

/*
 * Sets the scheduling algorithm to use.
 * 
 * int s: enum of algorithms.
 */
void set_scheduler(int s) {
    scheduler = s;
}

/*
 * Sets the memory allocation algorithm to use.
 * 
 * int m: enum of algorithms.
 */
void set_mem_allocator(int m) {
    mem_allocator = m;
}

/*
 * Sets the memory allocation algorithm to use.
 * 
 * int m: enum of algorithms.
 */
void set_mem(int m, ProcTable *proc_table) {
    proc_table->sys_mem = m;
}

/*
 * Sets the memory allocation algorithm to use.
 * 
 * int m: enum of algorithms.
 */
void set_quantum(int q, ProcTable *proc_table) {
    proc_table->quantum = q;
}

/*
 * Sets the verbosity for printing to stderr.
 * 
 * int v: enum of verbosity modes.
 */
void set_verbosity(int v) {
    verbosity = v;
}
