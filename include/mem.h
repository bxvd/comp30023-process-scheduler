/*
 * mem.c
 * 
 * A set of methods and data structures for handling memory
 * allocation for an operating system. Written for project 2
 * of COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */

#ifndef MEM_H
#define MEM_H

#include "sys.h"

#define min(a, b) (a < b ? a : b)

/*
 * Allocates memory for an array of Page structures
 * representing the memory in the scheduling simulator.
 * 
 * int size:      Total size in KB of the memory.
 * int page_size: Size in KB of each page.
 * 
 * Returns Page*: Pointer to new array of Pages.
 */
Page *create_memory(int size, int page_size);

/*
 * Allocates pages to the process in the current context.
 * Only allocates pages that are free, does not create
 * free pages.
 * 
 * System *sys: Pointer to an OS struct.
 * int target:  Target number of pages to allocated to the process.
 */
void allocate(System *sys, int target);

/*
 * Evicts pages currently allocated to a process.
 * 
 * System *sys: Pointer to an OS struct.
 * int pid:     Process ID to evict pages for.
 * int n:       Number of pages to evict.
 */
void evict_process(System *sys, int pid, int n);

/*
 * Finds the least recently allocated process in the
 * process table.
 * 
 * System sys: An OS struct.
 * 
 * Returns int: index in the process table for the oldest process.
 */
int oldest(System sys);

/*
 * Performs a memory swap based on the Swapping-X algorithm.
 * Chooses the oldest allocated processes and evicts all of
 * their pages as necessary, allocating them to the current
 * context.
 * 
 * System *sys: Pointer to an OS struct.
 */
void swap(System *sys);

/*
 * Performs memory swaps based on the virtual memory algorithm.
 * Chooses the oldest allocated processes and evicts only the
 * number of pages needed to allocated to and begin running the
 * current context.
 * 
 * System *sys: Pointer to an OS struct.
 */
void virtual(System *sys);

#endif
