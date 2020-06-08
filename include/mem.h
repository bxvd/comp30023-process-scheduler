#ifndef MEM_H
#define MEM_H

#include "memtypes.h"
#include "systypes.h"

// Page size in KB
#ifndef PAGE_SIZE
#define PAGE_SIZE 4
#endif

/*
 * Allocates memory for a new Memory struct and initialises its values;
 * 
 * int size: The size of the memory to be available in KB.
 * 
 * Returns Memory*: Pointer to the new Memory struct.
 */
Memory *create_memory(int size, int page_size);

/*
 * Uses clock cycle to allocate a page to memory for the current process
 * indicated in proc_table.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Memory *memory:        Pointer to a memory struct.
 * 
 * returns int: Enumerated status flag.
 */
int allocate_memory(ProcTable *proc_table, Memory *memory);

/*
 * Frees pages in a Memory struct.
 * 
 * int *pages:     Array of addresses to be freed.
 * int n:          Number of pages to be freed.
 * Memory *memory: Pointer to a memory struct.
 */
void free_memory(Process *process, int *pages, int n, Memory *memory);

#endif
