#include <stdlib.h>
#include <string.h>

#include "mem.h"

#define FREE -1
#define PAGE_LOAD_TIME 2

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

    // Shorthand
    Process *proc = &proc_table->procs[proc_table->current];

    // Check if more mem needs to be allocated
    if (proc->n_pages < proc->mem / memory->page_size) {

        // Takes two cycles to load a page to memory
        proc->load_s += 1;
        if (proc->load_s == PAGE_LOAD_TIME) {
            proc->load_s = 0;
            allocate_page(proc, memory, next_free(memory));
        }

        return LOADING;
    }

    return RUNNING;
}

void free_memory(int *pages, int n, Memory *memory) {

    for (int i = 0; i < n; i++) {
        memory->pages[pages[i]] = FREE;
    }
}
