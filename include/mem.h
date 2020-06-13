#ifndef MEM_H
#define MEM_H
#include <stdio.h>
#include "sys.h"

Page *create_memory(int size, int page_size);

void allocate(System *sys, int target);

void evict_process(System *sys, int pid, int n);

/*
 * Finds the least recently allocated process in the
 * process table.
 * 
 * System sys: OS data structure.
 * 
 * Returns int: index in the process table for the oldest process.
 */
int oldest(System sys);

void swap(System *sys);

void virtual(System *sys);

#endif
