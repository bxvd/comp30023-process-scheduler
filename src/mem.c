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

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <stdio.h>

#include "mem.h"

// Flag for unallocated memory addresses
#define FREE -1

// Clock cycles required to load a page into memory
#ifndef PAGE_LOAD_TIME
#define PAGE_LOAD_TIME 2
#endif

/*
 * Allocates memory for a new Memory struct and initialises its values.
 * 
 * int size: The size of the memory to be available in KB.
 * 
 * Returns Memory*: Pointer to the new Memory struct.
 */
Memory *create_memory(int size, int page_size) {

    Memory *memory = (Memory*)calloc(1, sizeof(Memory));
    memory->size = size;
    memory->page_size = page_size;
    memory->pages = (int*)malloc((size / page_size) * sizeof(int));
    
    for (int i = 0; i < size / page_size; i++) {
        memory->pages[i] = FREE;
    }

    return memory;
}

/*
 * Frees pages in a Memory struct.
 * 
 * int *pages:     Array of addresses to be freed.
 * int n:          Number of pages to be freed.
 * Memory *memory: Pointer to a memory struct.
 */
void free_memory(Process *process, int *pages, int n, Memory *memory) {

    memory->n_evicted = 0;

    for (int i = 0; i < n; i++) {

        // Remove page from process page array
        for (int j = 0; j < process->n_pages; j++) {
            process->pages[j] = process->pages[j] == pages[i] ? FREE : process->pages[j];
        }

        // Free pages from memory
        memory->pages[pages[i]] = FREE;
        memory->n_evicted++;
        memory->evicted = (int*)realloc(memory->evicted, memory->n_evicted * sizeof(int));
        memory->evicted[memory->n_evicted - 1] = pages[i];
    }

    process->n_pages -= n;
}

/*
 * Returns the next available page in memory.
 * 
 * Memory *memory: Pointer to a Memory struct containing pages.
 * 
 * Returns int: Index of available page.
 */
int next_free(Memory *memory) {

    for (int i = 0; i < memory->size / memory->page_size; i++) {
        if (memory->pages[i] == FREE) return i;
    }

    return FREE;
}

/*
 * Evicts pages from memory, beginning with the least recently used.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Memory *memory:        Pointer to a memory struct.
 * int n:                 Number of pages required.
 */
void make_free(ProcTable *proc_table, Memory *memory, int n) {

    int min_tl = INT_MAX, min_i = 0, freed = 0;

    
    while (freed < n) {

        // Get the least recently executed process that has pages available
        for (int i = 0; i < proc_table->n_procs; i++) {
            if (proc_table->procs[i].tl < min_tl && proc_table->procs[i].n_pages) {
                min_tl = proc_table->procs[i].tl;
                min_i = i;
            }
        }
        fprintf(stderr, "%d, %d\n", min_i, proc_table->procs[min_i].n_pages);

        // Increment freed count by how many pages this
        freed += proc_table->procs[min_i].n_pages;

        free_memory(&proc_table->procs[min_i],
                    proc_table->procs[min_i].pages,
                    proc_table->procs[min_i].n_pages,
                    memory);
    }
}

/*
 * Allocates a page of memory to a process.
 * 
 * Process *process: Pointer to process struct that requires memory allocation.
 * Memory *memory:   Memory struct containing pages of memory.
 * int page:         Index of memory page to allocate to the process.
 */
void allocate_page(Process *process, Memory *memory, int page) {

    process->n_pages++;
    process->pages[process->n_pages - 1] = page;
    process->tm += PAGE_LOAD_TIME;

    memory->pages[page] = process->id;
}

/*
 * Uses clock cycle to allocate a page to memory for the current process
 * indicated in proc_table.
 * 
 * ProcTable *proc_table: Pointer to a process table.
 * Memory *memory:        Pointer to a memory struct.
 * 
 * returns int: Enumerated status flag.
 */
int allocate_memory(ProcTable *proc_table, Memory *memory) {

    if (memory->allocator == UNLIMITED_MEMORY) return RUNNING;

    int next;

    // Shorthand
    Process *proc = &proc_table->procs[proc_table->current];

    // Check if more memory needs to be allocated
    if (proc->n_pages < proc->mem / memory->page_size) {

        // Takes two cycles to load a page to memory
        proc->load_s += 1;
        if (proc->load_s == PAGE_LOAD_TIME) {

            // Reset page load time
            proc->load_s = 0;
            
            // Check if memory needs to be freed to load this process
            if ((next = next_free(memory)) == FREE) {

                make_free(proc_table,
                          memory,
                          (proc->mem / memory->page_size) - proc->n_pages);

                next = next_free(memory);
            }

            allocate_page(proc, memory, next);
        }

        return LOADING;
    }

    return RUNNING;
}
