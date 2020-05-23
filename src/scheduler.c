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
#include <math.h>
#include <limits.h>

#include "sys.h"

#define OPTARGS "f:a:m:s:q:vd"
#define EPOCH   60

void print_stats(ProcTable *proc_table) {

    // Throughput, turnaround, makespan, and overhead
    int tp_min = INT_MAX, tp_max = 0, tp_avg = 0, trn = 0, ms;
    float oh_max = 0.0, oh_avg = 0.0;
    Process _proc;

    // Makespan
    ms = proc_table->procs[proc_table->n_procs - 1].tf;

    // Throughput intervals
    int n_intervals = ms / EPOCH, interval[n_intervals];
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
            oh_avg += _oh;
        }
    }

    // Throughput
    for (int i = 0; i < n_intervals; i++) {

        tp_min = interval[i] < tp_min ? interval[i] : tp_min;
        tp_max = interval[i] > tp_max ? interval[i] : tp_max;
        tp_avg += interval[i];
    }

    // Averages
    tp_avg = (int)ceil((float)tp_avg / n_intervals);
    trn = (int)ceil((float)trn / proc_table->n_procs);
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

    int n = 0;
    Process new_proc;
    FILE *file;

    if ((file = fopen(filename, "r")) == NULL) return ERROR;

    while (fscanf(file, "%d %d %d %d\n", &new_proc.ta, &new_proc.id, &new_proc.mem, &new_proc.tj) == 4) {

        new_proc.tr = new_proc.tj;
        new_proc.status = READY;

        // Expand array memory and copy data into it
        *procs = (Process*)realloc(*procs, ++n * sizeof(Process));
        memmove(*procs + n - 1, &new_proc, sizeof(Process));
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

void simulate(Process *procs, int n) {

    int t = -1; // Time
    ProcTable *proc_table = new_proc_table();

    // Sort processes in increasing order of arrival time and ID
    qsort(procs, n, sizeof(Process), proc_compare);

    // Loop as a clock that calls run() on each cycle
    do {
        //getchar();

        t++;
        for (int i = 0; i < n; i++) {
            if (procs[i].ta == t) add_process(proc_table, procs[i]);
        }

    } while (run(proc_table, t) == RUNNING);

    print_stats(proc_table);

    free(proc_table);
}

int main(int argc, char **argv) {
    
    int opt, n, scheduler = FF_SCHEDULING, mem_allocator = UNLIMITED_MEMORY,
        mem_size = 0, quantum = DEFAULT_QUANTUM, verbosity = NORMAL;
    char *filename;
    Process *procs = NULL;

    set(VERBOSITY, verbosity);

    // Handle CL options
    while ((opt = getopt(argc, argv, OPTARGS)) != -1) {
        switch (opt) {
            case 'f':
                filename = (char*)malloc(strlen(optarg) + 1);
                strcpy(filename, optarg);
                break;

            case 'a':
                if (!strcmp(optarg, "ff")) set(SCHEDULER, FF_SCHEDULING);
                if (!strcmp(optarg, "rr")) set(SCHEDULER, RR_SCHEDULING);
                if (!strcmp(optarg, "cs")) set(SCHEDULER, CUSTOM_SCHEDULING);
                break;
            
            case 'm':
                if (!strcmp(optarg, "u")) set(MEM_ALLOCATOR, UNLIMITED_MEMORY);
                if (!strcmp(optarg, "p")) set(MEM_ALLOCATOR, SWAPPING_X_MEMORY);
                if (!strcmp(optarg, "v")) set(MEM_ALLOCATOR, VIRTUAL_MEMORY);
                if (!strcmp(optarg, "cm")) set(MEM_ALLOCATOR, CUSTOM_MEMORY);
                break;
            
            case 's':
                set(MEM_SIZE, atoi(optarg));
                mem_size = atoi(optarg);
                break;

            case 'q':
                set(QUANTUM, atoi(optarg));
                quantum = atoi(optarg);
                break;

            case 'v':
                set(VERBOSITY, VERBOSE);
                verbosity = VERBOSE;
                break;
            
            case 'd':
                set(VERBOSITY, DEBUG);
                verbosity = DEBUG;
                break;
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

    simulate(procs, n);

    free(procs);
    free(filename);

    return 0;
}

