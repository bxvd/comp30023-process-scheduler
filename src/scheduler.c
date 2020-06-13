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
#include <stdarg.h>

#include "scheduler.h"

#define OPTARGS "f:a:m:s:q:vd"

/*
 * Calculates and prints statistics for processes that
 * have finished.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 */
void print_stats(System *sys) {

    // Throughput, turnaround, makespan, and overhead
    int tp_min = INT_MAX, tp_max = 0, tp_avg = 0, trn = 0, ms = 0;
    float oh_max = 0.0, oh_avg = 0.0;
    Process *p = sys->table.p;

    // Makespan
    for (int i = 0; i < sys->table.n; i++) {
        ms = p[i].time.finished > ms ? p[i].time.finished : ms;
    }

    // Throughput intervals
    int n_intervals = ceil((float)ms / EPOCH), interval[n_intervals];
    memset(interval, 0, n_intervals * sizeof(int));

    for (int i = 0; i < sys->table.n; i++) {

        if (p[i].status == TERMINATED) {

            // Increase count of processes finished in each interval
            interval[p[i].time.finished / (EPOCH + 1)]++;

            // Turnaround time
            int _trn = p[i].time.finished - p[i].time.arrived;
            trn += _trn;

            // Overhead
            float _oh = (float)_trn / p[i].time.job;
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
    trn = ceil((float)trn / sys->table.n);
    oh_avg /= sys->table.n;

    fprintf(stdout, "Throughput %d, %d, %d\n", tp_avg, tp_min, tp_max);
    fprintf(stdout, "Turnaround time %d\n", trn);
    fprintf(stdout, "Time overhead %.2f %.2f\n", oh_max, oh_avg);
    fprintf(stdout, "Makespan %d\n", ms);
}


/*
 * Loads processes from a file to a process table.
 * 
 * char *filename: File containing process metadata.
 * Process *p:     Array of processes.
 * 
 * Returns int: Number of processes loaded.
 */
int get_procs_from_file(char *filename, Process **p) {

    int n = 0, id, mem, t_arrived, t_job;
    FILE *file;

    if ((file = fopen(filename, "r")) == NULL) exit(EXIT_FAILURE);

    while (fscanf(file, "%d %d %d %d\n", &t_arrived, &id, &mem, &t_job) == 4) {

        Process *new_proc = create_process(id, mem, t_arrived, t_job);

        // Expand array memory and copy data into it
        *p = (Process*)realloc(*p, ++n * sizeof(Process));
        memmove(*p + n - 1, new_proc, sizeof(Process));

        free(new_proc);
    }

    fclose(file);
    return n;
}

void notify(Notification n, System sys, int var, ...) {

    int *values = NULL, n_values = 0, mem = 0;

    // Optional int array passed in
    if (var) {
        va_list ap;
        va_start(ap, var);
        values = va_arg(ap, int*);
        n_values = va_arg(ap, int);
        va_end(ap);
    }

    Process p = sys.table.p[sys.table.context];

    switch (n) {

        case RUN:

            // Get memory usage
            for (int i = 0; i < sys.n_pages; i++) {
                if (sys.pages[i].pid != UNDEF) mem++;
            }

            fprintf(stdout,
                    "%d, RUNNING, id=%d, remaining-time=%d",
                    sys.time,
                    p.id,
                    p.time.remaining);
            
            if (sys.allocator != U) {

                fprintf(stdout,
                        ", load-time=%d, mem-usage=%d%%, mem-addresses=[",
                        p.time.load,
                        ceil(((float)mem * 100) / sys.n_pages));
                
                // Look through addresses to find the ones allocated to the process
                int n = 0;
                for (int i = 0; i < sys.n_pages; i++) {
                    if (sys.pages[i].pid == p.id) {
                        
                        fprintf(stdout, "%d", i);

                        n++;
                        if (n < p.n_pages) fprintf(stdout, ",");
                    }
                }

                fprintf(stdout, "]");
            }

            fprintf(stdout, "\n");

            break;

        case FINISH:
            fprintf(stdout,
                    "%d, FINISHED, id=%d, proc-remaining=%d\n",
                    sys.time,
                    p.id,
                    sys.table.n_alive);
            break;
        
        case EVICT:

            fprintf(stdout, "%d, EVICTED, mem-addresses=[", sys.time);

            if (n_values) {

                fprintf(stdout, "%d", values[0]);
                for (int i = 1; i < n_values; i++) {
                    fprintf(stdout, ",%d", values[i]);
                }
                fprintf(stdout, "]\n");
            }

        default:
            break;
    }

    // malloc'd memory used for var arg
    if (values != NULL) free(values);
}

int main(int argc, char **argv) {
    
    int opt, n, mem_size = UNDEF, quantum = UNDEF;
    char *filename;
    Scheduler proc_scheduler;
    Allocator mem_allocator;
    Process *p = NULL;
    System *sys = NULL;

    // Handle CL options
    while ((opt = getopt(argc, argv, OPTARGS)) != -1) {
        switch (opt) {
            case 'f':
                filename = (char*)malloc(strlen(optarg) + 1);
                strcpy(filename, optarg);
                break;

            case 'a':
                if (!strcmp(optarg, "ff")) proc_scheduler = FF;
                if (!strcmp(optarg, "rr")) proc_scheduler = RR;
                if (!strcmp(optarg, "cs")) proc_scheduler = CS;
                break;
            
            case 'm':
                if (!strcmp(optarg, "u")) mem_allocator = U;
                if (!strcmp(optarg, "p")) mem_allocator = SWP;
                if (!strcmp(optarg, "v")) mem_allocator = V;
                if (!strcmp(optarg, "cm")) mem_allocator = CM;
                break;
            
            case 's': mem_size = atoi(optarg); break;
            case 'q': quantum = atoi(optarg); break;
        }
    }

    n = get_procs_from_file(filename, &p);
    sys = start(p, n, proc_scheduler, mem_allocator, mem_size, quantum);
    print_stats(sys);

    free(p);
    free(sys);

    return 0;
}

