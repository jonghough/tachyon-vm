class FileOps:
    ops_list = ["fopen", "fclose", "fread", "fwrite", "fdelete", "fseek", "ftell"]

    def __init__(self, virtual_machine):
        self.vm = virtual_machine

    def fopen(self):
        mode = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        file_name = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd = open(file=file_name, mode=mode)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = fd
        self.vm.instructionPtr += 1

    def fclose(self):
        fd = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd.close()
        self.vm.instructionPtr += 1

    def fread(self):
        bytes = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        data = fd.read(bytes)
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = data
        self.vm.instructionPtr += 1

    def fwrite(self):
        data = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd.write(data)
        self.vm.instructionPtr += 1

    def fseek(self):
        whence = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        offset = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd.seek(offset, whence)
        # python seek returns None, so we simulate it returning a value, (0) in this case
        self.vm.stackPtr += 1
        self.vm.datastack[self.vm.stackPtr] = 1
        self.vm.instructionPtr += 1

    def ftell(self):
        whence = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        offset = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd = self.vm.datastack[self.vm.stackPtr]
        self.vm.stackPtr -= 1
        fd.seek(offset, whence)
        self.vm.instructionPtr += 1
