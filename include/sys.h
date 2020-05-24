/*
 * sys.h sys.c
 * 
 * An operating system consisting of only process scheduling and
 * memory allocation procedures. Written for project 2 of
 * COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */ 

// Use the following default values if none specified
#ifndef DEFAULT_QUANTUM
#define DEFAULT_QUANTUM 10
#endif

// Status codes
enum { OK, ERROR, READY, RUNNING, FINISHED };

// System variables that can be set
enum { SCHEDULER, MEM_ALLOCATOR, MEM_SIZE, QUANTUM, VERBOSITY };

// System running modes
enum { FF_SCHEDULING, RR_SCHEDULING, CUSTOM_SCHEDULING };
enum { UNLIMITED_MEMORY, SWAPPING_X_MEMORY, VIRTUAL_MEMORY, CUSTOM_MEMORY };
enum { NORMAL, VERBOSE, DEBUG };

/*
 * Process struct for use in a process table.
 * 
 * int id:     Process ID.
 * int mem:    Memory required (KB).
 * int ta:     Time arrived.
 * int tj:     Job time.
 * int tr:     Time remaining.
 * int ts;     Time started.
 * int tl;     Time of last state change.
 * int tf:     Time finished.
 * int *pages: Memory addresses allocated to the process.
 * int status: Current state of the process.
 */
typedef struct {
    int id, mem, ta, tj, tr, ts, tl, tf, *pages, status;
} Process;

/*
 * Process table struct.
 * 
 * int n_procs:    Number of processes in table.
 * int n_alive:    Number of processes in READY or RUNNING state.
 * int current:    Index of process currently running.
 * Process *procs: Array of processes.
 */
typedef struct {
    int n_procs, n_alive, current;
    Process *procs;
} ProcTable;

/*
 * Allocates memory for a new ProcTable and initialises its values.
 * 
 * Returns ProcTable*: Pointer to the new ProcTable.
 */
ProcTable *new_proc_table();

/*
 * Adds a process to the process table.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Process new_proc:      Details of process to be added.
 * 
 * Returns int: Evaluates to true if there is an error, false otherwise.
 */
int add_process(ProcTable *proc_table, Process new_proc);

/*
 * Sets a system variable without exposing it.
 * 
 * int variable: enum of settable variables.
 * int value:    Value to set it to.
 */
void set(int variable, int value);

void start_process(Process *proc, int t);
void pause_process(Process *proc, int t);
void finish_process(Process *proc, int t);

/*
 * Executes one clock cycle on a process table when called.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * int t:                 Time.
 * 
 * Returns int: Enumerated status code.
 */
int run(ProcTable *proc_table, int t);
