#ifndef RR_H
#define RR_H

#include "sys.h"

/*
 * Allocates CPU time to processes according
 * to a round-robin algorithm.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Memory *memory:        Pointer to memory structure.
 * int t:                 Time.
 * 
 * Returns int: Enumerated status code.
 */
int rr_run(ProcTable *proc_table, Memory *memory, int t);

#endif
