#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vm_binary_operations.h"

#define INT_BINARY_OPX(src1, data1, src2, data2, dest, dest_type, dest_data, operator) \
    dest->type = dest_type;                                                            \
    dest->content.dest_data = (src2->content.data2 operator src1->content.data1);

// ----------------------------------------------
// DEFINES implicit integer type promotions
// i8 --> i32 --> i64
// ----------------------------------------------
#define DEFINE_INT_BOPX(d1, d2, t1, t2, op_name, operator)             \
    void d1##d2##op_name(data *src1, data *src2, data *dest)           \
    {                                                                  \
        if (t1 >= t2)                                                  \
        {                                                              \
            INT_BINARY_OPX(src1, d1, src2, d2, dest, t1, d1, operator) \
        }                                                              \
        else                                                           \
        {                                                              \
            INT_BINARY_OPX(src1, d1, src2, d2, dest, t2, d2, operator) \
        }                                                              \
    }

#define DEFINE_INT_BOPS(d1, d2, t1, t2)          \
    DEFINE_INT_BOPX(d1, d2, t1, t2, bit_and, &)  \
    DEFINE_INT_BOPX(d1, d2, t1, t2, bit_or, |)   \
    DEFINE_INT_BOPX(d1, d2, t1, t2, xor, ^)      \
    DEFINE_INT_BOPX(d1, d2, t1, t2, l_shift, <<) \
    DEFINE_INT_BOPX(d1, d2, t1, t2, r_shift, >>)

DEFINE_INT_BOPS(xi8, xi8, INT8, INT8)
DEFINE_INT_BOPS(xi8, xi32, INT8, INT32)
DEFINE_INT_BOPS(xi8, xi64, INT8, INT64)

DEFINE_INT_BOPS(xi32, xi8, INT32, INT8)
DEFINE_INT_BOPS(xi32, xi32, INT32, INT32)
DEFINE_INT_BOPS(xi32, xi64, INT32, INT64)

DEFINE_INT_BOPS(xi64, xi8, INT64, INT8)
DEFINE_INT_BOPS(xi64, xi32, INT64, INT32)
DEFINE_INT_BOPS(xi64, xi64, INT64, INT64)

#define DEFINE_INT_BINOPX(func, op)                    \
    void func(virtual_machine *v)                      \
    {                                                  \
        data t0,                                       \
            t1;                                        \
        t0 = SP_VAL;                                   \
        dec_sp(v); /*careful here*/                    \
        t1 = SP_VAL;                                   \
        data dest = SP_VAL;                            \
                                                       \
        switch (t0.type)                               \
        {                                              \
        case INT8:                                     \
        {                                              \
            switch (t1.type)                           \
            {                                          \
            case INT8:                                 \
                xi8xi8##func((&t0), (&t1), (&dest));   \
                break;                                 \
            case INT32:                                \
                xi8xi32##func((&t0), (&t1), (&dest));  \
                break;                                 \
            case INT64:                                \
                xi8xi64##func((&t0), (&t1), (&dest));  \
                break;                                 \
            default:                                   \
                break;                                 \
            }                                          \
            break;                                     \
        }                                              \
        case INT32:                                    \
        {                                              \
            switch (t1.type)                           \
            {                                          \
            case INT8:                                 \
                xi32xi8##func((&t0), (&t1), (&dest));  \
                break;                                 \
            case INT32:                                \
                xi32xi32##func((&t0), (&t1), (&dest)); \
                break;                                 \
            case INT64:                                \
                xi32xi64##func((&t0), (&t1), (&dest)); \
            default:                                   \
                break;                                 \
            }                                          \
            break;                                     \
        }                                              \
        case INT64:                                    \
        {                                              \
            switch (t1.type)                           \
            {                                          \
            case INT8:                                 \
                xi64xi8##func((&t0), (&t1), (&dest));  \
                break;                                 \
            case INT32:                                \
                xi64xi32##func((&t0), (&t1), (&dest)); \
                break;                                 \
            case INT64:                                \
                xi64xi64##func((&t0), (&t1), (&dest)); \
                break;                                 \
            default:                                   \
                break;                                 \
            }                                          \
            break;                                     \
        }                                              \
                                                       \
        default:                                       \
            break;                                     \
        }                                              \
        SP_VAL = dest;                                 \
                                                       \
        inc_ip(v);                                     \
    }

#define BINARY_OPX(src1, data1, src2, data2, dest, dest_type, dest_data, op) \
    dest->type = dest_type;                                                  \
    dest->content.dest_data = src2->content.data2 op src1->content.data1;

// ----------------------------------------------
// DEFINES implicit type promotions
// i8 --> i32 --> i64 --> f32 --> f64
// ----------------------------------------------
#define DEFINE_BOPX(d1, d2, t1, t2, op_name, operator)                             \
    void d1##d2##op_name(data *src1, data *src2, data *dest)                       \
    {                                                                              \
        char *k = #operator;                                                       \
        if (strcmp(k, "==") == 0 || strcmp(k, "<=") == 0 || strcmp(k, "<") == 0 || \
            strcmp(k, ">=") == 0 || strcmp(k, ">") == 0 || strcmp(k, "!=") == 0)   \
        {                                                                          \
            BINARY_OPX(src1, d1, src2, d2, dest, INT32, xi32, operator)            \
        }                                                                          \
        else if (t1 >= t2)                                                         \
        {                                                                          \
            BINARY_OPX(src1, d1, src2, d2, dest, t1, d1, operator)                 \
        }                                                                          \
        else if (t2 > t1)                                                          \
        {                                                                          \
            BINARY_OPX(src1, d1, src2, d2, dest, t2, d2, operator)                 \
        }                                                                          \
    }

#define DEFINE_BOPS(d1, d2, t1, t2)      \
    DEFINE_BOPX(d1, d2, t1, t2, add, +)  \
    DEFINE_BOPX(d1, d2, t1, t2, sub, -)  \
    DEFINE_BOPX(d1, d2, t1, t2, mul, *)  \
    DEFINE_BOPX(d1, d2, t1, t2, divx, /) \
    DEFINE_BOPX(d1, d2, t1, t2, eq, ==)  \
    DEFINE_BOPX(d1, d2, t1, t2, neq, !=) \
    DEFINE_BOPX(d1, d2, t1, t2, lt, <)   \
    DEFINE_BOPX(d1, d2, t1, t2, lte, <=) \
    DEFINE_BOPX(d1, d2, t1, t2, gt, >)   \
    DEFINE_BOPX(d1, d2, t1, t2, gte, >=) \
    DEFINE_BOPX(d1, d2, t1, t2, and, &&) \
    DEFINE_BOPX(d1, d2, t1, t2, or, ||)

DEFINE_BOPS(xi8, xi8, INT8, INT8)
DEFINE_BOPS(xi8, xi32, INT8, INT32)
DEFINE_BOPS(xi8, xi64, INT8, INT64)
DEFINE_BOPS(xi8, xf32, INT8, FLT32)
DEFINE_BOPS(xi8, xf64, INT8, FLT64)

DEFINE_BOPS(xi32, xi8, INT32, INT8)
DEFINE_BOPS(xi32, xi32, INT32, INT32)
DEFINE_BOPS(xi32, xi64, INT32, INT64)
DEFINE_BOPS(xi32, xf32, INT32, FLT32)
DEFINE_BOPS(xi32, xf64, INT32, FLT64)

DEFINE_BOPS(xi64, xi8, INT64, INT8)
DEFINE_BOPS(xi64, xi32, INT64, INT32)
DEFINE_BOPS(xi64, xi64, INT64, INT64)
DEFINE_BOPS(xi64, xf32, INT64, FLT32)
DEFINE_BOPS(xi64, xf64, INT64, FLT64)

DEFINE_BOPS(xf32, xi8, FLT32, INT8)
DEFINE_BOPS(xf32, xi32, FLT32, INT32)
DEFINE_BOPS(xf32, xi64, FLT32, INT64)
DEFINE_BOPS(xf32, xf32, FLT32, FLT32)
DEFINE_BOPS(xf32, xf64, FLT32, FLT64)

DEFINE_BOPS(xf64, xi8, FLT64, INT8)
DEFINE_BOPS(xf64, xi32, FLT64, INT32)
DEFINE_BOPS(xf64, xi64, FLT64, INT64)
DEFINE_BOPS(xf64, xf32, FLT64, FLT32)
DEFINE_BOPS(xf64, xf64, FLT64, FLT64)

#define DEFINE_BINOPX(func, op)                                                                           \
    void func(virtual_machine *v)                                                                         \
    {                                                                                                     \
        data t0,                                                                                          \
            t1;                                                                                           \
        t0 = SP_VAL;                                                                                      \
        dec_sp(v); /*careful here*/                                                                       \
        t1 = SP_VAL;                                                                                      \
        data dest = SP_VAL;                                                                               \
        if (t0.type == STR && t1.type == STR)                                                             \
        {                                                                                                 \
            char *k = #op;                                                                                \
            if (strcmp(k, "+") == 0)                                                                      \
            {                                                                                             \
                dest.type = STR;                                                                          \
                uint32_t len = t1.content.xstr->len + t0.content.xstr->len;                               \
                pmem *p = alloc_str(v, len);                                                              \
                dest.content.xstr = p;                                                                    \
                strcat(p->obj, t1.content.xstr->obj);                                                     \
                strcat(p->obj, t0.content.xstr->obj);                                                     \
            }                                                                                             \
            else if (strcmp(k, "==") == 0)                                                                \
            {                                                                                             \
                dest.type = INT32;                                                                        \
                dest.content.xi32 = strcmp(t0.content.xstr->obj, t0.content.xstr->obj) == 0;              \
            }                                                                                             \
            else if (strcmp(k, "!=") == 0)                                                                \
            {                                                                                             \
                dest.type = INT32;                                                                        \
                dest.content.xi32 = strcmp(t1.content.xstr->obj, t0.content.xstr->obj) != 0;              \
            }                                                                                             \
        }                                                                                                 \
        else if (t0.type == PTR && t1.type == PTR)                                                        \
        {                                                                                                 \
            char *k = #op;                                                                                \
            if (strcmp(k, "+") == 0)                                                                      \
            {                                                                                             \
                dest.type = PTR;                                                                          \
                uint32_t len = t1.content.xstr->len + t0.content.xstr->len;                               \
                parr *p;                                                                                  \
                if (t0.content.xptr->type == INT32)                                                       \
                {                                                                                         \
                    p = alloc_int32_arr(v, len);                                                          \
                }                                                                                         \
                else if (t0.content.xptr->type == INT64)                                                  \
                {                                                                                         \
                    p = alloc_int64_arr(v, len);                                                          \
                }                                                                                         \
                else if (t0.type == FLT32)                                                                \
                {                                                                                         \
                    p = alloc_float32_arr(v, (size_t)len);                                                \
                }                                                                                         \
                else if (t0.type == FLT64)                                                                \
                {                                                                                         \
                    p = alloc_float64_arr(v, (size_t)len);                                                \
                }                                                                                         \
                else if (t1.type == STR)                                                                  \
                {                                                                                         \
                    p = alloc_str_arr(v, len);                                                            \
                }                                                                                         \
                for (uint32_t i = 0; i < t1.content.xptr->len; i++)                                       \
                {                                                                                         \
                    copy_data_by_value(v, &t1.content.xptr->objs[i], &p->objs[i]);                        \
                }                                                                                         \
                p->len = len;                                                                             \
                for (uint32_t i = 0; i < t0.content.xptr->len; i++)                                       \
                {                                                                                         \
                    copy_data_by_value(v, &t0.content.xptr->objs[i], &p->objs[t1.content.xptr->len + i]); \
                }                                                                                         \
                dest.content.xptr = p;                                                                    \
                dest.type = PTR;                                                                          \
            }                                                                                             \
        }                                                                                                 \
        else                                                                                              \
        {                                                                                                 \
            switch (t0.type)                                                                              \
            {                                                                                             \
            case INT8:                                                                                    \
            {                                                                                             \
                switch (t1.type)                                                                          \
                {                                                                                         \
                case INT8:                                                                                \
                    xi8xi8##func((&t0), (&t1), (&dest));                                                  \
                    break;                                                                                \
                case INT32:                                                                               \
                    xi8xi32##func((&t0), (&t1), (&dest));                                                 \
                    break;                                                                                \
                case INT64:                                                                               \
                    xi8xi64##func((&t0), (&t1), (&dest));                                                 \
                    break;                                                                                \
                case FLT32:                                                                               \
                    xi8xf32##func((&t0), (&t1), (&dest));                                                 \
                    break;                                                                                \
                case FLT64:                                                                               \
                    xi8xf64##func((&t0), (&t1), (&dest));                                                 \
                    break;                                                                                \
                default:                                                                                  \
                    break;                                                                                \
                }                                                                                         \
                break;                                                                                    \
            }                                                                                             \
            case INT32:                                                                                   \
            {                                                                                             \
                switch (t1.type)                                                                          \
                {                                                                                         \
                case INT8:                                                                                \
                    xi32xi8##func((&t0), (&t1), (&dest));                                                 \
                    break;                                                                                \
                case INT32:                                                                               \
                    xi32xi32##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case INT64:                                                                               \
                    xi32xi64##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case FLT32:                                                                               \
                    xi32xf32##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case FLT64:                                                                               \
                    xi32xf64##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                default:                                                                                  \
                    break;                                                                                \
                }                                                                                         \
                break;                                                                                    \
            }                                                                                             \
            case INT64:                                                                                   \
            {                                                                                             \
                switch (t1.type)                                                                          \
                {                                                                                         \
                case INT8:                                                                                \
                    xi64xi8##func((&t0), (&t1), (&dest));                                                 \
                    break;                                                                                \
                case INT32:                                                                               \
                    xi64xi32##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case INT64:                                                                               \
                    xi64xi64##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case FLT32:                                                                               \
                    xi64xf32##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case FLT64:                                                                               \
                    xi64xf64##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                default:                                                                                  \
                    break;                                                                                \
                }                                                                                         \
                break;                                                                                    \
            }                                                                                             \
            case FLT32:                                                                                   \
            {                                                                                             \
                switch (t1.type)                                                                          \
                {                                                                                         \
                case INT8:                                                                                \
                    xf32xi8##func((&t0), (&t1), (&dest));                                                 \
                    break;                                                                                \
                case INT32:                                                                               \
                    xf32xi32##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case INT64:                                                                               \
                    xf32xi64##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case FLT32:                                                                               \
                    xf32xf32##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case FLT64:                                                                               \
                    xf32xf64##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                default:                                                                                  \
                    break;                                                                                \
                }                                                                                         \
                break;                                                                                    \
            }                                                                                             \
            case FLT64:                                                                                   \
            {                                                                                             \
                switch (t1.type)                                                                          \
                {                                                                                         \
                case INT8:                                                                                \
                    xf64xi8##func((&t0), (&t1), (&dest));                                                 \
                    break;                                                                                \
                case INT32:                                                                               \
                    xf64xi32##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case INT64:                                                                               \
                    xf64xi64##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case FLT32:                                                                               \
                    xf64xf32##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                case FLT64:                                                                               \
                    xf64xf64##func((&t0), (&t1), (&dest));                                                \
                    break;                                                                                \
                default:                                                                                  \
                    break;                                                                                \
                }                                                                                         \
                break;                                                                                    \
            }                                                                                             \
            default:                                                                                      \
                break;                                                                                    \
            }                                                                                             \
        }                                                                                                 \
        SP_VAL = dest;                                                                                    \
        if (t0.type == NONE)                                                                              \
        {                                                                                                 \
        }                                                                                                 \
        else if (t0.type == STR)                                                                          \
        {                                                                                                 \
        }                                                                                                 \
        else if (t0.type == PTR)                                                                          \
        {                                                                                                 \
        }                                                                                                 \
        if (t1.type == NONE)                                                                              \
        {                                                                                                 \
        }                                                                                                 \
        else if (t1.type == STR)                                                                          \
        {                                                                                                 \
        }                                                                                                 \
        else if (t1.type == PTR)                                                                          \
        {                                                                                                 \
        }                                                                                                 \
        inc_ip(v);                                                                                        \
    }

DEFINE_BINOPX(add, +)
DEFINE_BINOPX(sub, -)
DEFINE_BINOPX(mul, *)
DEFINE_BINOPX(divx, /)
DEFINE_BINOPX(eq, ==)
DEFINE_BINOPX(neq, !=)
DEFINE_BINOPX(lt, <)
DEFINE_BINOPX(lte, <=)
DEFINE_BINOPX(gt, >)
DEFINE_BINOPX(gte, >=)
DEFINE_BINOPX(and, &&)
DEFINE_BINOPX(or, ||)

DEFINE_INT_BINOPX(bit_and, &)
DEFINE_INT_BINOPX(bit_or, |)
DEFINE_INT_BINOPX(xor, ^)
DEFINE_INT_BINOPX(l_shift, <<)
DEFINE_INT_BINOPX(r_shift, >>)
