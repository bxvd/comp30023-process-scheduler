#include "swap.h"

void swap(System *sys) {

    // Shorthand
    Process *p = &sys->table.p[sys->table.context];

    int candidate, target = (p->mem / sys->page_size) - p->n_pages;

    p->time.load = 0;

    while (p->n_pages < target) {

        allocate(sys, target);

        if (p->n_pages < target) {
            
            candidate = oldest(*sys);
            evict(sys, sys->table.p[candidate].id, sys->table.p[candidate].n_pages);
        }
    }
}
