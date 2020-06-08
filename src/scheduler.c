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
#include <limits.h>

#include "scheduler.h"

#define OPTARGS "f:a:m:s:q:vd"

/*
 * Handles starting the process and printing output for the
 * FF scheduling algorithm.
 * 
 * ProcTable *proc_table: Pointer to process table.
 * Memory *memory:        Pointer to memory struct.
 * int t:                 Time.
 */
void simulate_start(ProcTable *proc_table, Memory *memory, int t) {

    // Shorthand variable
    Process *proc = &proc_table->procs[proc_table->current];

    // Print memory that may have been evicted for this process
    if (memory->n_evicted) {

        fprintf(stdout, "%d, EVICTED, mem-addresses=[", t);

        for (int i = 0; i < memory->n_evicted - 1; i++) {
            fprintf(stdout, "%d,", memory->pages[i]);
        }
        fprintf(stdout, "%d]\n", memory->pages[memory->n_evicted - 1]);

        // Reset the flag indicating pages have been evicted
        memory->n_evicted = 0;
    }

    // Process is about to begin loading
   // if (proc->status != LOADING) fprintf(stdout, "%d, RUNNING, id=%d, remaining-time=%d", t, proc->id, proc->tr);

    start_process(proc_table, memory, t);

    // Process has finished loading during this cycle
    if (proc->status == RUNNING) {

        if (proc->tl == t) fprintf(stdout, "%d, RUNNING, id=%d, remaining-time=%d", t, proc->id, proc->tr);
        
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
void simulate_finish(ProcTable *proc_table, Memory *memory, int t) {

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

    proc_table->n_alive--;

    fprintf(stdout, "%d, FINISHED, id=%d, proc-remaining=%d\n", t, proc->id, proc_table->n_alive);
}

/*
 * Calculates and prints statistics for processes that
 * have finished.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 */
void print_stats(ProcTable *proc_table) {

    // Throughput, turnaround, makespan, and overhead
    int tp_min = INT_MAX, tp_max = 0, tp_avg = 0, trn = 0, ms = 0;
    float oh_max = 0.0, oh_avg = 0.0;
    Process _proc;

    // Makespan
    for (int i = 0; i < proc_table->n_procs; i++) {
        ms = proc_table->procs[i].tf > ms ? proc_table->procs[i].tf : ms;
    }

    // Throughput intervals
    int n_intervals = ceil((float)ms / EPOCH), interval[n_intervals];
    memset(interval, 0, n_intervals * sizeof(int));

    for (int i = 0; i < proc_table->n_procs; i++) {

        _proc = proc_table->procs[i];

        if (_proc.status == FINISHED) {

            // Increase count of processes finished in each interval
            interval[_proc.tf / (EPOCH + 1)]++;

            // Turnaround time
            int _trn = _proc.tf - _proc.ta;
            trn += _trn;

            // Overhead
            float _oh = (float)_trn / _proc.tj;
            oh_max = _oh > oh_max ? _oh : oh_max;
            oh_avg += _oh;\
        }
    }

    // Throughput
    for (int i = 0; i < n_intervals; i++) {

        tp_min = interval[i] < tp_min ? interval[i] : tp_min;
        tp_max = interval[i] > tp_max ? interval[i] : tp_max;
        tp_avg += interval[i];
    }

    // Averages
    tp_avg = ceil((float)tp_avg / n_intervals);
    trn = ceil((float)trn / proc_table->n_procs);
    oh_avg /= proc_table->n_procs;

    fprintf(stdout, "Throughput %d, %d, %d\n", tp_avg, tp_min, tp_max);
    fprintf(stdout, "Turnaround time %d\n", trn);
    fprintf(stdout, "Time overhead %.2f %.2f\n", oh_max, oh_avg);
    fprintf(stdout, "Makespan %d\n", ms);
}

/*
 * Loads processes from a file to a process table.
 * 
 * char *filename: File containing process metadata.
 * Process *procs: Array of processes.
 * 
 * Returns int: Number of processes loaded.
 */
int get_procs_from_file(char *filename, Process **procs) {

    int n = 0, ta, id, mem, tj;
    FILE *file;

    if ((file = fopen(filename, "r")) == NULL) return ERROR;

    while (fscanf(file, "%d %d %d %d\n", &ta, &id, &mem, &tj) == 4) {

        Process *new_proc = create_process(id, mem, ta, tj);

        // Expand array memory and copy data into it
        *procs = (Process*)realloc(*procs, ++n * sizeof(Process));
        memmove(*procs + n - 1, new_proc, sizeof(Process));

        free(new_proc);
    }

    fclose(file);
    return n;
}

/*
 * Comparison function for sorting the process table for
 * Round-Robin scheduling.
 * 
 * const void *a, *b: Pointers to processes to be compared.
 * 
 * Returns int: Negative if a < b, 0 if a == b, positive if a > b.
 */
int proc_compare(const void *a, const void *b) {

    const Process *p1 = a, *p2 = b;
    
    if (p1->ta < p2->ta) return -1;
    if (p1->ta == p2->ta) {
        if (p1->id < p2->id) return -1;
        if (p1->id == p2->id) return 0;
    }
    return 1;
}

void simulate(Process *procs, int n, ProcTable *proc_table, Memory *memory) {

    int t = -1, status;

    // Sort processes in increasing order of arrival time and ID
    qsort(procs, n, sizeof(Process), proc_compare);

    // Loop as a clock that calls run() on each cycle
    do {
        fprintf(stderr, "%d", t);
        getchar();

        // Time
        t++;
        
        // Add a new process if clock is at its arrival time
        for (int i = proc_table->n_procs; i < n; i++) {
            if (procs[i].ta == t) add_process(proc_table, procs[i]);
        }

        status = run(proc_table, memory, t);

    // Continue running if there are still queued processes or if the system hasn't finished running
    } while (n > proc_table->n_procs || status != READY);

    print_stats(proc_table);
}

int main(int argc, char **argv) {
    
    int opt, n, scheduler = FF_SCHEDULING, mem_allocator = UNLIMITED_MEMORY,
        mem_size = 0, quantum = DEFAULT_QUANTUM, verbosity = NORMAL;
    char *filename;
    ProcTable *proc_table = NULL;
    Process *procs = NULL;
    Memory *memory = NULL;

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
                if (!strcmp(optarg, "cm")) mem_allocator = CUSTOM_MEMORY;
                break;
            
            case 's': mem_size = atoi(optarg); break;
            case 'q': quantum = atoi(optarg); break;
        }
    }

    /* DEBUG */
    if (verbosity == DEBUG) {

        fprintf(stderr, "argv:");

        for (int i = 0; i < argc; i++) {
            fprintf(stderr, " %s", argv[i]);   
        }

        fprintf(stderr, "\n");
    }

    n = get_procs_from_file(filename, &procs);

    /* VERBOSE */
    if (verbosity == DEBUG || verbosity == VERBOSE) {
        fprintf(stderr, "\nScheduling algorithm    Memory allocator    Memory size%s    File name\n", scheduler == RR_SCHEDULING ? "    Quantum" : "");

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

        fprintf(stderr, "\nProcesses loaded (%d processes):\n(id, memory, time arrived, job time)\n", n);
        for (int i = 0; i < n; i++) {
            fprintf(stderr, "%d %d %d %d\n", procs[i].id, procs[i].mem, procs[i].ta, procs[i].tj);
        }
        fprintf(stderr, "\n");
    }

    // Initialise data structures
    proc_table = create_proc_table();
    memory = create_memory(mem_size, PAGE_SIZE);

    proc_table->scheduler = scheduler;
    proc_table->quantum = quantum;
    memory->allocator = mem_allocator;

    // Main program
    simulate(procs, n, proc_table, memory);

    free(procs);
    free(proc_table);
    free(memory->pages);
    free(memory);
    free(filename);

    return 0;
}

