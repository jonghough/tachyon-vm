
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "topcode.h"
#include "memory.h"
#include "log.h"
#include "ty_math.h"

virtual_machine *create_vm(long stack_size, long max_heap_size, char *program, uint32_t program_size, uint8_t debug, gc_op gc_op)
{
    virtual_machine *vm = (virtual_machine *)malloc(sizeof(virtual_machine));

    vm->stack_limit = stack_size;
    vm->stack_size = stack_size;
    vm->max_heap_size = max_heap_size;
    vm->prog = (prog *)malloc(sizeof(prog));
    vm->prog->prog = program;
    vm->prog->prog_sz = program_size;
    vm->state = 1;
    vm->stack = (data *)malloc(vm->stack_size * sizeof(data));
    for (uint32_t i = 0; i < vm->stack_size; i++)
    {
        vm->stack[i].type = NONE;
    }
    vm->ST = 0;
    vm->SP = 0;
    vm->FP = 0;
    vm->debug = debug;
    vm->gc_op = gc_op;
    return vm;
}

void destroy_vm(virtual_machine *vm)
{
    vm->gc_op(vm, 1);
#if DEBUG_BUILD
    ty_log(TY_DEBUG, "DESTROYING NOW!\n");
#endif
    for (uint32_t i = 0; i < vm->stack_size; i++)
    {
#if DEBUG_BUILD
        ty_log(TY_DEBUG, "destroy vm, iteration %d\n", i);
#endif
    }
    for (uint32_t i = 0; i < vm->code->size; i++)
    {
        code c = vm->code->code_arr[i];
#if DEBUG_BUILD
        ty_log(TY_DEBUG, "delete code: %s , o1p %d o2p %d\n", c.opcode.name, c.o1p, c.o2p);
#endif
        if (c.o1p > 0)
        {
            free(c.o1);
        }
        if (c.o2p > 0)
        {
            free(c.o2);
        }
    }
    free(vm->stack);
    free(vm->code->code_arr);
    free(vm->code);
    free(vm->prog);
    free(vm);
}

void run(virtual_machine *vm)
{
    uint64_t ctr = 0;
    while (vm->state && vm->IP < vm->code->size)
    {
        if (ctr > 0 && vm->IP == vm->EP)
        {
            break;
        }
        ctr++;
        read_one(vm);
    }
    destroy_vm(vm);
}

void initialize(virtual_machine *v)
{
    char *s = v->prog->prog;          // program bytes
    uint32_t slen = v->prog->prog_sz; // length of program (num bytes)
    uint32_t ptr = 0;                 // pointer to current byte
    uint32_t hlen;                    // header length
    hlen = *(uint32_t *)(s + ptr);
    ptr += 4;
    char header[hlen + 1];
    memcpy(header, &s[ptr], hlen);
    header[hlen] = 0;
    // =========================================================
    // parse the header.
    ptr += hlen;

    int32_t ep = -1;      // entry point
    int32_t prog_sz = -1; // program size, number of lines

    if (hlen > 0)
    {
        char *p2;
        char *tk = strtok_r(header, " ", &p2); // split by spaces
        int header_ctr = 0;
        while (tk != NULL && header_ctr++ >= 0)
        {
            tk = strtok_r(NULL, " ", &p2);
            if (header_ctr == 1)
                ep = atoi(tk);
            else if (header_ctr == 3)
                prog_sz = atoi(tk);
        }
    }
    else
    {
        ty_log(TY_ERROR, "Error. Header informaiton not found. Malformed program.\n");
        exit(1);
    }

    v->IP = ep;
    v->EP = ep;
    if (ep == -1)
    {
        ty_log(TY_ERROR, "Error. Malformed header cannot be parsed correctly.\n");
        exit(1);
    }
    if (prog_sz == -1)
    {
        ty_log(TY_ERROR, "Error. Malformed header cannot be parsed correctly.\n");
        exit(1);
    }
    v->code = (code_vals *)malloc(sizeof(code_vals));
    v->code->size = prog_sz;
    v->code->code_arr = (code *)malloc(prog_sz * sizeof(code));

    int code_ptr = 0;

    while (ptr < slen)
    {
        code *c = v->code->code_arr + code_ptr;
        code_ptr++;
        // =========================================================
        uint32_t c0 = *(uint32_t *)(s + ptr);
        memcpy(&c0, &s[ptr], 4);
        ptr += 4;
        c->idx = c0;
        c->o1p = 0;
        c->o2p = 0;
        uint32_t op0 = *(uint32_t *)(s + ptr);
        ptr += 4;
        c->opcode = get_op_from_int(op0);
        uint32_t arg0len = *(uint32_t *)(s + ptr);
        // =========================================================
        ptr += 4;
        if (arg0len > 0)
        {
            char args[arg0len + 1];
            memcpy(args, &s[ptr], arg0len);
            args[arg0len] = 0;
            ptr += arg0len;
            parse_args(c, args);
        }
    }
}

void parse_args(code *c, char *arg_str)
{

    char *p2;

    c->o1p = 0;
    c->o2p = 0;

    int found_a1 = 0;
    if (c->opcode.value == 0x0006)
    {
        char *tkc = malloc(strlen(arg_str));
        strcpy(tkc, arg_str);

        c->o1 = tkc;
        c->o1p = 1;
    }
    else
    {
        char *tk_inner = strtok_r(arg_str, " ", &p2);
        while (tk_inner != NULL)
        {
            if (!found_a1)
            {
                char *tkc = malloc(strlen(tk_inner));
                strcpy(tkc, tk_inner);
                c->o1 = tkc;
                c->o1p = 1;
                found_a1 = 1;
            }
            else
            {
                char *tkc = malloc(strlen(tk_inner));
                strcpy(tkc, tk_inner);
                c->o2 = tkc;
                c->o2p = 1;
            }

            tk_inner = strtok_r(NULL, "", &p2);
        }
    }
}

void read_one(virtual_machine *vm)
{
    code c = vm->code->code_arr[vm->IP];
#if DEBUG_BUILD
    ty_log(TY_DEBUG, "# =================== selected op is name:\t%-18s, value: %-8d ===================\n", c.opcode.name, c.opcode.value);
    get_heap_size(vm);
#endif
    debug_stack(vm);
    switch (c.opcode.value)
    {
    case 0x0001:
        push_i8(vm, c.o1);
        break;

    case 0x0002:
        push_i32(vm, c.o1);
        break;
    case 0x0003:
        push_i64(vm, c.o1);
        break;
    case 0x0004:
        push_f32(vm, c.o1);
        break;
    case 0x0005:
        push_f64(vm, c.o1);
        break;
    case 0x0006:
        push_s(vm, c.o1);
        break;
    case 0x0007:
        add(vm);
        break;
    case 0x0008:
        sub(vm);
        break;
    case 0x0009:
        mul(vm);
        break;
    case 0x000A:
        divx(vm);
        break;
    case 0x000B:
        rem(vm);
        break;
    case 0x000C:
        eq(vm);
        break;
    case 0x000D:
        neq(vm);
        break;
    case 0x000E:
        lt(vm);
        break;
    case 0x000F:
        lte(vm);
        break;
    case 0x0010:
        gt(vm);
        break;
    case 0x0011:
        gte(vm);
        break;
    case 0x0012:
    {
        and(vm);
        break;
    }
    case 0x0013:
    {
        or (vm);
        break;
    }
    case 0x0014:
    {
        bit_and(vm);
        break;
    }
    case 0x0015:
    {
        bit_or(vm);
        break;
    }
    case 0x0016:
    {
        xor(vm);
        break;
    }
    case 0x0017:
    {
        l_shift(vm);
        break;
    }
    case 0x0018:
    {
        r_shift(vm);
        break;
    }
    case 0x0019:
    {
        bit_not(vm);
        break;
    }
    case 0x001A:
    {
        not(vm);
        break;
    }
    case 0x001B:
    {
        uint32_t plix;
        sscanf(c.o1, "%d", (int *)&plix);
        push_local(vm, plix);
        break;
    }
    case 0x001C:
    {
        uint32_t slix;
        sscanf(c.o1, "%d", (int *)&slix);
        set_local(vm, slix);
        break;
    }
    case 0x001D:
    {
        uint32_t slii;
        sscanf(c.o1, "%d", (int *)&slii);
        set_local_idx(vm, slii);
        break;
    }
    case 0x001E:
    {
        uint32_t pli;
        sscanf(c.o1, "%d", (int *)&pli);
        push_local_idx(vm, pli);
        break;
    }
    case 0x001F:
    {
        uint32_t spi;
        sscanf(c.o1, "%d", (int *)&spi);
        set_param(vm, spi);
        break;
    }
    case 0x0020:
    {
        uint32_t spii;
        sscanf(c.o1, "%d", (int *)&spii);
        set_param_idx(vm, spii);
        break;
    }

    case 0x0021:
    {
        uint32_t ppi;
        sscanf(c.o1, "%d", (int *)&ppi);
        push_param(vm, ppi);
        break;
    }

    case 0x0022:
    {
        uint32_t ppaii;
        sscanf(c.o1, "%d", (int *)&ppaii);
        push_param_idx(vm, ppaii);
        break;
    }
    case 0x0023:
    {
        uint32_t slai;
        sscanf(c.o1, "%d", (int *)&slai);
        set_local_a(vm, slai);
        break;
    }
    case 0x0024:
    {
        uint32_t srii;
        sscanf(c.o1, "%d", (int *)&srii);
        set_record_idx(vm, srii);
        break;
    }
    case 0x0025:
    {

        uint32_t sprii;
        sscanf(c.o1, "%d", (int *)&sprii);
        set_param_record_idx(vm, sprii);
        break;
    }
    case 0x0026:
    {

        uint32_t slri;
        sscanf(c.o1, "%d", (int *)&slri);

        set_local_r(vm, slri);
        break;
    }

    case 0x0027:
    {
        uint32_t plri;
        sscanf(c.o1, "%d", (int *)&plri);
        push_local_r(vm, plri);
        break;
    }

    case 0x0028:
    {
        uint32_t ppri;
        sscanf(c.o1, "%d", (int *)&ppri);
        push_param_r(vm, ppri);
        break;
    }
    case 0x0029:
        ret(vm);
        break;
    case 0x002A:
    {
        uint32_t icall;
        sscanf(c.o1, "%d", (int *)&icall);
        call(vm, icall);
        break;
    }
    case 0x002B:
    {
        uint32_t isizex1;
        sscanf(c.o1, "%d", (int *)&isizex1);
        uint32_t isizex2;
        sscanf(c.o2, "%d", (int *)&isizex2);
        sizex(vm, isizex1, isizex2);
        break;
    }
    case 0x002C:
        stdoutx(vm);
        break;
    case 0x002D:
    {
        readx(vm);
        break;
    }
    case 0x002E:
    {
        uint32_t ixgoto;
        sscanf(c.o1, "%d", (int *)&ixgoto);
        xgoto(vm, ixgoto);
        break;
    }
    case 0x002F:
    {
        cast(vm, c.o1);
        break;
    }
    case 0x0030:
    {
        uint32_t ijneq;
        sscanf(c.o1, "%i", (int *)&ijneq);
        jneq(vm, ijneq);
        break;
    }
    case 0x0031:
        // ENTRY
        break;
    case 0x0032:
        // NOOP
        vm->IP++;
        break;

    case 0x0033:
        length(vm);
        break;
    case 0x0034:
        fopenx(vm);
        break;
    case 0x0035:
        freadx(vm);
        break;
    case 0x0036:
        fwritex(vm);
        break;
    case 0x0037:
        fclosex(vm);
        break;
    case 0x0038:
        fdeletex(vm);
        break;
    case 0x0039:
        fseekx(vm);
        break;
    case 0x003A:
        push_stack_r(vm);
        break;
    case 0x003B:
        push_stack_a(vm);
        break;
    case 0x003C:
        ty_sin(vm);
        break;
    case 0x003D:
        ty_cos(vm);
        break;
    case 0x003E:
        ty_tan(vm);
        break;
    case 0x003F:
        ty_asin(vm);
        break;
    case 0x0040:
        ty_acos(vm);
        break;
    case 0x0041:
        ty_atan(vm);
        break;
    case 0x0042:
        ty_sinh(vm);
        break;
    case 0x0043:
        ty_cosh(vm);
        break;
    case 0x0044:
        ty_tanh(vm);
        break;
    case 0x0045:
        ty_asinh(vm);
        break;
    case 0x0046:
        ty_acosh(vm);
        break;
    case 0x0047:
        ty_atanh(vm);
        break;
    case 0x0048:
        ty_exp(vm);
        break;
    case 0x0049:
        ty_logar(vm);
        break;
    case 0x004A:
        ty_logar10(vm);
        break;
    case 0x004B:
        // not found
        break;
    }
    if (vm->ST < vm->SP)
    {
        vm->ST = vm->SP;
    }
}
