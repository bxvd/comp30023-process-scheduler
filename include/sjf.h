/*
 * sjf.c
 * 
 * A Shortest-Job-First process scheduling algorithm to run
 * for a process scheduling simulator. Written for project
 * 2 of COMP30023 Computer Systems, semester 1 2020.
 * 
 * Author: Brodie Daff
 *         bdaff@student.unimelb.edu.au
 */

#include "sys.h"

/*
 * Handles a clock cycle for the OS according to
 * Shortest-Job-First scheduling.
 * 
 * System *sys: Pointer to the OS.
 */
void cs_step(System *sys);
