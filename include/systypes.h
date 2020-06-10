#ifndef SYSTYPES_H
#define SYSTYPES_H

typedef enum status { ERROR, INIT, START, READY, LOADING, RUNNING, TERMINATED } Status;
typedef enum scheduler { FF, RR, CS } Scheduler;
typedef enum allocator { U, SWP, V, CM } Allocator;

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
 */
typedef struct PTime {
    int arrived, job, remaining, started, last, finished;
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
 * int context:   Index of process in the current context.
 */
typedef struct PTable {
    Status status;
    Process *p;
    int n, context;
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
 * Process *context:    Pointer to the current process for easy access.
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
    Process *context;
    int time, quantum, mem_size, page_size, n_pages;
} System;

#endif
