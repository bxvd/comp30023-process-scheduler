/*
 * ff.c
 * 
 * An First-Come-First-Served process scheduling algorithm
 * to run for a process scheduling simulator. Written for
 * project 2 of COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */ 

#ifndef FF_H
#define FF_H

// Define types and system calls
#include "sys.h"

/*
 * First-Come-Fisrt-Served process scheduling algorithm.
 * 
 * ProcTable *proc_table: Pointer to process table.
 * Memory *memory:        Pointer to memory struct.
 * int t:                 Time.
 * 
 * Returns int: Enumerated status flag.
 */
int ff_run(ProcTable *proc_table, Memory *memory, int t);

#endif
