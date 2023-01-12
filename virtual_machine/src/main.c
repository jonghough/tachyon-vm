#include "vm.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>
#include "program_reader.h"
#include "log.h"
#include "topcode.h"
#include "gc.h"

#define UNUSED(x) (void)(x)

char *read_zip(const char *file_path, const char *program_name, uint32_t *program_length)
{
    return read_program_file(file_path, program_name, program_length);
}

char *read_file(const char *file_path)
{
    FILE *fp;
    fp = fopen(file_path, "rb");
    if (fp == NULL)
    {
        ty_log(TY_ERROR, "Could not read the file located at %s\n", file_path);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *string = malloc(sz + 1);
    fread(string, sz, 1, fp);
    fclose(fp);
    return string;
}

void no_gc(virtual_machine *v, int run_cleanup)
{
    /* nothing to do */
    UNUSED(v);
    UNUSED(run_cleanup);
}

int main(int argc, char *argv[])
{
    int opt;
    int stack_size;
    int heap_size;
    int debug;
    char *program_file;
    int gc_on = 1;
    while ((opt = getopt(argc, argv, "s:h:p:d:g:")) != -1)
    {
        switch (opt)
        {
        case 's':
            if (optarg == NULL)
            {
                ty_log(TY_INFO, "argument for option -s is not given.\n");
            }
            stack_size = atoi(optarg);
            break;
        case 'h':
            if (optarg == NULL)
            {
                ty_log(TY_INFO, "argument for option -h is not given.\n");
            }
            heap_size = atoi(optarg);
            break;
        case 'p':
            if (optarg == NULL)
            {
                ty_log(TY_INFO, "argument for option -p is not given.\n");
            }
            program_file = optarg;
            break;
        case 'd':
            debug = atoi(optarg);
            break;
        case 'g':
            /* run the gc */
            gc_on = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s [-shp] [file...]\n", argv[0]);
            exit(1);
        }
    }

    if (program_file == NULL)
    {
        ty_log(TY_ERROR, "Could not load program file. None given.\n");
        exit(1);
    }

    char copy[strlen(program_file)];
    strcpy(copy, program_file);
    char *file = basename(copy);
    int len = strlen(file);

    file[len - 6] = 0; //.btyarc
    char prog[len];
    memset(prog, 0, strlen(prog));
    strcat(prog, file);
    strcat(prog, "btcode");
    // ty_log(TY_INFO, "program %s\n", prog);
    uint32_t prog_size = 0;
    char *filestr = read_zip(program_file, prog, &prog_size);

    gc_op g;
    if (gc_on)
    { 
        g = run_gc;
    }
    else
    {
        g = no_gc;
    }
    virtual_machine *vm = create_vm(stack_size, heap_size, filestr, prog_size, debug, g);
    initialize(vm);
    run(vm);
    free(filestr);
    exit(0);
}
