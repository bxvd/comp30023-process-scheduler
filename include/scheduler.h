#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "sys.h"
#include "mem.h"

#define EPOCH 60

#define ceil(x) (x > (float)((int)x) ? (int)x + 1 : (int)x)

/*
 * Handles starting the process and printing output for the
 * FF scheduling algorithm.
 * 
 * ProcTable *proc_table: Pointer to process table.
 * Memory *memory:        Pointer to memory struct.
 * int t:                 Time.
 */
void simulate_start(ProcTable *proc_table, Memory *memory, int t);

/*
 * Handles finishing the process and printing output for the
 * FF scheduling algorithm.
 * 
 * ProcTable *proc_table: Pointer to process table.
 * Memory *memory:        Pointer to memory struct.
 * int t:                 Time.
 */
void simulate_finish(ProcTable *proc_table, Memory *memory, int t);

#endif
