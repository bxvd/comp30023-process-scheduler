/*
 * ff.c
 * 
 * A First-Come-First-Served process scheduling algorithm
 * to run for a process scheduling simulator. Written for
 * project 2 of COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */ 

#ifndef FF_H
#define FF_H

#include "sys.h"

/*
 * Handles a clock cycle for the OS according to
 * First-Come-First-Served scheduling.
 * 
 * System *sys: Pointer to the OS.
 */
void ff_step(System *sys);

#endif
