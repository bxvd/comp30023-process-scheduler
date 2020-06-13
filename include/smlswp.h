/*
 * smlswp.c
 * 
 * A set of methods for performing memory swapping prioritising
 * smaller footprint processes. Written for project 2 of
 * COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */

#ifndef SMLSWP_H
#define SMLSWP_H

#include "sys.h"

/*
 * Performs memory swapping with the goal of keeping the entirety of
 * smaller processes in memory and only forcing the largest processes
 * to use virtual memory.
 * 
 * System *sys: Pointer to an OS struct.
 */
void smallswap(System *sys);

#endif
