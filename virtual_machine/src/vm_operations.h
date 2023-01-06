#ifndef VM_OPERATIONS
#define VM_OPERATIONS
#include <string.h>
#include "topcode.h"


// funcs
void push_i8(virtual_machine *v, char *val);
void push_i32(virtual_machine *v, char *val);
void push_i64(virtual_machine *v, char *val);
void push_f32(virtual_machine *v, char *val);
void push_f64(virtual_machine *v, char *val);
void push_s(virtual_machine *v, char *val);
 
void jneq(virtual_machine *v, uint32_t n);
 
void not(virtual_machine * v);

 

void append(virtual_machine *v);

void push_local(virtual_machine *v, uint32_t n);
void set_local(virtual_machine *v, uint32_t n);
void set_param(virtual_machine *v, uint32_t n);
void push_param(virtual_machine *v, uint32_t n);

// local, param index
void push_local_idx(virtual_machine *v, uint32_t n);
void set_local_idx(virtual_machine *v, uint32_t n);
void set_param_idx(virtual_machine *v, uint32_t n);
void push_param_idx(virtual_machine *v, uint32_t n);
void set_local_a(virtual_machine *v, uint32_t n);

//records
void set_record_idx(virtual_machine *v, uint32_t n);
void set_param_record_idx(virtual_machine *v, uint32_t n);
void set_local_r(virtual_machine *v, uint32_t n); 
void push_local_r(virtual_machine *v, uint32_t n);
void push_param_r(virtual_machine *v, uint32_t n);
void push_stack_r(virtual_machine * v);
void push_stack_a(virtual_machine * v);

// call, goto
void call(virtual_machine *v, uint32_t n);
void stdoutx(virtual_machine *vm);
void readx(virtual_machine *vm);
void sizex(virtual_machine *vm, uint32_t p, uint32_t l);
void xgoto(virtual_machine *v, uint32_t n); 
void ret(virtual_machine *v);
void length(virtual_machine *v);

void inc_sp(virtual_machine *v);
void dec_sp(virtual_machine *v);
void inc_ip(virtual_machine * v);
void jmp_sp(virtual_machine *vm, uint32_t b);


void stack_alloc(virtual_machine *v, data src);
 
// debug
void debug_stack(virtual_machine *v); 

#endif
