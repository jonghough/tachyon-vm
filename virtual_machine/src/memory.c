#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "memory.h"
#include "log.h"

#define ALLOC_CUSTOM(type, sz) ((void *)malloc(sz * sizeof(type)))

void destroy_pfile(pfile *m)
{
    free(m);
}

void destroy_parr(parr *m)
{
    free(m->objs);
    free(m);
}

prec *allocate_mem_prec(virtual_machine *v)
{
    prec *p = (prec *)malloc(sizeof(prec));
    p->type = REC;
    gc_obj *d = allocate_gc_obj(v);
    d->content.xrec = p;
    d->type = GCREC;
    return p;
}

pfile *allocate_mem_pfile(virtual_machine *v)
{
    pfile *p = (pfile *)malloc(sizeof(pfile));

    gc_obj *d = allocate_gc_obj(v);
    d->content.xfileptr = p;
    d->type = GCFILEPTR;
    return p;
}

data *allocate_mem_data(virtual_machine *v, uint32_t num, uint8_t with_gc)
{
    data *p = (data *)malloc(num * sizeof(data));
    if (!p)
    {
        panic_on_malloc_fail();
    }
    if (with_gc)
    {
        gc_obj *d = allocate_gc_obj(v);
        d->type = GCDATA;
        d->content.xd = p;
    }

    return p;
}

void destroy_prec(prec *m)
{
    free(m->objs);
    free(m);
}

void destroy_data(data *d)
{ 
     
     if (d->type == PTR)
    { 
        destroy_parr(d->content.xptr); 
    }
    else if (d->type == STR)
    { 
        destroy_pmem(d->content.xstr); 
    }
    else if (d->type == REC)
    { 
        destroy_prec(d->content.xrec); 
    }
    free(d);
 
}

void destroy_pmem(pmem *m)
{

    free(m->obj);
    free(m);
}

void copy_data(data *src, data *dest)
{
    dest->type = src->type;

    if (src->type == STR)
    { 
        dest->content.xstr = src->content.xstr;
    }
    else if (src->type == PTR)
    { 
        dest->content.xptr = src->content.xptr;
    }
    else
    {
        dest->content = src->content;
    }
}

void panic_on_malloc_fail()
{
    ty_log(TY_ERROR,"Failed to allocate memory\n");
    exit(1);
}

gc_obj *get_heap_ref_tail(virtual_machine *v)
{
    if (!v->heap_refs)
    {
        return NULL;
    }
    else
    {
        gc_obj *c = v->heap_refs;
        gc_obj *n;
        while (c)
        {
            n = c;
            c = c->next;
        }
        return n;
    }
}

gc_obj *allocate_gc_obj(virtual_machine *v)
{
    gc_obj *p = (gc_obj *)malloc(sizeof(gc_obj));
    p->prev = NULL;
    p->next = NULL;
    if (!p)
    {
        panic_on_malloc_fail();
    }
 

    gc_obj *cur = get_heap_ref_tail(v);
    if (!cur)
    {
        v->heap_refs = p;
    }
    else
    {
        cur->next = p;
        p->prev = cur;
    }
    return p;
}

uint64_t get_heap_size(virtual_machine *v)
{
    gc_obj *h = v->heap_refs;
    if (!h)
        return 0;
    uint64_t t = 0;
    while (h)
    {
        t++;
        h = h->next;
    }
    return t;
}

pmem *alloc_str(virtual_machine *v, size_t len)
{
    pmem *p = (pmem *)malloc(sizeof(pmem));

    if (!p)
    {
        panic_on_malloc_fail();
    }
    p->type = STR;
    p->len = len;
    char *chars = malloc((len) * sizeof(char));
    p->obj = chars;

    if (!p->obj)
    {
        panic_on_malloc_fail();
    }

    gc_obj *d = allocate_gc_obj(v);

    d->content.xstr = p;
    d->type = GCSTR;
    memset(p->obj, 0, len * sizeof(char));
    return p;
}

void copy_data_ref(virtual_machine *v, data *src, data *dest)
{

    if (src->type == STR)
    {
        copy_data_by_value(v, src, dest);
    }
    else if (src->type == PTR)
    {

        dest->content.xptr = src->content.xptr;
    }
    else if (src->type == REC)
    {

        dest->content.xrec = src->content.xrec;
    }
    else if (src->type == INT8)
    {
        dest->content.xi8 = src->content.xi8;
    }
    else if (src->type == INT32)
    {
        dest->content.xi32 = src->content.xi32;
    }
    else if (src->type == INT64)
    {
        dest->content.xi64 = src->content.xi64;
    }
    else if (src->type == FLT32)
    {
        dest->content.xf32 = src->content.xf32;
    }
    else if (src->type == FLT64)
    {
        dest->content.xf64 = src->content.xf64;
    }
    else if (src->type == FILEPTR)
    {
        dest->content.xfileptr = src->content.xfileptr;
    }
  
}

void set_function_param(virtual_machine *v, data *src, data *dest)
{
    if (src->type == PTR || src->type == STR || src->type == REC)
    {

        copy_data_by_value(v, src, dest);
    }
    else
    {
        copy_data_by_value(v, src, dest);
    }
}

void copy_data_by_value(virtual_machine *v, data *src, data *dest)
{
    dest->type = src->type;

    if (dest->type == INT8)  
    {
        dest->content.xi8 = src->content.xi8;
    }
    else if (dest->type == INT32)  
    {
        dest->content.xi32 = src->content.xi32;
    }
    else if (dest->type == INT64)  
    {
        dest->content.xi64 = src->content.xi64;
    }
    else if (dest->type == FLT32 )
    {
         dest->content.xf32 = src->content.xf32;
    }
    else if ( dest->type == FLT64)
    {
         dest->content.xf64 = src->content.xf64;
    }
    else if (dest->type == FILEPTR)
    {
        memcpy(dest, src, 8);
    }
    else if (dest->type == PTR)
    {
        uint32_t len = src->content.xptr->len;
        parr *p;
        if (src->content.xptr->type == INT32)
        {
            p = alloc_int32_arr(v, len);
            //
            // ARRAY ELEMENTS ARE COPIED BY VALUE, SO
            // NEED TO MAKE COPY HERE.
            for (uint32_t i = 0; i < src->content.xptr->len; i++)
            {
                memcpy(p->objs, src->content.xptr->objs, sizeof(data) * src->content.xptr->len);
            }
        }
        else if (src->content.xptr->type == INT64)
        {
            p = alloc_int64_arr(v, len);
            for (uint32_t i = 0; i < src->content.xptr->len; i++)
            {
                copy_data_by_value(v, &src->content.xptr->objs[i], &p->objs[i]);
            }
        }
        else if (src->content.xptr->type == FLT32)
        {
            p = alloc_float32_arr(v, len);
            for (uint32_t i = 0; i < src->content.xptr->len; i++)
            {
                copy_data_by_value(v, &src->content.xptr->objs[i], &p->objs[i]);
            }
        }
        else if (src->content.xptr->type == FLT64)
        {
            p = alloc_float64_arr(v, len);
            for (uint32_t i = 0; i < src->content.xptr->len; i++)
            {
                copy_data_by_value(v, &src->content.xptr->objs[i], &p->objs[i]);
            }
        }
        else if (src->content.xptr->type == STR)
        {

            p = alloc_str_arr(v, len);
            for (uint32_t i = 0; i < src->content.xptr->len; i++)
            {
                pmem *srcp = src->content.xptr->objs[i].content.xstr;
                pmem *copy = alloc_str(v, srcp->len);
                strcpy(copy->obj, srcp->obj);
                copy->len = srcp->len;
                copy->type = STR;
                p->objs[i].type = STR;
                p->objs[i].content.xstr = copy;
            }
        }
        else{
            ty_log(TY_ERROR, "Could not copy data. Unknown type.");
            exit(1);
        }

        p->len = len;
        dest->content.xptr = p;
    }
    else if (dest->type == REC)
    { 
        uint32_t len = src->content.xrec->len;
        prec *p = allocate_mem_prec(v); 
        data *di = malloc(sizeof(data) * len);
        for (uint32_t i = 0; i < src->content.xrec->len; i++)
        {
            
            copy_data_by_value(v, &src->content.xrec->objs[i], &di[i]);
        }
        p->type = REC;
        p->objs = di;
        p->len = len;
        dest->content.xrec = p;
    }
    else if (dest->type == STR)
    {
        pmem *srcp = src->content.xstr;
        pmem *copy = alloc_str(v, srcp->len);
        strcpy((char *)copy->obj, (char *)srcp->obj);
        copy->len = srcp->len;
        copy->type = STR;
        dest->content.xstr = copy;
    }
}

void copy_str_index(virtual_machine *v, data *src, data *dest, uint32_t index)
{
    if (src->type != STR)
    {
        ty_log(TY_ERROR,"Error: Copy str called with wrong type: %d\n", src->type);
        exit(1);
    }
    
    pmem *copy = alloc_str(v, 2); 
    strncpy(((char *)copy->obj), (char*)src->content.xstr->obj + index, 1);
 
    copy->len = 1;
    copy->type = STR;
    dest->content.xstr = copy;
    dest->type = STR;
}

gctype gctype_from_dtype(dtype type_enum)
{
    if (type_enum == PTR)
    {
        return GCARR;
    }
    else if (type_enum == STR)
    {
        return GCSTR;
    }
    else if (type_enum == REC)
    {
        return GCREC;
    }
    else if (type_enum == FILEPTR)
    {
        return GCFILEPTR;
    }
    ty_log(TY_ERROR,"cannot find gc type for %d\n", type_enum);
    exit(1);
}

ALLOCATE_MEMX(alloc_int8_arr, INT8, data)
ALLOCATE_MEMX(alloc_int32_arr, INT32, data)
ALLOCATE_MEMX(alloc_int64_arr, INT64, data)
ALLOCATE_MEMX(alloc_float32_arr, FLT32, data)
ALLOCATE_MEMX(alloc_float64_arr, FLT64, data)
ALLOCATE_MEMX(alloc_str_arr, STR, data)
ALLOCATE_MEMX(alloc_rec_arr, REC, data)
