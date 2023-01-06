#ifndef DATATYPES_H
#define DATATYPES_H
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct parr parr;
typedef struct pmem pmem;
typedef struct prec prec;
typedef struct pfile pfile;
typedef struct gc_obj gc_obj;

typedef enum dtype
{
    NONE,
    INT8,
    INT32,
    INT64,
    FLT32,
    FLT64,
    PTR,
    STR,
    REC,
    FILEPTR
} dtype;

typedef enum gctype
{
    GCDATA,
    GCARR,
    GCSTR,
    GCREC,
    GCFILEPTR
} gctype;

typedef struct data_x
{

    union
    {
        int8_t xi8;
        int32_t xi32;
        int64_t xi64;
        float xf32;
        double xf64;
        parr *xptr; // array of pmem
        pmem *xstr;
        prec *xrec;
        pfile *xfileptr;

    } content;
    dtype type;
    uint8_t marked : 1;
} data;

struct gc_obj
{
    gctype type;
    union
    {
        parr *xptr;
        pmem *xstr;
        prec *xrec;
        pfile *xfileptr;
        data *xd;

    } content;
    gc_obj *prev;
    gc_obj *next;
};
struct pmem
{
    dtype type;
    uint32_t len;
    void *obj;
    uint8_t marked : 1;
};

struct parr
{
    dtype type;
    uint32_t len;
    data *objs;
    uint8_t marked : 1;
};

struct prec
{
    dtype type;
    uint32_t len;
    data *objs;
    uint8_t marked : 1;
};

struct pfile
{
    FILE *file;
    uint8_t marked : 1;
};

#endif
