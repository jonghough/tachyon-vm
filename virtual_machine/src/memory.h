#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
#include "datatypes.h"
#include "vm.h"

void destroy_parr(parr *m);
// ===== pmem ===================
void destroy_pmem(pmem *m);
// ===== prec ===================
prec *allocate_mem_prec(virtual_machine *v);
void destroy_prec(prec *m);
// ===== pfile ==================
pfile *allocate_mem_pfile(virtual_machine *v);
void destroy_pfile(pfile *m);

data *allocate_mem_data(virtual_machine *v, uint32_t num, uint8_t with_gc);
void destroy_data(data *d);

pmem *alloc_str(virtual_machine *v, size_t len);
void panic_on_malloc_fail();

void copy_data(data *src, data *dest);
void copy_data_ref(virtual_machine *v, data *src, data *dest);
void copy_data_by_value(virtual_machine *v, data *src, data *dest);
void copy_str_index(virtual_machine * v, data * src, data * dest, uint32_t index);
void set_function_param(virtual_machine *v, data *src, data *dest);

gc_obj *allocate_gc_obj(virtual_machine *v);
uint64_t get_heap_size(virtual_machine *v);
gc_obj *get_heap_ref_tail(virtual_machine *v);
gctype gctype_from_dtype(dtype type_enum);

parr *alloc_int8_arr(virtual_machine *v, size_t size);
parr *alloc_int32_arr(virtual_machine *v, size_t size);
parr *alloc_int64_arr(virtual_machine *v, size_t size);
parr *alloc_float32_arr(virtual_machine *v, size_t size);
parr *alloc_float64_arr(virtual_machine *v, size_t size);
parr *alloc_str_arr(virtual_machine *v, size_t size);
parr *alloc_rec_arr(virtual_machine *v, size_t size);

#define ALLOCATE_MEMX(func, type_enum, type_n)         \
    parr *func(virtual_machine *v, size_t size)        \
    {                                                  \
        parr *p = (parr *)malloc(sizeof(parr));        \
        if (!p)                                        \
        {                                              \
            panic_on_malloc_fail();                    \
        }                                              \
        gc_obj *d = allocate_gc_obj(v);                \
        d->type = GCARR;                               \
        d->content.xptr = p;                           \
        p->type = type_enum;                           \
        p->objs = (data *)malloc(size * sizeof(data)); \
        if (!p->objs)                                  \
        {                                              \
            panic_on_malloc_fail();                    \
        }                                              \
        return p;                                      \
    }

#endif
