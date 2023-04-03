class BinaryOps:
    ops_list = ["add", "sub", "mul", "div", "rem", "eq", "neq", "gte", "gt", "lte", "lt", "bit_and", "bit_or", "xor",
                "bit_not", "l_shift", "r_shift"]
    special_ops_list = ["and", "or", "not"]

    def __init__(self, virtual_machine):
        self.vm = virtual_machine

    def add(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = t1 + t0
        self.vm.instructionPtr += 1

    def sub(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = t1 - t0
        self.vm.instructionPtr += 1

    def mul(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = t0 * t1
        self.vm.instructionPtr += 1

    def div(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = t1 / t0
        self.vm.instructionPtr += 1

    def rem(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = t1 % t0
        self.vm.instructionPtr += 1

    def eq(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t0 == t1)
        self.vm.instructionPtr += 1

    def neq(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t0 != t1)
        self.vm.instructionPtr += 1

    def gt(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 > t0)
        self.vm.instructionPtr += 1

    def gte(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 >= t0)
        self.vm.instructionPtr += 1

    def lt(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 < t0)
        self.vm.instructionPtr += 1

    def lte(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 <= t0)
        self.vm.instructionPtr += 1

    def andx(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 > 0 and t0 > 0)
        self.vm.instructionPtr += 1

    def orx(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 > 0 or t0 > 0)
        self.vm.instructionPtr += 1

    def bit_and(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 & t0)
        self.vm.instructionPtr += 1

    def bit_or(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 | t0)
        self.vm.instructionPtr += 1

    def xor(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 ^ t0)
        self.vm.instructionPtr += 1

    def l_shift(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 << t0)
        self.vm.instructionPtr += 1

    def r_shift(self):
        t0, t1 = self.vm._take2()
        t0 = self.vm.infer_type(t0)
        t1 = self.vm.infer_type(t1)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(t1 >> t0)
        self.vm.instructionPtr += 1

    def bit_not(self):
        a = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = int(~a)
        self.vm.instructionPtr += 1

    def notx(self):
        a = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = 1 if int(a) == 0 else 0
        self.vm.instructionPtr += 1
