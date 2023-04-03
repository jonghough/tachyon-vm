#include "ty_math.h"
#include "log.h"
#include <math.h>
#include <stdio.h>

#define SWITCH_MATH_OP(math_op)                                                    \
    data in = v->stack[v->SP];                                                     \
    data out = v->stack[v->SP];                                                    \
    double res;                                                                    \
    switch (in.type)                                                               \
    {                                                                              \
    case INT8:                                                                     \
        res = math_op(in.content.xi8);                                             \
        out.type = FLT64;                                                          \
        out.content.xf64 = res;                                                    \
        break;                                                                     \
    case INT32:                                                                    \
        res = math_op(in.content.xi32);                                            \
        out.type = FLT64;                                                          \
        out.content.xf64 = res;                                                    \
        break;                                                                     \
    case INT64:                                                                    \
        res = math_op(in.content.xi64);                                            \
        out.type = FLT64;                                                          \
        out.content.xf64 = res;                                                    \
        break;                                                                     \
    case FLT32:                                                                    \
        res = math_op(in.content.xf32);                                            \
        out.type = FLT64;                                                          \
        out.content.xf64 = res;                                                    \
        break;                                                                     \
    case FLT64:                                                                    \
        res = math_op(in.content.xf64);                                            \
        out.type = FLT64;                                                          \
        out.content.xf64 = res;                                                    \
        break;                                                                     \
    case PTR:                                                                      \
        ty_log(TY_ERROR, "Cannot perform math operation on array type.\n");        \
        exit(1);                                                                   \
        break;                                                                     \
    case STR:                                                                      \
        ty_log(TY_ERROR, "Cannot perform math operation on string type.\n");       \
        exit(1);                                                                   \
        break;                                                                     \
    case REC:                                                                      \
        ty_log(TY_ERROR, "Cannot perform math operation on record type.\n");       \
        exit(1);                                                                   \
        break;                                                                     \
    case FILEPTR:                                                                  \
        ty_log(TY_ERROR, "Cannot perform math operation on file pointer type.\n"); \
        exit(1);                                                                   \
        break;                                                                     \
    case NONE:                                                                     \
        ty_log(TY_ERROR, "Cannot perform math operation on null type.\n");         \
        exit(1);                                                                   \
        break;                                                                     \
    }                                                                              \
    SP_VAL = out;                                                                  \
    inc_ip(v);

#define DEF_MATH_FUNC(func, func_op)   \
    void ty_##func(virtual_machine *v) \
    {                                  \
        SWITCH_MATH_OP(func_op);       \
    }

// trigonometric
DEF_MATH_FUNC(sin, sin)
DEF_MATH_FUNC(cos, cos)
DEF_MATH_FUNC(tan, tan)
DEF_MATH_FUNC(asin, asin)
DEF_MATH_FUNC(acos, acos)
DEF_MATH_FUNC(atan, atan)
// hyperbolic
DEF_MATH_FUNC(sinh, sinh)
DEF_MATH_FUNC(cosh, cosh)
DEF_MATH_FUNC(tanh, tanh)
DEF_MATH_FUNC(asinh, asinh)
DEF_MATH_FUNC(acosh, acosh)
DEF_MATH_FUNC(atanh, atanh)
// exponential
DEF_MATH_FUNC(exp, exp)
DEF_MATH_FUNC(logar, log)
DEF_MATH_FUNC(logar10, log10)


