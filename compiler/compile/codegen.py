from compile.compiler import Context, FunContext
from compile.record import Record
from compile.opcodes import OpCode
from compile.type_check import push_for_type


class CodeGenerator:

    def __init__(self, context: Context):
        self.base_context = context

    def generate(self) -> list[OpCode]:
        self.base_context.collect_labels(0, {})
        # res = [f"entry: {self.base_context.entry_point} "]
        res = [OpCode(None, f"entry: {self.base_context.entry_point} ", None)]
        for cxt in self.base_context.contexts:
            if isinstance(cxt, FunContext):
                res += self._generate(cxt, self.base_context.label_dict, self.base_context.function_labels)
            else:
                raise Exception("Undefined Context. Error in function definitions.")
        if not self.base_context.code:
            raise Exception("Uncalled entry point. There appears to be no main function called.")
        last_code = None
        for code in self.base_context.code:
            if code.op_type == "label":
                res.append(OpCode(code.index, "noop", None))
            elif code.op_type == "goto":
                goto_op_idx = self.base_context.label_dict[code.operation]
                res.append(OpCode(code.index, "goto", goto_op_idx))
            elif code.op_type == "jneq":
                goto_op_idx = self.base_context.label_dict[code.operation]
                res.append(OpCode(code.index, "jneq", goto_op_idx))
            elif code.op_type == "call":
                call_op_idx = self.base_context.label_dict[self.base_context.function_labels[code.operation]]
                res.append(OpCode(code.index, "call", call_op_idx))

            else:
                res.append(OpCode(code.index, code.operation, None))
            last_code = code
        res[0].operation += f"size: {last_code.index + 1}"
        return res

    def _generate(self, cxt: Context, label_dict: dict, function_dict: dict) -> list[OpCode]:
        res: list[OpCode] = []

        for child_cxt in cxt.contexts:
            res += self._generate(child_cxt, cxt.label_dict, cxt.function_labels)
        res.append(OpCode(cxt.call_index, "noop", None))
        res.append(OpCode(cxt.header.index, cxt.header.operation, cxt.size_args))

        for idx, p in enumerate(cxt.get_params):
            # get param from stack and put in param section
            res.append(OpCode(p.index, "set_param", p.operation))
        for code in cxt.code:
            if code.op_type == "label":
                res.append(OpCode(code.index, "noop", None))
            elif code.op_type == "goto":
                goto_op_idx = label_dict[code.operation]
                res.append(OpCode(code.index, "goto", goto_op_idx))
            elif code.op_type == "jneq":
                goto_op_idx = label_dict[code.operation]
                res.append(OpCode(code.index, "jneq", goto_op_idx))
            elif code.op_type == "call":
                found = False
                if code.operation in function_dict.keys():
                    call_op_idx = label_dict[function_dict[code.operation]]
                    res.append(OpCode(code.index, "call", call_op_idx))
                    found = True
                elif code.operation in cxt.function_labels.keys():
                    call_op_idx = cxt.label_dict[cxt.function_labels[code.operation]]
                    res.append(OpCode(code.index, "call", call_op_idx))
                    found = True
                if not found:
                    raise ValueError(f"Could not find label {code.operation} in {cxt.name}")
            elif code.op_type == "push_stack_r":
                res.append(OpCode(code.index, "push_stack_r", None))
                print("push stack r")
            elif code.op_type == "push_stack_a":
                res.append(OpCode(code.index, "push_stack_a", None))
                print("push stack r")
            elif code.op_type == "set_local_r":
                op = code.operation
                for idx, lv in enumerate(cxt.local_vars):
                    if lv == op:
                        res.append(OpCode(code.index, "set_local_r", idx))
                        break
            elif code.op_type == "set_local":
                op = code.operation
                for idx, lv in enumerate(cxt.local_vars):
                    if lv == op:
                        res.append(OpCode(code.index, "set_local", idx))
                        break
            elif code.op_type == "set_record_idx":
                _, idx, lv = cxt.find_local_var_for_name(code.operation)
                res.append(OpCode(code.index, "set_record_idx", idx))
            elif code.op_type == "set_param":
                op = code.operation
                res.append(OpCode(code.index, "set_param", op))
            elif code.op_type == "set_param_record_idx":
                op = code.operation
                for idx, p in enumerate(cxt.params):
                    if p[0] == op:
                        res.append(OpCode(code.index, "set_param_record_idx", idx))
                        break
            elif code.op_type == "set_local_a":
                op = code.operation
                for idx, lv in enumerate(cxt.local_vars):
                    if lv == op:
                        res.append(OpCode(code.index, "set_local_a", idx))
                        break
            elif code.op_type == "set_param_idx":
                res.append(OpCode(code.index, "set_param_idx", code.operation))
            elif code.op_type == "set_local_idx":
                res.append(OpCode(code.index, "set_local_idx", code.operation))
            elif code.op_type == "push_local_r":
                op = code.operation
                for idx, lv in enumerate(cxt.local_vars):
                    if lv == op:
                        res.append(OpCode(code.index, "push_local_r", idx))
                        break
            elif code.op_type == "push_param_r":
                op = code.operation
                for idx, p in enumerate(cxt.params):
                    if p[0] == op:
                        res.append(OpCode(code.index, "push_param_r", idx))
                        break
            elif code.op_type == "push_array_index":
                op = code.operation
                found = False
                for idx, p in enumerate(cxt.params):
                    if p[0] == op:
                        res.append(OpCode(code.index, "push_param_idx", idx))
                        found = True
                        break
                if not found:
                    for idx, lv in enumerate(cxt.local_vars):
                        if lv == op:
                            res.append(OpCode(code.index, "push_local_idx", idx))
                            found = True
                            break
                if not found:
                    errmsg = f"cannot find {op}"
                    raise ValueError(errmsg)
            elif code.op_type == "identifier":
                o = code.operation
                found = False
                for idx, p in enumerate(cxt.params):
                    if p[0] == o:
                        res.append(OpCode(code.index, "push_param", idx))
                        found = True
                        break
                if not found:
                    for idx, lv in cxt.local_vars.items():
                        if lv.variable_name == o:
                            res.append(OpCode(code.index, "push_local", lv.index))
                            found = True
                            break
                if not found:
                    raise ValueError(f"The symbol {o} was not found")
            elif code.op_type == "cast":
                res.append(OpCode(code.index, "cast", code.operation))

            elif code.op_type == "push":
                push_type = push_for_type(code.datatype.dtype)
                ops = code.operation
                if push_type == "push_f32" or push_type == "Push_f64":
                    ops = '{:f}'.format(float(ops))

                res.append(OpCode(code.index, push_type, ops))
            else:
                splits = code.operation.split(' ')
                if len(splits) == 1:
                    res.append(OpCode(code.index, code.operation, None))
                elif len(splits) == 2:
                    res.append(OpCode(code.index, splits[0], splits[1]))
                else:
                    raise Exception(f"Unknown operation {code.operation}, {len(splits)}")
            cxt.index += 1
        return res
