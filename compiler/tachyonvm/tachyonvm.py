import shlex

import click

from tachyonvm.Datatypes import Record


class TachyonVM:

    def __init__(self, program: str, debug: bool = False) -> None:
        self.program = program
        self.debug = debug
        self.datastack: list = [None] * 480
        self.header = None
        self.code = []
        self.stackPtr: int = 0
        self.framePtr: int = 0
        self.instructionPtr = 0
        self.EP = 0
        self._finish = False
        self.INSTR_PTR = 0
        self.debug_str = ""
        self._create_code_vector()

    def _create_code_vector(self):
        program = self.program.split("\n")
        self.header = program[0]
        self.instructionPtr = int(self.header.split(' ')[1])
        self.EP = self.instructionPtr
        self.code = program[1:]
        for i in range(len(self.code)):
            code = self.code[i]
            c = code.split(":")
            self.code[i] = ' '.join(c[1:]).strip()
        self.INSTR_PTR = len(self.code)

    def run(self):
        self.datastack[0] = 0
        ops = 0
        while self.instructionPtr < len(self.code):
            if self.debug:
                print(f"Iteration of instruction loop: iteration: {ops} -- instruction pointer: {self.instructionPtr}")
            if ops > 0 and self.instructionPtr == self.EP:
                return
            instruction_with_args = self.read_one_instruction()
            instruction = instruction_with_args[0]
            if len(instruction_with_args) > 1:
                args = instruction_with_args[1:]
            else:
                args = None
            self._execute_instruction(instruction, args)
            ops += 1
        print(self.debug_str)

    def _execute_instruction(self, instruction, args):
        if self.debug:
            print(f"executing instruction : {instruction}, with args : {args}")
            print(
                f"SP: {self.stackPtr}, FP: {self.framePtr}, IP: {self.instructionPtr}\n stack before execution: {self.datastack[:self.stackPtr + 1]}")
        if (not args or args == (None,)) and instruction not in ("and", "or", "not"):
            getattr(self, instruction)()
        else:
            if instruction in ("add", "sub", "mul", "div", "rem", "ret", "eq", "neq",
                               "stdout", "noop", "gte", "bit_and", "bit_or", "xor",
                               "bit_not", "l_shift", "r_shift", "length", "push_stack_r",
                                "push_stack?a"
                               ):
                getattr(self, instruction)()
            elif instruction in (
                    "APPEND",
                    "set_local_a", "push_i8", "push_i32", "push_i64", "push_f32",
                    "push_f64", "push_s", "call", "goto",
                    "set_param", "push_local_idx", "set_local_idx", "push_param_idx", "set_param_idx",
                    "push_param", "set_local", "push_local", "set_record_idx",
                    "set_param_record_idx",
                    "set_local_r", "push_local_r", "push_param_r",
                    "jneq", "cast", "fopen", "fclose", "fread", "fwrite", "fdelete"):
                getattr(self, instruction)(args[0])
            elif instruction in ("size",):
                getattr(self, instruction)(args[0], args[1])
            elif instruction in ("and", "or", "not"):
                getattr(self, instruction + "x")()
            else:
                raise ValueError(f"unknown instruction {instruction}")

    def infer_type(self, data):
        # type inference should be handled by compiler
        return data

    def read_one_instruction(self):
        it = self.code[self.instructionPtr]
        if self.debug:
            print(f"raw instruction {it} , and instruction pointer {self.instructionPtr}")

        it = shlex.split(it)

        return it

    def call(self, label):
        self.INSTR_PTR = self.instructionPtr
        self.instructionPtr = int(label)

    def noop(self):
        self.instructionPtr += 1

    def pushx(self, val):
        self.stackPtr += 1
        self.datastack[self.stackPtr] = val
        self.instructionPtr += 1

    def APPEND(self, val):
        self.instructionPtr += 1

    def set_record_idx(self, n: int):

        p = self.datastack[self.framePtr + 2]
        idx = self.datastack[self.stackPtr]
        val = self.datastack[self.stackPtr - 1]
        self.datastack[self.framePtr + 2 + 1 + 1 + int(p) + int(n)].data[int(idx)] = val
        self.stackPtr -= 1
        self.instructionPtr += 1

    def set_local_r(self, n: int):
        record_size = self.datastack[self.stackPtr]
        p = self.datastack[self.framePtr + 2]
        data = []
        i = record_size
        while i > 0:
            data.append(self.datastack[self.stackPtr - i])
            i -= 1

        self.stackPtr += 1
        rec = Record()
        rec.data = data
        self.datastack[self.stackPtr] = rec
        self.datastack[self.framePtr + 2 + 1 + 1 + int(p) + int(n)] = self.datastack[self.stackPtr]

        self.instructionPtr += 1

    def push_i8(self, val):
        if self.debug:
            print(f"pushing value {val} of type {type(val)}")
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(val)
        self.instructionPtr += 1

    def push_i32(self, val):
        if self.debug:
            print(f"pushing value {val} of type {type(val)}")
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(val)
        self.instructionPtr += 1

    def push_i64(self, val):
        if self.debug:
            print(f"pushing value {val} of type {type(val)}")
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(val)
        self.instructionPtr += 1

    def push_f32(self, val):
        if self.debug:
            print(f"pushing value {val} of type {type(val)}")
        self.stackPtr += 1
        self.datastack[self.stackPtr] = float(val)
        self.instructionPtr += 1

    def push_f64(self, val):
        if self.debug:
            print(f"pushing value {val} of type {type(val)}")
        self.stackPtr += 1
        self.datastack[self.stackPtr] = float(val)
        self.instructionPtr += 1

    def push_s(self, val):
        if self.debug:
            print(f"pushing value {val} of type {type(val)}")
        self.stackPtr += 1
        self.datastack[self.stackPtr] = str(val)
        self.instructionPtr += 1

    def set_local(self, n):
        p = self.datastack[self.framePtr + 2]
        self.datastack[self.framePtr + 2 + 1 + 1 + int(p) + int(n)] = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        self.instructionPtr += 1

    def set_local_idx(self, n):
        p = self.datastack[self.framePtr + 2]
        val = self.datastack[self.stackPtr]
        idx = self.datastack[self.stackPtr - 1]
        self.datastack[self.framePtr + 2 + 1 + 1 + int(p) + int(n)][int(idx)] = val
        self.stackPtr -= 2
        self.instructionPtr += 1

    def set_local_a(self, n):
        p = self.datastack[self.framePtr + 2]
        size = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        array = []
        ptr = self.stackPtr - int(size) + 1
        while ptr <= self.stackPtr:
            array.append(self.datastack[ptr])
            ptr += 1
        self.stackPtr = self.stackPtr - int(size)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = array
        self.datastack[self.framePtr + 2 + 1 + 1 + int(p) + int(n)] = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        self.instructionPtr += 1

    def push_local(self, n):
        p = self.datastack[self.framePtr + 2]
        self.stackPtr += 1
        self.datastack[self.stackPtr] = self.datastack[self.framePtr + 4 + int(p) + int(n)]

        self.instructionPtr += 1

    def push_local_r(self, n):
        p = self.datastack[self.framePtr + 2]
        idx = int(self.datastack[self.stackPtr])
        self.datastack[self.stackPtr] = self.datastack[self.framePtr + 4 + int(p) + int(n)].data[idx]

        self.instructionPtr += 1
    def push_stack_r(self):
        idx = int(self.datastack[self.stackPtr])
        rec = self.datastack[self.stackPtr-1]
        self.datastack[self.stackPtr] = rec.data[idx]

        self.instructionPtr += 1
    def push_stack_a(self):
        idx = int(self.datastack[self.stackPtr])
        arr = self.datastack[self.stackPtr-1]
        self.datastack[self.stackPtr] = arr[idx]

        self.instructionPtr += 1

    def push_local_idx(self, n):
        p = self.datastack[self.framePtr + 2]
        idx = int(self.datastack[self.stackPtr])
        self.datastack[self.stackPtr] = self.datastack[self.framePtr + 4 + int(p) + int(n)][idx]

        self.instructionPtr += 1

    def push_param_r(self, n):
        idx = int(self.datastack[self.stackPtr])
        self.datastack[self.stackPtr] = self.datastack[self.framePtr + 4 + int(n)].data[idx]

        self.instructionPtr += 1

    def push_param_idx(self, n):
        idx = int(self.datastack[self.stackPtr])
        self.datastack[self.stackPtr] = self.datastack[self.framePtr + 4 + int(n)][idx]

        self.instructionPtr += 1

    def push_param(self, n):
        self.stackPtr += 1
        p = self.datastack[self.framePtr]
        self.datastack[self.stackPtr] = self.datastack[self.framePtr + 4 + int(n)]

        self.instructionPtr += 1

    def set_param(self, n):
        l = self.datastack[self.framePtr + 1]
        p = self.datastack[self.framePtr + 2]
        self.datastack[self.framePtr + 2 + 1 + 1 + int(n)] = self.datastack[self.framePtr - int(p) + int(n)]

        self.instructionPtr += 1

    def set_param_record_idx(self, n):
        idx = self.datastack[self.stackPtr]
        val = self.datastack[self.stackPtr - 1]
        self.datastack[self.framePtr + 2 + 1 + 1 + int(n)].data[int(idx)] = val
        self.stackPtr -= 1
        self.instructionPtr += 1

    def set_param_idx(self, n):
        val = self.datastack[self.stackPtr]
        idx = self.datastack[self.stackPtr - 1]
        self.datastack[self.framePtr + 2 + 1 + 1 + int(n)][int(idx)] = val
        self.stackPtr -= 1
        self.instructionPtr += 1

    def frame(self, l, p):
        self.push(self.framePtr)
        self.framePtr = self.stackPtr + 1
        self.stackPtr += 2 + l + p
        self.datastack[self.framePtr] = l
        self.datastack[self.framePtr + 1] = p

    def jump(self, label):
        self.codePtr = label
        self.instructionPtr += 1

    def jeq(self, label):
        v = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        if v:
            self.jump(label)

        self.instructionPtr += 1

    def jneq(self, label):
        v = self.datastack[self.stackPtr]
        self.stackPtr -= 1

        self.instructionPtr += 1
        if not v:
            self.jump(label)
            self.instructionPtr = int(label)

    def stdout(self):
        v = self.datastack[self.stackPtr]
        self.debug_str += f"\n{v}"
        self.instructionPtr += 1

    def stdin(self):
        readin = input('> ')
        self.stackPtr += 1
        self.datastack[self.stackPtr] = readin

        self.instructionPtr += 1

    def ret(self):
        fp = self.framePtr
        res = self.datastack[self.stackPtr]
        self.stackPtr = self.framePtr - int(self.datastack[fp + 2]) - 1
        self.framePtr = self.datastack[fp]
        if self.debug:
            print(f"RETURN: on return stack and frame ptrs {self.stackPtr}, {self.framePtr}")

        self.instructionPtr = int(self.datastack[fp + 1])
        if self.debug:
            print(f"RETURN: instruction pointer {self.instructionPtr}, {len(self.code)}")
        self.stackPtr += 1
        self.datastack[self.stackPtr] = res
        for i in range(self.stackPtr + 1, len(self.datastack)):
            self.datastack[i] = None

    def _take2(self):
        a = self.datastack[self.stackPtr]
        b = self.datastack[self.stackPtr - 1]
        self.stackPtr -= 2
        return a, b

    def goto(self, n):
        self.instructionPtr = int(n)

    # ====================================
    # setup stack frame
    # ...+----------+----------------+------------+------------+ ...
    # ...| ret_inst | ret_stack_addr | no. params | no. locals | ...
    # ...+----------+----------------+------------+------------+ ...
    # =====================================
    def size(self, p, l):
        if self.debug:
            print(f"Setting up stack frame for function now. param count and local var count {p} , {l}")
        fp = self.framePtr
        self.stackPtr += 1
        self.framePtr = self.stackPtr
        self.datastack[self.stackPtr] = fp
        self.stackPtr += 1
        self.datastack[self.stackPtr] = self.INSTR_PTR + 1
        self.stackPtr += 1
        self.datastack[self.stackPtr] = p

        self.stackPtr += 1
        self.datastack[self.stackPtr] = l
        self.stackPtr = self.framePtr + 2 + int(p) + int(l) + 1
        self.instructionPtr += 1

    def cast(self, t: str):
        r = self.datastack[self.stackPtr]
        if t == "i8":
            cr = int(r)
        elif t == "i32":
            cr = int(r)
        elif t == "i64":
            cr = int(r)
        elif t == "f32":
            cr = float(r)
        elif t == "f64":
            cr = float(r)
        elif t == "str":
            cr = str(r)
        else:
            raise ValueError(f"Cannot perform type cast for type {t}")

        self.datastack[self.stackPtr] = cr
        self.instructionPtr += 1

    def add(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = t1 + t0
        self.instructionPtr += 1

    def sub(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = t1 - t0
        self.instructionPtr += 1

    def mul(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = t0 * t1
        self.instructionPtr += 1

    def div(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = t1 / t0
        self.instructionPtr += 1

    def rem(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = t1 % t0
        self.instructionPtr += 1

    def eq(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t0 == t1)
        self.instructionPtr += 1

    def neq(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t0 != t1)
        self.instructionPtr += 1

    def gt(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 > t0)
        self.instructionPtr += 1

    def gte(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 >= t0)
        self.instructionPtr += 1

    def lt(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 < t0)
        self.instructionPtr += 1

    def lte(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 <= t0)
        self.instructionPtr += 1

    def andx(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 > 0 and t0 > 0)
        self.instructionPtr += 1

    def orx(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 > 0 or t0 > 0)
        self.instructionPtr += 1

    def bit_and(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 & t0)
        self.instructionPtr += 1

    def bit_or(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 | t0)
        self.instructionPtr += 1

    def xor(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 ^ t0)
        self.instructionPtr += 1

    def l_shift(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 << t0)
        self.instructionPtr += 1

    def r_shift(self):
        t0, t1 = self._take2()
        t0 = self.infer_type(t0)
        t1 = self.infer_type(t1)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(t1 >> t0)
        self.instructionPtr += 1

    def bit_not(self):
        a = self.datastack[self.stackPtr]
        self.stackPtr += 1
        self.datastack[self.stackPtr] = int(~a)
        self.instructionPtr += 1

    def notx(self):
        a = self.datastack[self.stackPtr]
        self.stackPtr += 1
        self.datastack[self.stackPtr] = 1 if int(a) == 0 else 0
        self.instructionPtr += 1

    def length(self):
        # do not increment the stack pointer here.
        v = self.datastack[self.stackPtr]
        l = len(v)

        self.datastack[self.stackPtr] = l

        self.instructionPtr += 1

    def fopen(self):
        mode = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        file_name = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd = open(file=file_name, mode=mode)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = fd
        self.instructionPtr += 1

    def fclose(self):
        fd = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd.close()
        self.instructionPtr += 1

    def fread(self):
        bytes = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        data = fd.read(bytes)
        self.stackPtr += 1
        self.datastack[self.stackPtr] = data
        self.instructionPtr += 1

    def fwrite(self):
        data = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd.write(data)
        self.instructionPtr += 1

    def fseek(self):
        whence = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        offset = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd.seek(offset,whence)
        # python seek returns None, so we simulate it returning a value, (0) in this case
        self.stackPtr += 1
        self.datastack[self.stackPtr] = 1
        self.instructionPtr += 1

    def ftell(self):
        whence = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        offset = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd = self.datastack[self.stackPtr]
        self.stackPtr -= 1
        fd.seek(offset,whence)
        self.instructionPtr += 1


@click.command()
@click.option('--program', "-p", prompt='Program file path',
              help='The path to the program file.')
@click.option("--debug", "-d", type=bool, default=False,
              help='Show Python stacktrace on error.')
def run_vm(program, debug):
    with open(program) as p:
        prog = p.read()
        TachyonVM(prog, debug).run()


if __name__ == "__main__":
    run_vm()
