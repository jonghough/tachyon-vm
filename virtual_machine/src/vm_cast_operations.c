
#include <stdlib.h>
#include <stdio.h>
#include "vm_operations.h"
#include "vm_defs.h"
#include "memory.h"
#include "log.h"

#define DEFINE_SIMPLE_CAST_FUNC(func, to_enum, to_type, to_data, from_enum, from_type, from_data, str_fmt) \
    void func(virtual_machine *v, data *src)                                                               \
    {                                                                                                      \
        if (to_enum < 6 && to_enum > 0)                                                                    \
        {                                                                                                  \
            src->type = to_enum;                                                                           \
            src->content.to_data = (to_type)src->content.from_data;                                        \
        }                                                                                                  \
        else if (to_enum == STR)                                                                           \
        {                                                                                                  \
            pmem *p = alloc_str(v, 20);                                                                    \
            p->len = 20;                                                                                   \
            sprintf(p->obj, str_fmt, src->content.from_data);                                              \
            src->type = to_enum;                                                                           \
            src->content.xstr = p;                                                                         \
        }                                                                                                  \
        else                                                                                               \
        {                                                                                                  \
            ty_log(TY_ERROR, "Error casting value.\n");                                                    \
            exit(1);                                                                                       \
        }                                                                                                  \
    }

#define CAST_FOR_TYPE(name, type_enum, type, type_data, str_fmt)                                        \
    DEFINE_SIMPLE_CAST_FUNC(name##_to_int8, INT8, int8_t, xi8, type_enum, type, type_data, str_fmt)     \
    DEFINE_SIMPLE_CAST_FUNC(name##_to_int32, INT32, int32_t, xi32, type_enum, type, type_data, str_fmt) \
    DEFINE_SIMPLE_CAST_FUNC(name##_to_int64, INT64, int64_t, xi64, type_enum, type, type_data, str_fmt) \
    DEFINE_SIMPLE_CAST_FUNC(name##_to_flt32, FLT32, float, xf32, type_enum, type, type_data, str_fmt)   \
    DEFINE_SIMPLE_CAST_FUNC(name##_to_flt64, FLT64, double, xf64, type_enum, type, type_data, str_fmt)  \
    DEFINE_SIMPLE_CAST_FUNC(name##_to_str, STR, int32_t, xi32, type_enum, type, type_data, str_fmt)

CAST_FOR_TYPE(int8, INT8, int8_t, xi8, "%hhd")
CAST_FOR_TYPE(int32, INT32, int32_t, xi32, "%d")
CAST_FOR_TYPE(int64, INT64, int64_t, xi64, "%ld")
CAST_FOR_TYPE(flt32, FLT32, float, xf32, "%f")
CAST_FOR_TYPE(flt64, FLT64, double, xf64, "%f")

void cast(virtual_machine *v, char *t)
{
    data r = SP_VAL;

    if (strcmp(t, "i8") == 0)
    {
        if (r.type == FLT32)
        {
            flt32_to_int8(v, &r);
        }
        else if (r.type == FLT64)
        {
            flt64_to_int8(v, &r);
        }
        else if (r.type == INT64)
        {
            int64_to_int8(v, &r);
        }

        else if (r.type == INT8)
        {
            int8_to_int8(v, &r);
        }
        else if (r.type == STR)
        {
            uint8_t i1;
            sscanf((char *)(r.content.xstr->obj), "%hhd", (int8_t *)&i1);
            r.content.xi8 = i1;
            r.type = INT8;
        }
    }
    else if (strcmp(t, "i32") == 0)
    {
        if (r.type == FLT32)
        {
            flt32_to_int32(v, &r);
        }
        else if (r.type == FLT64)
        {
            flt64_to_int32(v, &r);
        }
        else if (r.type == INT64)
        {
            int64_to_int32(v, &r);
        }

        else if (r.type == INT8)
        {
            int8_to_int32(v, &r);
        }
        else if (r.type == STR)
        {
            uint32_t i1;
            sscanf((char *)(r.content.xstr->obj), "%d", (int *)&i1);
            r.content.xi32 = i1;
            r.type = INT32;
        }
    }
    else if (strcmp(t, "i64") == 0)
    {
        if (r.type == FLT32)
        {
            flt32_to_int64(v, &r);
        }
        else if (r.type == FLT64)
        {
            flt64_to_int64(v, &r);
        }
        else if (r.type == INT32)
        {
            int32_to_int64(v, &r);
        }
        else if (r.type == INT8)
        {
            int8_to_int64(v, &r);
        }
        else if (r.type == STR)
        {
            int64_t i1;
            sscanf((char *)(r.content.xstr->obj), "%ld", (long *)&i1);
            r.content.xi64 = i1;
            r.type = INT64;
        }
    }
    else if (strcmp(t, "f32") == 0)
    {
        if (r.type == INT64)
        {
            int64_to_flt32(v, &r);
        }
        else if (r.type == FLT64)
        {
            flt64_to_flt32(v, &r);
        }
        else if (r.type == INT32)
        {
            int32_to_flt32(v, &r);
        }

        else if (r.type == INT8)
        {
            int8_to_flt32(v, &r);
        }
        else if (r.type == STR)
        {
            float f;
            sscanf((char *)(r.content.xstr->obj), "%f", &f);
            r.content.xf32 = f;
            r.type = FLT32;
        }
    }
    else if (strcmp(t, "f64") == 0)
    {
        if (r.type == INT64)
        {
            int64_to_flt64(v, &r);
        }
        else if (r.type == FLT32)
        {
            flt64_to_flt64(v, &r);
        }
        else if (r.type == INT32)
        {
            int32_to_flt64(v, &r);
        }
        else if (r.type == INT8)
        {
            int8_to_flt64(v, &r);
        }
        else if (r.type == STR)
        {
            double f;
            sscanf((char *)(r.content.xstr->obj), "%lf", &f);
            r.content.xf64 = f;
            r.type = FLT64;
        }
    }
    else if (strcmp(t, "str") == 0)
    {
        if (r.type == INT32)
        {
            int32_to_str(v, &r);
        }
        else if (r.type == INT64)
        {
            int64_to_str(v, &r);
        }
        else if (r.type == FLT32)
        {
            flt32_to_str(v, &r);
        }
        else if (r.type == FLT64)
        {
            flt64_to_str(v, &r);
        }
    }
    else
    {
        // bad thing
        ty_log(TY_ERROR, "Error trying to cast types.\n");
        exit(1);
    }
    stack_alloc(v, r);
    inc_ip(v);
}
