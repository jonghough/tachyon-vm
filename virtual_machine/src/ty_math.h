#ifndef TY_MATH_H
#define TY_MATH_H

#include "memory.h"
#include "datatypes.h"
#include "vm_defs.h"

void ty_sin(virtual_machine * v);
void ty_cos(virtual_machine * v);
void ty_tan(virtual_machine * v);

void ty_asin(virtual_machine * v);
void ty_acos(virtual_machine * v);
void ty_atan(virtual_machine * v);

void ty_sinh(virtual_machine * v);
void ty_cosh(virtual_machine * v);
void ty_tanh(virtual_machine * v);

void ty_asinh(virtual_machine * v);
void ty_acosh(virtual_machine * v);
void ty_atanh(virtual_machine * v);

void ty_logar(virtual_machine * v);
void ty_logar10(virtual_machine * v);
void ty_exp(virtual_machine * v); 

#endif
