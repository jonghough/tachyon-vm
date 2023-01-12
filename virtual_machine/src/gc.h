#ifndef GC_H
#define GC_H
#include "vm.h"
#include "datatypes.h"

void mark_data(data *d);
void mark(virtual_machine *v);
void sweep(virtual_machine *v);
void run_gc(virtual_machine *v, int run_cleanup);
void cleanup_gc(virtual_machine *v);
#endif
