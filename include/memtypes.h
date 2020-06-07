#ifndef MEMTYPES_H
#define MEMTYPES_H

// System running modes
enum { UNLIMITED_MEMORY, SWAPPING_X_MEMORY, VIRTUAL_MEMORY, CUSTOM_MEMORY };

/*
 * Memory struct consisting of an array of ints, with the array
 * index representing the page index and each value representing
 * the PID that the page is allocated to.
 * 
 * int allocator: Enumerated values for allocator algorithm.
 * int size:      Size of the memory in KB.
 * int page_size: Size in KB of each page.
 * int *pages:    Array of pages containing PID of its allocation.
 */
typedef struct {
    int allocator, size, page_size, *pages;
} Memory;

#endif
