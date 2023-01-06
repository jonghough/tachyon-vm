opcode_list = [
    ("push_i8", 1),
    ("push_i32", 2),
    ("push_i64", 3),
    ("push_f32", 4),
    ("push_f64", 5),
    ("push_s", 6),
    ("add", 7),
    ("sub", 8),
    ("mul", 9),
    ("div", 10),
    ("rem", 11),
    ("eq", 12),
    ("neq", 13),
    ("lt", 14),
    ("lte", 15),
    ("gt", 16),
    ("gte", 17),
    ("and", 18),
    ("or", 19),
    ("bit_and", 20),
    ("bit_or", 21),
    ("xor", 22),
    ("l_shift", 23),
    ("r_shift", 24),
    ("bit_not", 25),
    ("not", 26),
    ("push_local", 27),
    ("set_local", 28),
    ("set_local_idx", 29),
    ("push_local_idx", 30),
    ("set_param", 31),
    ("set_param_idx", 32),
    ("push_param", 33),
    ("push_param_idx", 34),
    ("set_local_a", 35),
    ("set_record_idx", 36),
    ("set_param_record_idx", 37),
    ("set_local_r", 38),
    ("push_local_r", 39),
    ("push_param_r", 40),
    ("ret", 41),
    ("call", 42),
    ("size", 43),
    ("stdout", 44),
    ("stdin", 45),
    ("goto", 46),
    ("cast", 47),
    ("jneq", 48),
    ("entry", 49),
    ("noop", 50),
    ("length", 51),
    ("fopen", 52),
    ("fread", 53),
    ("fwrite", 54),
    ("fclose", 55),
    ("fdelete", 56),
    ("fseek", 57),
    ("push_stack_r", 58),
    ("push_stack_a", 59),
    ("notfound", 60)]


class OpCode:

    def __init__(self, line_number, operation, args):
        self.line_number = line_number
        self.operation = operation
        self.args = args

    def as_text(self) -> str:
        if self.line_number is not None:
            if self.args is not None:
                return f"{self.line_number}: {self.operation} {self.args}"
            else:
                return f"{self.line_number}: {self.operation}"
        else:

            return f"{self.operation}"

    def as_binary(self) -> bytes:
        if self.line_number is not None:
            if self.args is not None:
                intop = None
                for os in opcode_list:
                    if os[0] == self.operation:
                        intop = os[1]
                        break
                ln = int(self.line_number).to_bytes(4, byteorder='little')
                args_len = len(str.encode(str(self.args)))

                a = ln + int(intop).to_bytes(4, byteorder='little') + args_len.to_bytes(4,
                                                                                        byteorder='little') + str.encode(
                    str(self.args))
                return a
            else:
                intop = None
                for os in opcode_list:
                    if os[0] == self.operation:
                        intop = os[1]
                        break
                ln = int(self.line_number).to_bytes(4, byteorder='little')
                no_args = 0
                a = ln + int(intop).to_bytes(4, byteorder='little') + no_args.to_bytes(4, byteorder='little')
                return a
        else:
            header_len = len(str.encode(str(self.operation)))
            return header_len.to_bytes(4, byteorder='little') + str.encode(self.operation)
