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

#ifndef SYS_H
#define SYS_H

#ifndef PAGE_SIZE
#define PAGE_SIZE 4
#endif


#define PAGE_LOAD_TIME 2
#define MIN_MEM        16
#define MIN_PAGES      (MIN_MEM / PAGE_SIZE)

#ifndef DEFAULT_QUANTUM
#define DEFAULT_QUANTUM 10
#endif

// Flag for checking uninitialised variables
#define UNDEF -1

typedef enum status { ERROR, INIT, START, READY, LOADING, RUNNING, TERMINATED } Status;
typedef enum scheduler { FF, RR, CS } Scheduler;
typedef enum allocator { U, SWP, V, CM } Allocator;
typedef enum notification { RUN, FINISH, EVICT } Notification;

/*
 * Process time struct for tracking process metadata and
 * for calculating running statistics.
 * 
 * int arrived:   Arrival time of process.
 * int job:       Total time required to complete process.
 * int remaining: Remaining time required to complete process.
 * int started:   Time that the process began running.
 * int last:      Time of the process' last state change.
 * int finished:  Time that the process was completed.
 * int load:      Time most recently spent loading pages into memory.
 */
typedef struct PTime {
    int arrived, job, remaining, started, last, finished, load;
} PTime;

/*
 * Memory page.
 * 
 * int pid: Process ID that this page is allocated to.
 * int pix: Index within the the allocated process' array of Pages.
 */
typedef struct Page {
    int pid, pix;
} Page;

/*
 * Process struct for use in a process table.
 * 
 * Status status: Current state of the process.
 * PTime time:    Process Time struct to track process metadata.
 * Page **pages:  Array of memory addresses allocated to the process.
 * int id:        Process ID.
 * int mem:       Memory required (in KB).
 * int n_pages:   Number of pages in memory.
 */
typedef struct Process {
    Status status;
    PTime time;
    Page **pages;
    int id, mem, n_pages;
} Process;

/*
 * Encapsulates all process data.
 * 
 * Status status: Current state of the process.
 * Process *p:    Array of processes.
 * int n:         Number of processes in table.
 * int n_alive:   Number of processes that haven't been terminated.
 * int context:   Index of process in the current context.
 */
typedef struct PTable {
    Status status;
    Process *p;
    int n, n_alive, context;
} PTable;

/*
 * A de-facto OS structure. Contains all data and state tracking needed
 * for running the system.
 * 
 * Status status:       Current state of the system.
 * PTable table:        Process table.
 * Page *pages:         Memory pages;
 * Scheduler scheduler: Process scheduling algorithm to use.
 * Allocator allocator: Memory allocation algorithm to use.
 * int time:            Current system time.
 * int quantum:         Quantum time limit for a process (if applicable).
 * int mem_size:        System memory size (in KB).
 * int page_size:       Memory page size (in KB).
 * int n_pages:         Number of memory pages.
 */
typedef struct System {
    Status status;
    PTable table;
    Page *pages;
    Scheduler scheduler;
    Allocator allocator;
    int time, quantum, mem_size, page_size, n_pages;
} System;

#include "scheduler.h"
#include "mem.h"
#include "ff.h"
#include "rr.h"
//#include "swap.h"

Process *create_process(int id, int mem, int t_arrived, int t_job);

System *start(Process *p, int n, Scheduler s, Allocator a, int m, int q);

void evict(System *sys, int pid, int n);

/*
 * Finds the least recently allocated process in the
 * process table.
 * 
 * System sys: OS data structure.
 * 
 * Returns int: index in the process table for the oldest process.
 */
int oldest(System sys);

/*
 * Begins running the process in the current context.
 * 
 * System *sys: Pointer to an OS.
 */
void process_start(System *sys);

void process_pause(System *sys);

void process_resume(System *sys);

void process_finish(System *sys);

#endif