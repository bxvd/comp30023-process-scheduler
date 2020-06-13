/*
 * rr.c
 * 
 * A Round-Robin process scheduling algorithm to run for a
 * process scheduling simulator. Written for project 2 of
 * COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */

#ifndef RR_H
#define RR_H
#include <stdio.h>
#include "sys.h"

/*
 * Handles a clock cycle for the OS according to
 * Round-Robin scheduling.
 * 
 * System *sys: Pointer to the OS.
 */
void rr_step(System *sys);

#endif
