import math


class MathOps:
    ops_list = ["sin", "cos", "tan", "asin", "acos", "atan",
                "sinh", "cosh", "tanh", "asinh", "acosh", "atanh",
                "exp", "log", "log10"]

    def __init__(self, virtual_machine):
        self.vm = virtual_machine

    def sin(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.sin(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def cos(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.cos(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def tan(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.tan(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def asin(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.asinh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def acos(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.acosh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def atan(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.atanh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def sinh(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.sinh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def cosh(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.cosh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def tanh(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.tanh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def asinh(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.asinh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def acosh(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.acosh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def atanh(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.atanh(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def exp(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.exp(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def log(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.log(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1

    def log10(self):
        v = self.vm.datastack[self.vm.stackPtr]
        l = math.log10(v)

        self.vm.datastack[self.vm.stackPtr] = l

        self.vm.instructionPtr += 1
