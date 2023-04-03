
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "topcode.h"

op get_op_by_val(char val)
{
    return topcodes[(int)val];
}

op topcodes[] = {
    {"push_i8", 1},
    {"push_i32", 2},
    {"push_i64", 3},
    {"push_f32", 4},
    {"push_f64", 5},
    {"push_s", 6},
    {"add", 7},
    {"sub", 8},
    {"mul", 9},
    {"div", 10},
    {"rem", 11},
    {"eq", 12},
    {"neq",
     13},
    {"lt",
     14},
    {"lte",
     15},
    {"gt",
     16},
    {"gte",
     17},
    {"and",
     18},
    {"or",
     19},
    {"bit_and", 20},
    {"bit_or", 21},
    {"xor", 22},
    {"l_shift", 23},
    {"r_shift", 24},
    {"bit_not", 25},
    {"not", 26},
    {"push_local",
     27},
    {"set_local",
     28},
    {"set_local_idx",
     29},
    {"push_local_idx",
     30},
    {"set_param",
     31},
    {"set_param_idx",
     32},
    {"push_param",
     33},
    {"push_param_idx",
     34},
    {"set_local_a",
     35},
    {"set_record_idx", 36},
    {"set_param_record_idx", 37},
    {"set_local_r", 38},
    {"push_local_r", 39},
    {"push_param_r", 40},
    {"ret",
     41},
    {"call",
     42},
    {"size",
     43},
    {"stdout",
     44},
    {"stdin",
     45},
    {"goto",
     46},
    {"cast",
     47},
    {"jneq",
     48},
    {"entry",
     49},
    {"noop",
     50},
    {"length",
     51},
    {"fopen", 52},
    {"fread", 53},
    {"fwrite", 54},
    {"fclose", 55},
    {"fdelete", 56},
    {"fseek", 57},
    {"push_stack_r", 58},
    {"push_stack_a", 59},
      {"sin", 60},
    {"cos", 61},
    {"tan", 62},
    {"asin", 63},
    {"acos", 64},
    {"atan", 65},
    {"sinh", 66},
    {"cosh", 67},
    {"tanh", 68},
    {"asinh", 69},
    {"acosh", 70},
    {"atanh", 71},
    {"exp", 72},
    {"log", 73},
    {"log10", 74},
    {"notfound",
     75}

};

op get_op(char *name)
{

    size_t n = sizeof(topcodes) / sizeof(topcodes[0]);
    for (uint32_t i = 0; i < n; i++)
    {

        if (strcmp(name, topcodes[i].name) == 0)
        {
            return topcodes[i];
        }
    }
    return topcodes[n - 1];
}

op get_op_from_int(uint32_t val)
{

    size_t n = sizeof(topcodes) / sizeof(topcodes[0]);
    for (uint32_t i = 0; i < n; i++)
    {

        if (val == (uint32_t)topcodes[i].value)
        {
            return topcodes[i];
        }
    }
    return topcodes[n - 1];
}
