#ifndef SYSTYPES_H
#define SYSTYPES_H

// Status codes
enum { OK, ERROR, READY, RUNNING, FINISHED, WAITING, LOADING };

// System variables that can be set
enum { SCHEDULER, MEM_ALLOCATOR, VERBOSITY };

// System running modes
enum { FF_SCHEDULING, RR_SCHEDULING, CUSTOM_SCHEDULING };
enum { NORMAL, VERBOSE, DEBUG };

/*
 * Process struct for use in a process table.
 * 
 * int id:      Process ID.
 * int mem:     Memory required (KB).
 * int ta:      Time arrived.
 * int tj:      Job time.
 * int tr:      Time remaining.
 * int ts;      Time started.
 * int tl;      Time of last state change.
 * int tf:      Time finished.
 * int tm:      Time taken to load memory.
 * int load_s:  Page load status as a percentage (int between 0 and 100).
 * int *pages:  Memory addresses allocated to the process.
 * int n_pages: Number of pages in memory.
 * int status:  Current state of the process.
 */
typedef struct {
    int id, mem, ta, tj, tr, ts, tl, tf, tm, load_s, *pages, n_pages, status;
} Process;

/*
 * Process table struct.
 * 
 * int scheduler:  Enumerated values of process scheduling algorithms available.
 * int n_procs:    Number of processes in table.
 * int n_alive:    Number of processes in READY or RUNNING state.
 * int current:    Index of process currently running.
 * int quantum:    Quantum to use for scheduling.
 * Process *procs: Array of processes.
 */
typedef struct {
    int scheduler, n_procs, n_alive, current, quantum;
    Process *procs;
} ProcTable;

#endif
