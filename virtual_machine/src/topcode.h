#ifndef TOPCODE_H
#define TOPCODE_H

#include <stdint.h>
#include "datatypes.h"

typedef struct
{
    char *name;
    char value;
} op;

typedef struct
{
    char *prog;
    long prog_sz;
} prog;

typedef struct
{
    uint32_t idx;
    op opcode;
    uint8_t o1p;
    uint8_t o2p;
    char *o1;
    char *o2;
} code;

typedef struct
{
    uint32_t size;
    code *code_arr;
} code_vals;

typedef struct virtual_machine virtual_machine;
typedef void (*gc_op)(virtual_machine *v, int run_cleanup);

struct virtual_machine
{
    uint64_t max_heap_size;        /* max size of heap */
    uint64_t heap_size;            /* size of heap */
    uint64_t stack_size;           /* size of data stack */
    uint64_t stack_limit;          /* limit of data stack */
    uint8_t stack_frame_header_sz; /* size of stack frame header */
    uint32_t SP;                   /* stack pointer */
    uint32_t FP;                   /* frame pointer */
    uint32_t IP;                   /* instruction pointer */
    uint32_t EP;                   /* end of program */
    uint32_t IPTR;                 /* return instruction pont pointer */
    uint32_t ST;                   /* stack frame top */
    data *stack;                   /* data stack */
    int state;                     /* state of the vm */
    uint8_t debug;                 /* debug flag. Output debug info. */
    code_vals *code;               /* code vector */
    prog *prog;                    /* the program instructions */
    gc_obj *heap_refs;             /* linked list of gc heap objects */
    gc_op gc_op;                   /* gc operation callback */
};

extern op topcodes[];

op get_op(char *name);
op get_op_by_val(char val);
op get_op_from_int(uint32_t val);

#endif
