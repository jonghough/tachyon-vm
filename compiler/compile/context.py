from __future__ import annotations

from typing import Dict

from compile.errors import MissingReturnStatement
from compile.local_vars import LocalVar, Datatype, DT
from compile.operation import Op


class Context:

    def __init__(self):
        self.code: list[Op] = []
        self.contexts: list[Context] = []
        self.records: list[Record] = []
        self.local_vars: Dict[str, LocalVar] = {}
        self.params = []
        self.name = ""
        self.local_var_counter = 0
        self.functions = []
        self.variables = []
        self.index = 0
        self.label_dict = {}
        self.function_labels = {}
        self.entry_point = 0
        self.parent = self
        # while loop stack.
        self.whiles = []

    def add_local_var(self, name: str, local_var: LocalVar):
        if not local_var.initial_value:
            raise Exception("Datatype must be given")
        if local_var.initial_value.dtype == DT.RECORD and local_var.initial_value.info is None:
            raise Exception("Record tpyes must be given an record type name")
        self.local_vars[name] = local_var

    def get_context(self, name: str) -> Context | None:
        for cxt in self.contexts:
            if name == cxt.name: return cxt
        if self.parent and self.parent != self:
            return self.parent.get_context(name)
        return None

    def get_record(self, name: str) -> Record | None:
        for rec in self.records:
            if name == rec.name: return rec
        if self.parent and self.parent != self:
            return self.parent.get_record(name)
        return None

    def get_function(self, name: str) -> tuple | None:
        for func in self.parent.functions:
            if func[0] == name:
                return func
        for func in self.functions:
            if func[0] == name:
                return func
        # is this a recursive call?
        if self.name == name and isinstance(self,FunContext):
            return (self.name, self.return_datatype)

        raise ValueError(f"Unreferenced function called {name}")

    def find_param_for_name(self, name: str):
        for idx, p in enumerate(self.params):
            if p[0] == name:
                return p, idx
        return None

    def find_local_var_for_name(self, name: str):
        for idx, lv in enumerate(self.local_vars):
            if lv == name:
                return lv, idx, self.local_vars[lv]
        return None,None,None

    def collect_labels(self, index: int, label_dict: dict[str, int]) -> int:
        self.index = index
        for cxt in self.contexts:
            cxt.collect_labels(self.index, self.label_dict)
            self.index = cxt.index

        self.entry_point = self.index
        for code in self.code:
            if code.op_type == "label":
                self.label_dict[code.operation] = self.index
            code.index = self.index
            self.index += 1
        # whatever is at top of stack is returned
        self.index += 1
        return self.index


class FunContext(Context):

    def __init__(self) -> None:
        super().__init__()
        self.label = ""
        self.name = ""
        self.local_ops = []
        self.params = []
        self.call_index = -1
        self.header = None
        self.get_params = []
        self.return_op = None
        self.return_datatype = None
        self.return_blocks = []


    def get_identifier(self, identifier: str):
        if identifier in self.local_vars.keys():
            return self.local_vars[identifier].data_type
        else:
            for p, t in self.params:
                if identifier == p:
                    return t
        raise ValueError(f"cannot find this identifier in the current context: {identifier} , context: {self.name}")

    def collect_labels(self, index: int, label_dict: dict[str, int]) -> int:
        if len(self.return_blocks) == 0:
            raise MissingReturnStatement(self.name)
        self.index = index
        self.label_dict = self.label_dict | label_dict
        for cxt in self.contexts:
            cxt.collect_labels(self.index, self.label_dict)
            self.index = cxt.index

        label_dict[self.label] = self.index
        self.call_index = self.index
        self.index += 1
        self.header = Op("header",
                         f"size", Datatype(DT.INT32, 0, None))
        self.size_args = f"{len(self.params)} {len(self.local_vars.keys())}"
        self.header.index = self.index
        self.index += 1
        for idx, p in enumerate(self.params):
            # get param from stack and put in param section
            op = Op("set_param", f"{idx}", p[1])
            op.index = self.index
            self.get_params.append(op)
            self.index += 1

        for code in self.code:
            if code.op_type == "label":
                label_dict[code.operation] = self.index
            code.index = self.index
            self.index += 1
        return self.index


