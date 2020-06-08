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

#include "systypes.h"
#include "mem.h"
#include "rr.h"
#include "ff.h"

// Use the following default values if none specified
#ifndef DEFAULT_QUANTUM
#define DEFAULT_QUANTUM 10
#endif

/*
 * Allocates memory for a new ProcTable and initialises its values.
 * 
 * Returns ProcTable*: Pointer to the new ProcTable.
 */
ProcTable *create_proc_table();

/*
 * Creates a new process and initialise its values.
 * 
 * int id:  Process ID (must be unique).
 * int mem: Amount of memory required by the process in KB.
 * int ta:  Arrival time of the instruction to create the process.
 * int tj:  The process' job time (total CPU time required).
 * 
 * Returns Process*: Pointer to the newly created process.
 */
Process *create_process(int id, int mem, int ta, int tj);

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
 * Performs setup for the current process in a process table
 * to begin running it.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Memory *memory:        Pointer to a memory struct.
 * int t:                 Time.
 */
void start_process(ProcTable *proc_table, Memory *memory, int t);

/*
 * NEEDS DOC
 */
void pause_process(Process *proc, int t);

/*
 * Destroys and performs cleanup for the current process in a
 * process table.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Memory *memory:        Pointer to a memory struct.
 * int t:                 Time.
 */
void finish_process(ProcTable *proc_table, Memory *memory, int t);

/*
 * Executes one clock cycle on a process table when called.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Memory *memory:        Pointer to a memory struct.
 * int t:                 Time.
 * 
 * Returns int: Enumerated status code.
 */
int run(ProcTable *proc_table, Memory *memory, int t);

#endif
