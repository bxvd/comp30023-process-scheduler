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

#include "sys.h"

#define OPTARGS "f:a:m:s:q:vd"

int main(int argc, char **argv) {
    
    int opt;
    char *filename;
    ProcTable *proc_table = new_proc_table();

    set_verbosity(NORMAL);

    // Handle CL options
    while ((opt = getopt(argc, argv, OPTARGS)) != -1) {
        switch (opt) {
            case 'f':
                filename = (char*)malloc(strlen(optarg) + 1);
                strcpy(filename, optarg);
                break;

            case 'a':
                if (!strcmp(optarg, "ff")) set_scheduler(FF_SCHEDULING);
                if (!strcmp(optarg, "rr")) set_scheduler(RR_SCHEDULING);
                if (!strcmp(optarg, "cs")) set_scheduler(CUSTOM_SCHEDULING);
                break;
            
            case 'm':
                if (!strcmp(optarg, "u")) mem_allocator = UNLIMITED_MEMORY;
                if (!strcmp(optarg, "p")) mem_allocator = SWAPPING_X_MEMORY;
                if (!strcmp(optarg, "v")) mem_allocator = VIRTUAL_MEMORY;
                if (!strcmp(optarg, "cm")) mem_allocator = CUSTOM_MEMORY;
                break;
            
            case 's': set_mem(atoi(optarg), proc_table); break;
            case 'q': set_quantum(atoi(optarg), proc_table); break;
            case 'v': set_verbosity(VERBOSE); break;
            case 'd': set_verbosity(DEBUG); break;
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

    if (get_procs_from_file(filename, proc_table)) return 1;

    /* VERBOSE */
    if (verbosity == DEBUG || verbosity == VERBOSE) {
        fprintf(stderr, "Scheduling algorithm    Memory allocator    Memory size%s    File name\n", scheduler == RR_SCHEDULING ? "    Quantum" : "");

        switch(scheduler) {
            case FF_SCHEDULING: fprintf(stderr, "First-come-first-served "); break;
            case RR_SCHEDULING: fprintf(stderr, "Round-robin             "); break;
            case CUSTOM_SCHEDULING: fprintf(stderr, "Custom                  "); break;
        }

        switch (mem_allocator) {
            case UNLIMITED_MEMORY: fprintf(stderr, "Unlimited           Unlimited      "); break;
            case SWAPPING_X_MEMORY: fprintf(stderr, "Swapping-X          %11i    ", proc_table->sys_mem); break;
            case VIRTUAL_MEMORY: fprintf(stderr, "Virtual             %11i    ", proc_table->sys_mem); break;
            case CUSTOM_MEMORY: fprintf(stderr, "Custom              %11i    ", proc_table->sys_mem); break;
        }

        if (scheduler == RR_SCHEDULING) fprintf(stderr, "%7i    ", proc_table->quantum);
        fprintf(stderr, "%s\n", filename);

        for (int i = 0; i < proc_table->n_procs; i++) {
            fprintf(stderr, "%d %d %d %d\n", proc_table->procs[i].id, proc_table->procs[i].mem, proc_table->procs[i].ta, proc_table->procs[i].tr);
        }
    }

    free(proc_table->procs);
    free(filename);

    return 0;
}

