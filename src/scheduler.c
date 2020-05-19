/*
 * scheduler
 * 
 * A process scheduling and memory management simulator written for
 * project 2 of COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */ 

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define OPTARGS "f:a:m:s:q:vd"

#define DEFAULT_QUANTUM 10

enum { FF_SCHEDULING, RR_SCHEDULING, CUSTOM_SCHEDULING } scheduler;
enum { UNLIMITED_MEMORY, SWAPPING_X_MEMORY, VIRTUAL_MEMORY, CUSTOM_MEMORY } mem_allocator;
enum { NORMAL, VERBOSE, DEBUG } verbosity;

typedef struct {
    int id;
    int mem;
    int ta;
    int tr;
} Process;

typedef struct {
    int n_procs;
    Process *procs;
} ProcTable;

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
int get_procs(char *filename, ProcTable *proc_table) {

    Process new_proc;
    FILE *file = fopen(filename, "r");

    if (file == NULL) return 1;

    while (fscanf(file, "%d %d %d %d\n", &new_proc.ta, &new_proc.id, &new_proc.mem, &new_proc.tr) == 4) {
        if (add_process(proc_table, new_proc)) return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    
    int opt, mem_size, quantum = DEFAULT_QUANTUM;
    char *filename;
    ProcTable proc_table;

    verbosity = NORMAL;

    // Handle CL options
    while ((opt = getopt(argc, argv, OPTARGS)) != -1) {
        switch (opt) {
            case 'f':
                filename = (char*)malloc(strlen(optarg) + 1);
                strcpy(filename, optarg);
                break;

            case 'a':
                if (!strcmp(optarg, "ff")) scheduler = FF_SCHEDULING;
                if (!strcmp(optarg, "rr")) scheduler = RR_SCHEDULING;
                if (!strcmp(optarg, "cs")) scheduler = CUSTOM_SCHEDULING;
                break;
            
            case 'm':
                if (!strcmp(optarg, "u")) mem_allocator = UNLIMITED_MEMORY;
                if (!strcmp(optarg, "p")) mem_allocator = SWAPPING_X_MEMORY;
                if (!strcmp(optarg, "v")) mem_allocator = VIRTUAL_MEMORY;
                if (!strcmp(optarg, "cm")) mem_allocator = CUSTOM_SCHEDULING;
                break;
            
            case 's': mem_size = atoi(optarg); break;
            case 'q': quantum = atoi(optarg); break;
            case 'v': verbosity = VERBOSE; break;
            case 'd': verbosity = DEBUG; break;
        }
    }

    if (get_procs(filename, &proc_table)) return 1;

    /* DEBUG */
    if (verbosity == DEBUG) {
        fprintf(stderr, "Scheduling algorithm    Memory allocator    Memory size%s    File name\n", scheduler == RR_SCHEDULING ? "    Quantum" : "");

        switch(scheduler) {
            case FF_SCHEDULING: fprintf(stderr, "First-come-first-served "); break;
            case RR_SCHEDULING: fprintf(stderr, "Round-robin             "); break;
            case CUSTOM_SCHEDULING: fprintf(stderr, "Custom                  "); break;
        }

        switch (mem_allocator) {
            case UNLIMITED_MEMORY: fprintf(stderr, "Unlimited           Unlimited      "); break;
            case SWAPPING_X_MEMORY: fprintf(stderr, "Swapping-X          %11i    ", mem_size); break;
            case VIRTUAL_MEMORY: fprintf(stderr, "Virtual             %11i    ", mem_size); break;
            case CUSTOM_MEMORY: fprintf(stderr, "Custom              %11i    ", mem_size); break;
        }

        if (scheduler == RR_SCHEDULING) fprintf(stderr, "%7i    ", quantum);
        fprintf(stderr, "%s\n", filename);

        for (int i = 0; i < proc_table.n_procs; i++) {
            fprintf(stderr, "%d %d %d %d\n", proc_table.procs[i].id, proc_table.procs[i].mem, proc_table.procs[i].ta, proc_table.procs[i].tr);
        }
    }

    free(proc_table.procs);
    free(filename);

    return 0;
}

