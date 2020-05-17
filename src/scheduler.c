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

int main(int argc, char **argv) {
    
    int opt, mem_size, quantum = DEFAULT_QUANTUM;
    char *filename;
    verbosity = NORMAL;

    // Handle CL options
    while ((opt = getopt(argc, argv, OPTARGS)) != -1) {
        switch (opt) {
            case 'f':
                filename = (char*)malloc(strlen(optarg + 1));
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
            case 'v': verbosity = VERBOSE; break;
            case 'd': verbosity = DEBUG; break;
        }
    }

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
    }

    free(filename);

    return 0;
}

