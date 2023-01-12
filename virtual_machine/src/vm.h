#ifndef VM_H
#define VM_H
#include <stdint.h>
#include <stddef.h>
#include "topcode.h" 
#include "datatypes.h"
#include "vm_operations.h"
#include "vm_file_operations.h"
#include "vm_cast_operations.h"
#include "vm_binary_operations.h"
 
virtual_machine *create_vm(long stack_size, long max_heap_size, char *program, uint32_t program_size, uint8_t debug, gc_op gc_op);
void destroy_vm(virtual_machine *vm); 
void initialize(virtual_machine *v);
void allocate_stack(virtual_machine *v); 
void run(virtual_machine *v);
void read_one(virtual_machine *v);
void parse_args(code *c, char * arg_str);

 
#endif
