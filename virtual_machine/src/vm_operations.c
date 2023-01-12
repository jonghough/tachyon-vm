#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "vm_operations.h"
#include "vm_defs.h"
#include "memory.h"
#include "gc.h"
#include "log.h"

void rem(virtual_machine *v)
{
    data t0, t1;
    t0 = SP_VAL;
    dec_sp(v);
    t1 = SP_VAL;
    data dest = SP_VAL;
    if (t0.type == t1.type && t0.type == INT8)
    {
        dest.type = INT8;
        dest.content.xi8 = t1.content.xi8 % t0.content.xi8;
        SP_VAL = dest;
    }
    else if (t0.type == t1.type && t0.type == INT32)
    {
        dest.type = INT32;
        dest.content.xi32 = t1.content.xi32 % t0.content.xi32;
        SP_VAL = dest;
    }
    else if (t0.type == t1.type && t0.type == INT64)
    {
        dest.type = INT64;
        dest.content.xi64 = t1.content.xi64 % t0.content.xi64;
        SP_VAL = dest;
    }
    else
    {
        ty_log(TY_ERROR, "cannot take remainder of non-integer types.\n");
        exit(1);
    }
    inc_ip(v);
}
// funcs
void push_i8(virtual_machine *v, char *val)
{
    inc_sp(v);
    data *d = &SP_VAL;
    d->type = INT8;
    int8_t i;
    sscanf(val, "%hhd", &i);
    d->content.xi8 = i;
    inc_ip(v);
}

void push_i32(virtual_machine *v, char *val)
{
    inc_sp(v);
    data *d = &SP_VAL;
    d->type = INT32;
    int32_t i;
    sscanf(val, "%d", &i);
    d->content.xi32 = i;
    inc_ip(v);
}

void push_i64(virtual_machine *v, char *val)
{
    inc_sp(v);
    data *d = &SP_VAL;
    d->type = INT64;
    int64_t i;
    sscanf(val, "%ld", &i);
    d->content.xi64 = i;
    inc_ip(v);
}

void push_f32(virtual_machine *v, char *val)
{
    inc_sp(v);
    data *d = &SP_VAL;
    d->type = FLT32;
    d->content.xf32 = atof(val);
    inc_ip(v);
}

void push_f64(virtual_machine *v, char *val)
{
    inc_sp(v);
    data *d = &SP_VAL;
    d->type = FLT64;
    d->content.xf64 = atof(val);
    inc_ip(v);
}

void push_d(virtual_machine *v, char *val)
{
    inc_sp(v);
    data *d = &SP_VAL;
    d->type = FLT64;
    double i;
    sscanf(val, "%lf", &i);
    d->content.xf64 = i;
    inc_ip(v);
}

void push_s(virtual_machine *v, char *val)
{
    inc_sp(v);
    data *d = &SP_VAL;
    d->type = STR;
    size_t l = strlen(val);
    pmem *p = alloc_str(v, l);
    char *cpy = (char *)(p->obj);
    strcpy(cpy, val + 1);
    cpy[l - 2] = 0;
    d->content.xstr = p;
    inc_ip(v);
}

void allocate_stack(virtual_machine *v)
{
    int sz = sizeof(data);
    v->stack_limit = v->stack_size / sz;

    if (malloc(v->stack_limit * sz) == 0)
    {
        // continue
        v->SP = 0;
    }
    else
    {
        ty_log(TY_ERROR, "Failed to allocate stack memory\n");
        exit(1);
    }
}

void push_local(virtual_machine *v, uint32_t n)
{

    data p = v->stack[v->FP + 2];
    inc_sp(v);
    copy_data_by_value(v, &v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n], &v->stack[v->SP]);
    inc_ip(v);
}
void set_local(virtual_machine *v, uint32_t n)
{
    data p = v->stack[v->FP + 2];
    copy_data_by_value(v, &SP_VAL, &v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n]);
    dec_sp(v);
    inc_ip(v);
}

void push_param(virtual_machine *v, uint32_t n)
{
    inc_sp(v);
    data param = v->stack[v->FP + HEADER_SIZE + n];
    v->stack[v->SP] = param;
    inc_ip(v);
}

// local, param index
void push_local_idx(virtual_machine *v, uint32_t n)
{
    data dp = v->stack[v->FP + 2];
    data idx = v->stack[v->SP];
    int32_t i = idx.content.xi32;
    if (v->stack[v->FP + HEADER_SIZE + (dp.content.xi32) + n].type == PTR)
    {
        copy_data_by_value(v, &(v->stack[v->FP + HEADER_SIZE + (dp.content.xi32) + n].content.xptr->objs[i]), &v->stack[v->SP]);
    }
    else if (v->stack[v->FP + HEADER_SIZE + (dp.content.xi32) + n].type == STR)
    {
        copy_str_index(v, &(v->stack[v->FP + HEADER_SIZE + (dp.content.xi32) + n]), &v->stack[v->SP], i);
    }
    inc_ip(v);
}
void set_local_idx(virtual_machine *v, uint32_t n)
{

    data p = v->stack[v->FP + 2];
    data idx = v->stack[v->SP - 1];
    if (v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n].type == PTR)
    {
        copy_data_by_value(v, &v->stack[v->SP], &v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n].content.xptr->objs[idx.content.xi32]);
    }
    else if (v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n].type == STR)
    {

        copy_str_index(v, &(v->stack[v->FP + HEADER_SIZE + (p.content.xi32) + n]), &v->stack[v->SP], idx.content.xi32);
    }
    dec_sp(v);
    dec_sp(v);
    inc_ip(v);
}
void push_param_idx(virtual_machine *v, uint32_t n)
{
    data idx = v->stack[v->SP];
    data arr = v->stack[v->FP + HEADER_SIZE + n];
    if (arr.type == PTR)
    {
        SP_VAL = v->stack[v->FP + HEADER_SIZE + n].content.xptr->objs[idx.content.xi32];
    }

    inc_ip(v);
}
void set_local_a(virtual_machine *v, uint32_t n)
{
    data p = v->stack[v->FP + 2];
    data size = v->stack[v->SP];
    dec_sp(v);
    int32_t sz = size.content.xi32;
    data d = v->stack[v->SP - sz + 1];
    parr *pa;
    switch (d.type)
    {
    case INT8:
        pa = alloc_int8_arr(v, sz);
        break;
    case INT32:
        pa = alloc_int32_arr(v, sz);
        break;
    case INT64:
        pa = alloc_int64_arr(v, sz);
        break;
    case FLT32:
        pa = alloc_float32_arr(v, sz);
        break;
    case FLT64:
        pa = alloc_float64_arr(v, sz);
        break;
    case STR:
        pa = alloc_str_arr(v, sz);
        break;
    case REC:
        pa = alloc_rec_arr(v, sz);
        break;
    default:
        ty_log(TY_ERROR, "Error creating array. Type %d is not allowed.\n", d.type);
        exit(1);
    }

    uint32_t ptr = v->SP - sz;
    int c = 0;
    while (ptr++ < v->SP)
    {
        copy_data_by_value(v, &v->stack[ptr], &pa->objs[c++]);
    }
    pa->len = sz;
    v->SP = v->SP - sz;
    inc_sp(v);
    v->stack[v->SP].type = PTR;
    v->stack[v->SP].content.xptr = pa;
    v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n] = v->stack[v->SP];
    dec_sp(v);
    v->IP += 1;
}

// call, goto
void call(virtual_machine *v, uint32_t n)
{
    v->IPTR = v->IP;
    v->IP = n;
}
void xgoto(virtual_machine *v, uint32_t n)
{
    v->IP = n;
}

void inc_sp(virtual_machine *v)
{
    if (v->stack_limit <= v->SP)
    {
        ty_log(TY_ERROR, "Stack overflow. Stack limit exceeded.\n");
        exit(1);
    }
    data *d = &v->stack[++v->SP];
    d->type = NONE;
}

void clear(virtual_machine *v, uint32_t n)
{
    data d = v->stack[n];
    printf("data clear %d", d.type);
    ty_log(TY_DEBUG, "data clear");
    d.type = NONE;
    dec_sp(v);
}

void not(virtual_machine * v)
{

    data d = SP_VAL;
    inc_sp(v);
    data r = SP_VAL;
    r.type = INT32;
    uint32_t val = d.content.xi32;
    r.content.xi32 = val == 0 ? 1 : 0;
    SP_VAL = r;
    inc_ip(v);
}

void bit_not(virtual_machine *v)
{
    data d = SP_VAL;
    inc_sp(v);
    data r = SP_VAL;
    r.type = INT32;
    uint32_t val = d.content.xi32;
    r.content.xi32 = ~val;
    SP_VAL = r;
    inc_ip(v);
}

void dec_sp(virtual_machine *v)
{
    if (v->SP == 0)
    {
        return;
    }
    data d = v->stack[v->SP];
    d.type = NONE;
    SP_VAL = d;
    v->SP--;
}

void jmp_sp(virtual_machine *vm, uint32_t b)
{
    uint32_t i = 0;
    while (i++ < b)
    {
        dec_sp(vm);
    }
}

void inc_ip(virtual_machine *v)
{
    v->IP++;
}

void sizex(virtual_machine *v, uint32_t p, uint32_t l)
{

    int32_t fp = v->FP;
    inc_sp(v);
    v->FP = v->SP;
    data spd1 = SP_VAL;
    spd1.type = INT32;
    spd1.content.xi32 = fp;
    SP_VAL = spd1;
    inc_sp(v);
    data spd2 = SP_VAL;
    spd2.type = INT32;
    spd2.content.xi32 = v->IPTR + 1;
    SP_VAL = spd2;
    inc_sp(v);
    data spd3 = SP_VAL;
    spd3.type = INT32;
    spd3.content.xi32 = p;
    SP_VAL = spd3;
    inc_sp(v);
    data spd4 = SP_VAL;
    spd4.type = INT32;
    spd4.content.xi32 = l;
    SP_VAL = spd4;
    v->SP = v->FP + (HEADER_SIZE - 1) + p + l;
    inc_ip(v);
}

void stdoutx(virtual_machine *vm)
{
    uint32_t sp = vm->SP;
    data d = vm->stack[sp];
    if (d.type == INT8)
    {
        printf("%hhd\n", (char)d.content.xi8);
    }
    else if (d.type == INT32)
    {
        printf("%d\n", (int)d.content.xi32);
    }
    else if (d.type == INT64)
    {
        printf("%ldl\n", (long)d.content.xi64);
    }
    else if (d.type == FLT32)
    {
        printf("%f\n", d.content.xf32);
    }
    else if (d.type == FLT64)
    {
        printf("%lf\n", d.content.xf64);
    }
    else if (d.type == PTR)
    {
        printf("%ls\n", (int *)d.content.xptr);
    }
    else if (d.type == STR)
    {
        printf("%s\n", (char *)d.content.xstr->obj);
    }
    else
    {
    }

    inc_ip(vm);
}

void length(virtual_machine *v)
{
    uint32_t sp = v->SP;
    data d = v->stack[sp];
    dec_sp(v);
    inc_sp(v);
    data *r = &SP_VAL;
    r->type = INT32;
    r->content.xi32 = d.content.xptr->len;

    inc_ip(v);
}

void readx(virtual_machine *v)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread = 0;
    nread = getline(&line, &len, stdin);
    if (ferror(stdin))
    {
        ty_log(TY_ERROR, "Error reading input.\n");
        exit(1);
    }

    // remove the newline (UNIX \n)
    if (line[nread - 1] == '\n')
    {
        line[nread - 1] = '\0';
    }
    inc_sp(v);
    data *d = &SP_VAL;
    d->type = STR;
    pmem *p = alloc_str(v, nread - 1);
    strcpy((char *)p->obj, line);
    free(line); // TODO: dont free!
    d->content.xstr = p;
    inc_ip(v);
}

void ret(virtual_machine *v)
{
    uint32_t fp = v->FP;
    // uint32_t param_count = v->stack[fp + 2].content.xi32;
    // uint32_t local_count = v->stack[fp + 3].content.xi32;
    // uint32_t lim = fp + HEADER_SIZE + param_count + local_count;
    data r = SP_VAL;

    uint32_t sp = v->ST;
    v->SP = v->FP - v->stack[fp + 2].content.xi32 - 1;
    v->FP = v->stack[fp].content.xi32;
    v->IP = v->stack[fp + 1].content.xi32;
    inc_sp(v);

    copy_data_by_value(v, &r, &SP_VAL);
#if DEBUG_BUILD
    ty_log(TY_DEBUG, "return: IP %d, delete stack from %d down to %d\n", v->IP, sp, v->SP);
#endif
    debug_stack(v);
    for (; sp > v->SP; sp--)
    {
#if DEBUG_BUILD
        ty_log(TY_DEBUG, "Next op, stack pointer is at: %d\n", sp);
#endif
        data d = v->stack[sp];
        d.type = NONE;
        v->stack[sp] = d;
    }

    v->ST = v->SP;
    /*** RUN GC HERE **/
    v->gc_op(v, 0);
}

void set_param(virtual_machine *v, uint32_t n)
{

    data p = v->stack[v->FP + 2];
    set_function_param(v, &v->stack[v->FP - p.content.xi32 + n], &v->stack[v->FP + HEADER_SIZE + n]);
    inc_ip(v);
}

void set_param_idx(virtual_machine *v, uint32_t n)
{
    data idx = v->stack[v->SP - 1];
    copy_data_by_value(v, &SP_VAL, &v->stack[v->FP + HEADER_SIZE + n].content.xptr->objs[idx.content.xi32]);
    dec_sp(v);
    inc_ip(v);
}

void set_record_idx(virtual_machine *v, uint32_t n)
{
    data idx = v->stack[v->SP];
    data p = v->stack[v->FP + 2];
    copy_data_by_value(v, &v->stack[v->SP - 1], &v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n].content.xptr->objs[idx.content.xi32]);
    dec_sp(v);
    inc_ip(v);
}
void set_param_record_idx(virtual_machine *v, uint32_t n)
{
    data idx = v->stack[v->SP];
    copy_data_by_value(v, &v->stack[v->SP - 1], &v->stack[v->FP + HEADER_SIZE + n].content.xptr->objs[idx.content.xi32]);
    dec_sp(v);
    inc_ip(v);
}
void set_local_r(virtual_machine *v, uint32_t n)
{
    data rec_size = SP_VAL;
    data p = v->stack[v->FP + 2];
    data *arr = allocate_mem_data(v, rec_size.content.xi32, 0);
    uint32_t i = rec_size.content.xi32;
    while (i > 0)
    {
        copy_data_by_value(v, &v->stack[v->SP - i], &arr[rec_size.content.xi32 - i]);
        i--;
    }
    inc_sp(v);
    prec *record = allocate_mem_prec(v);
    record->len = rec_size.content.xi32;
    record->objs = arr;

    v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n].type = REC;
    v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n].content.xrec = record;
    inc_ip(v);
}
void push_local_r(virtual_machine *v, uint32_t n)
{
    data p = v->stack[v->FP + 2];
    uint32_t idx = v->stack[v->SP].content.xi32;
    v->stack[v->SP] = v->stack[v->FP + HEADER_SIZE + p.content.xi32 + n].content.xrec->objs[idx];
    inc_ip(v);
}
void push_param_r(virtual_machine *v, uint32_t n)
{
    data idx = v->stack[v->SP];
    data r = v->stack[v->FP + HEADER_SIZE + n];
    prec *item = r.content.xrec;
    data res = item->objs[idx.content.xi32];
    copy_data_by_value(v, &res, &SP_VAL);
    inc_ip(v);
}

void push_stack_r(virtual_machine *v)
{
    data idx = v->stack[v->SP];
    data rec = v->stack[v->SP - 1];
    copy_data_by_value(v, &rec.content.xrec->objs[idx.content.xi32], &SP_VAL);
    inc_ip(v);
}
void push_stack_a(virtual_machine *v)
{
    data idx = v->stack[v->SP];
    data arr = v->stack[v->SP - 1];
    copy_data_by_value(v, &arr.content.xptr->objs[idx.content.xi32], &SP_VAL);
    inc_ip(v);
}

void debug_stack(virtual_machine *v)
{
    if (!v->debug)
        return;

    ty_log(TY_DEBUG, "~~~~~~~~~~~~~ STACK ~~~~~~~~~~~~~~~~~\n");
#if DEBUG_BUILD
    ty_log(TY_DEBUG, "stack state: SP, FP, IP: %d, %d, %d\n", v->SP, v->FP, v->IP);
#endif
    for (uint32_t i = 0; i <= v->SP; i++)
    {
        data d = v->stack[i];
        if (d.type == INT8)
        {
            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, %hhd\n", i, d.type, d.content.xi8);
        }
        else if (d.type == INT32)
        {
            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, %d\n", i, d.type, d.content.xi32);
        }
        else if (d.type == INT64)
        {
            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, %ld\n", i, d.type, d.content.xi64);
        }
        else if (d.type == FLT32)
        {
            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, %f\n", i, d.type, d.content.xf32);
        }
        else if (d.type == FLT64)
        {
            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, %lf\n", i, d.type, d.content.xf64);
        }
        else if (d.type == STR)
        {

            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, %s, count %d\n", i, d.type, (char *)d.content.xstr->obj, 0);
        }
        else if (d.type == PTR)
        {
            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, array type %d, length %d, count %d:\n", i, d.type, d.content.xptr->type, d.content.xptr->len, 0);
            for (uint32_t j = 0; j < d.content.xptr->len; j++)
            {
                if (d.content.xptr->type == INT32)
                    ty_log(TY_DEBUG, "\t├%d, \n", (int)d.content.xptr->objs[j].content.xi32);
                else if (d.content.xptr->type == FLT32)
                    ty_log(TY_DEBUG, "\t├%f, \n", (float)d.content.xptr->objs[j].content.xf32);
                else if (d.content.xptr->type == STR)
                    ty_log(TY_DEBUG, "\t├%s, \n", (char *)d.content.xptr->objs[j].content.xstr->obj);
            }

            if (d.content.xptr->type == 1)
            {
                exit(1);
            }
        }
        else if (d.type == REC)
        {
            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, record type %d, len %d, count %d: \n", i, d.type, d.content.xrec->type, d.content.xrec->len, 0);
            ty_log(TY_DEBUG, "len of record %d\n", d.content.xrec->len);
            for (uint32_t j = 0; j < d.content.xrec->len; j++)
            {
                if (d.content.xrec->objs[j].type == INT32)
                    ty_log(TY_DEBUG, "\t├%d, \n", (int)d.content.xrec->objs[j].content.xi32);
                else if (d.content.xrec->objs[j].type == FLT32)
                    ty_log(TY_DEBUG, "\t├%f, \n", (float)d.content.xrec->objs[j].content.xf32);
                else if (d.content.xrec->objs[j].type == STR)
                    ty_log(TY_DEBUG, "\t├%s, \n", (char *)d.content.xrec->objs[j].content.xstr->obj);
                else if (d.content.xrec->objs[j].type == PTR)
                {
                    ty_log(TY_DEBUG, "\t├array of len %d, \n", d.content.xrec->objs[j].content.xptr->len);
                }
                else if (d.content.xrec->objs[j].type == REC)
                    ty_log(TY_DEBUG, "\t├rec of len %d, \n", d.content.xrec->objs[j].content.xrec->len);

                else
                {
                    ty_log(TY_DEBUG, "unknown type %d, \n", d.content.xrec->objs[j].type);
                    // exit(1);
                    break;
                }
            }
            if (d.content.xrec->type < 0 || d.content.xrec->type > 10)
            {
                // exit(1);
            }
        }
        else if (d.type == FILEPTR)
        {

            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, count %d FILEPTR\n", i, d.type, 0);
        }
        else
        {
            ty_log(TY_DEBUG, "debug stack idx %d, type and content: %d, ---\n", i, d.type);
        }
    }
}

void jneq(virtual_machine *v, uint32_t n)
{
    data d = SP_VAL;
    dec_sp(v);
    inc_ip(v);
    if (d.content.xi32 == 0)
    {
        v->IP = n;
    }
}

void stack_alloc(virtual_machine *v, data src)
{
    if (src.type == STR)
    {
        // src.content.xstr->c++;
    }
    else if (src.type == PTR)
    {
        // src.content.xptr->c++;
    }
    SP_VAL = src;
}

void append(virtual_machine *v)
{
    inc_ip(v);
}
