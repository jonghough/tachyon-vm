#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "vm_operations.h"
#include "vm_defs.h"
#include "memory.h"
#include "log.h"

void fopenx(virtual_machine *v)
{
    data mode = v->stack[v->SP];
    dec_sp(v);
    data file_name = v->stack[v->SP];
    dec_sp(v);
    FILE *fp = fopen(file_name.content.xstr->obj, mode.content.xstr->obj);
    inc_sp(v);

    pfile *pf = allocate_mem_pfile(v);
    pf->file = fp; 
    data *data_fptr = &v->stack[v->SP];
    data_fptr->content.xfileptr = pf;
    data_fptr->type = FILEPTR;
    inc_ip(v);
}
void freadx(virtual_machine *v)
{
    struct stat file_stat;

    data bytes = v->stack[v->SP];
    dec_sp(v);
    data pf = v->stack[v->SP];
    dec_sp(v);

    if (fstat(fileno(pf.content.xfileptr->file), &file_stat) < 0)
    {
        ty_log(TY_ERROR,"Error reading file.");
        exit(1);
    }
    off_t i = file_stat.st_size;

    pmem *mem = alloc_str(v,(size_t)i);
    size_t b = (size_t)bytes.content.xi32;
    if (bytes.content.xi32 > (int32_t)i)
    {
        b = (size_t)i;
    }
    fread(mem->obj, b, 1, pf.content.xfileptr->file);
    inc_sp(v);

    data d = v->stack[v->SP];
    d.content.xstr = mem;
    d.type = STR;
    SP_VAL = d;
    inc_ip(v);
}
void fwritex(virtual_machine *v)
{
    data bytes = v->stack[v->SP];
    dec_sp(v);
    data pf = v->stack[v->SP];
    dec_sp(v);
    fwrite(bytes.content.xstr->obj, bytes.content.xstr->len, 1, pf.content.xfileptr->file);
    inc_ip(v);
}
void fclosex(virtual_machine *v)
{
    data pf = v->stack[v->SP];
    dec_sp(v);
    fclose(pf.content.xfileptr->file);
    inc_ip(v);
}
void fdeletex(virtual_machine *v)
{
    data file_name = v->stack[v->SP];
    dec_sp(v);
    int r = remove(file_name.content.xstr->obj);
    if (r != 0)
    {
        ty_log(TY_ERROR,"Unable to delete file %s.\n", (char *)file_name.content.xstr->obj);
    }
    inc_ip(v);
}

void fseekx(virtual_machine *v)
{ 
    data whence = v->stack[v->SP];
    dec_sp(v);
    data offset = v->stack[v->SP];
    dec_sp(v);
    data pf = v->stack[v->SP];
    dec_sp(v);
    int output = fseek( pf.content.xfileptr->file,offset.content.xi64,whence.content.xi32);
    if(output != 0){
        ty_log(TY_ERROR,"Failed fseek on file.\n");
        exit(1);
    }
    inc_sp(v);

    data d = v->stack[v->SP];
    d.content.xi32 = output;
    d.type = INT32;
    SP_VAL = d;
    inc_ip(v);
}
