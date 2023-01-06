#ifndef VM_BINARY_OPERATIONS
#define VM_BINARY_OPERATIONS
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "topcode.h"
#include "memory.h"
#include "datatypes.h"
#include "vm_defs.h"


// binary arith
void add(virtual_machine *v);
void sub(virtual_machine *v);
void mul(virtual_machine *v);
void divx(virtual_machine *v);
void sub(virtual_machine *v);
void rem(virtual_machine *v);
// boolean
void eq(virtual_machine *v);
void neq(virtual_machine *v);
void gt(virtual_machine *v);
void gte(virtual_machine *v);
void lt(virtual_machine *v);
void lte(virtual_machine *v);


void and(virtual_machine *v);
void or(virtual_machine *v);

void bit_and(virtual_machine * v);
void bit_or(virtual_machine * v);
void xor(virtual_machine * v);
void bit_not(virtual_machine * v);
void l_shift(virtual_machine * v);
void r_shift(virtual_machine * v);
#endif
