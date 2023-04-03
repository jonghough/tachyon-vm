from __future__ import annotations

import re
import sys
from typing import Dict, Union
from lark import Lark, Token
import uuid

from lark.tree import Tree

from compile.context import Context, FunContext
from compile.local_vars import LocalVar, Datatype, DT
from compile.operation import Op
from compile.record import Record
from compile.symbols import AstSymbols
from compile.type_check import infer_type_arr, infer_type, infer_array_type, check_cast

# built-in functions
func_ops = {
    "read": ["stdin", DT.STR, None],
    "print": ["stdout", DT.INT32, ("Any",)],
    "tostring": ["tostring", DT.STR, ("Any",)],
    "toint": ["toint", DT.INT32, ("Any",)],
    "tofloat": ["tofloat", DT.FLOAT32, ("Any",)],
    "length": ["length", DT.INT32, ("Array",)],
    "fopen": ["fopen", DT.INT32, (DT.STR, DT.STR,)],
    "fread": ["fread", DT.STR, (DT.INT32, DT.INT32)],
    "fwrite": ["fwrite", DT.STR, (DT.INT32, DT.STR)],
    "fclose": ["fclose", DT.INT32, (DT.INT32,)],
    "fseek": ["fseek", DT.INT32, (DT.INT32, DT.INT64, DT.INT32)],
    "ftell": ["ftell", DT.INT32, (DT.INT32,)],
    "fdelete": ["fdelete", DT.INT32, (DT.INT32,)],
    "sin": ["sin", DT.FLOAT64, ("Any",)],
    "cos": ["cos", DT.FLOAT64, ("Any",)],
    "tan": ["tan", DT.FLOAT64, ("Any",)],
    "asin": ["asin", DT.FLOAT64, ("Any",)],
    "acos": ["acos", DT.FLOAT64, ("Any",)],
    "atan": ["atan", DT.FLOAT64, ("Any",)],
    "sinh": ["sinh", DT.FLOAT64, ("Any",)],
    "cosh": ["cosh", DT.FLOAT64, ("Any",)],
    "tanh": ["tanh", DT.FLOAT64, ("Any",)],
    "asinh": ["asinh", DT.FLOAT64, ("Any",)],
    "acosh": ["acosh", DT.FLOAT64, ("Any",)],
    "atanh": ["atanh", DT.FLOAT64, ("Any",)],

}


def allowed_types_for(dtype: DT) -> set:
    if dtype in [DT.INT8, DT.INT32, DT.INT64]:
        return {DT.INT8, DT.INT32, DT.INT64}
    elif dtype in [DT.FLOAT32, DT.FLOAT64]:
        return {DT.FLOAT32, DT.FLOAT64}
    else:
        return {dtype}


class Compiler:

    def __init__(self, grammar: str, trace: bool = False):
        super().__init__()
        self.program: str = None
        self.grammar = grammar
        self.cxt = Context()
        if not trace:
            sys.tracebacklimit = 0
        self.parser = Lark(self.grammar, parser="earley", propagate_positions=True)

    def compile(self, program: str) -> None:
        self.program = program
        self.compile_program(self.program, self.cxt)

    def handle_fname(self, name: Tree, cxt: Context) -> Datatype:
        function_name = None
        for child in name.children:
            if child in func_ops.keys():
                cxt.code.append(Op(func_ops[child][0], f"{func_ops[child][0]}", Datatype(func_ops[child][1], 0, None)))
                return Datatype(func_ops[child][1], 0, None)
            else:
                function_name = str(child)
                cxt.code.append(Op("call", f"{child}", cxt.get_function(function_name)[1]))

        return cxt.get_function(function_name)[1]

    def handle_function_call(self, fc: Tree, cxt: Context) -> Datatype:
        function_name = None
        fname = None
        for child in fc.children:
            if child.data == AstSymbols.fname:
                fname = child
                function_name = child.children[0]
            if child.data == AstSymbols.arguments:
                self.handle_arguments(child, function_name, 0, cxt)

        return self.handle_fname(fname, cxt)

    def handle_arguments(self, args: Tree, function_name: str, param_idx: int, cxt: Context) -> None:

        for child in args.children:
            if child.data == AstSymbols.expression:
                r = self.handle_expression(child, cxt)

                if function_name not in (
                        "print", "read", "length", "fopen", "fread", "fwrite", "fclose", "fseek", "ftell",
                        "sin", "cos", "tan",
                        "asin", "acos", "atan",
                        "sinh", "cosh", "tanh",
                        "asinh", "acosh", "atanh"):
                    func_type = cxt.get_context(function_name).params[param_idx][1]
                    if r.dtype != func_type.dtype:
                        if func_type.dtype in (DT.INT8, DT.INT32, DT.INT64) and \
                                r.dtype in (DT.INT8, DT.INT32, DT.INT64):
                            r.dtype = func_type.dtype
                        elif func_type.dtype in (DT.FLOAT32, DT.FLOAT64) and \
                                r.dtype in (DT.FLOAT32, DT.FLOAT64):
                            r.dtype = func_type.dtype
                        else:
                            raise ValueError(
                                f"Function Parameter type mismatch for {function_name}: {r.dtype} and {func_type.dtype}  at LINE: {args.meta.line}.")
                else:
                    for k, v in func_ops.items():
                        if function_name == k:
                            if v[2] is None:
                                raise ValueError(
                                    f"Function Parameter for {function_name}: param at index {param_idx} is not found in function signature, at LINE: {args.meta.line}.")
                            if len(v[2]) <= param_idx:
                                raise ValueError(
                                    f"Function Parameter for {function_name}: param at index {param_idx} is not found in function signature. Too many arguments, at LINE: {args.meta.line}.")
                            expected_param = v[2][param_idx]
                            if r.dtype == expected_param:
                                continue
                            if expected_param == "Any":
                                continue
                            elif expected_param == "Array":
                                if r.info is None:
                                    raise ValueError(
                                        f"Function Parameter for {function_name}: param at index {param_idx} is not found in function signature. Too many arguments, at LINE: {args.meta.line}.")
                                if r.info == "Array":
                                    continue
                                if "[]" in r.info:
                                    continue
                                else:
                                    raise ValueError(
                                        f"Function Parameter for {function_name}: param at index {param_idx} is not found in function signature. Too many arguments, at LINE: {args.meta.line}.")
                            elif expected_param == DT.STR:
                                if r.dtype != DT.STR:
                                    raise ValueError(
                                        f"Function Parameter for {function_name}: param at index {param_idx} is not found in function signature. Too many arguments, at LINE: {args.meta.line}.")
                            elif expected_param in (DT.INT8, DT.INT32, DT.INT64) and \
                                    r.dtype in (DT.INT8, DT.INT32, DT.INT64):
                                r.dtype = expected_param
                            elif expected_param in (DT.FLOAT32, DT.FLOAT64) and \
                                    r.dtype in (DT.FLOAT32, DT.FLOAT64):
                                r.dtype = expected_param
                            else:
                                raise ValueError(
                                    f"Function Parameter type mismatch for {function_name}: {r.dtype} and {expected_param}  at LINE: {args.meta.line}.")

                param_idx += 1
            elif child.data == AstSymbols.arguments:
                self.handle_arguments(child, function_name, param_idx, cxt)

    def handle_binary_exp(self, bexp: Tree, cxt: Context) -> Datatype:
        ops = []
        operator = None
        for child in bexp.children:
            if child.data == AstSymbols.expression:
                r = self.handle_expression(child, cxt)
                ops.append(r)
            elif child.data == AstSymbols.operator:
                operator = child.children[0].type
        # TODO check if the operation is valid with the operands
        cxt.code.append(Op(operator.lower(), operator.lower(), r))
        actual_type = infer_type_arr(bexp, ops, operator)
        return Datatype(actual_type, None, None)

    def handle_identifier(self, id: Tree, cxt: Context) -> None:
        for child in id.children:
            if isinstance(child, Token):
                pass
            else:
                cxt.code.append(Op("unknown", f"unknown {child.data}"))

    def handle_cast_expression(self, exp: Tree, cxt: Context) -> Datatype:
        datatype = None
        precast = None
        for child in exp.children:
            if isinstance(child, Token):
                datatype = Datatype(DT.from_string(str(child)), None, str(child))
            elif child.data == AstSymbols.expression:
                precast = self.handle_expression(child, cxt)
        if not datatype:
            raise ValueError(f"No cast datatype found in cast expression at LINE: {exp.meta.line}.")
        if not precast:
            raise ValueError("No type found in cast expression at LINE: {exp.meta.line}.")
        can_cast = check_cast(precast, datatype)
        if not can_cast:
            raise ValueError(f"Cast from {precast} to {datatype} is illegal, at LINE: {exp.meta.line}.")
        cxt.code.append(Op("cast", str(DT.to_string(datatype.dtype)), datatype))
        return datatype

    def handle_expression(self, expression: Tree, cxt: FunContext, expected_dtype: Datatype = None) -> Datatype:
        for child in expression.children:
            if isinstance(child, Token):  # Token:
                alpha = re.match("\'", str(child))
                t = "str"
                if alpha:
                    val = str(child)
                elif "." in str(child):
                    val = float(str(child))
                    t = "f32"
                else:
                    val = int(str(child))
                    t = "i32"
                if expected_dtype:
                    cxt.code.append(Op("push", f"{val}", expected_dtype))
                else:
                    cxt.code.append(Op("push", f"{val}", Datatype(DT.from_string(t), val, t)))

                return infer_type(child)
            elif child.data == AstSymbols.binary_exp:
                res = self.handle_binary_exp(child, cxt)
                if not res:
                    raise ValueError(f"Expression must return a value, at LINE: {expression.meta.line}.")
                return res
            elif child.data == AstSymbols.expression:
                res = self.handle_expression(child, cxt)
                if not res:
                    raise ValueError(f"Expression must return a value, at LINE: {expression.meta.line}.")
                return res
            elif child.data == AstSymbols.identifier:
                op = Op("identifier", child.children[0], None)
                cxt.code.append(op)
                self.handle_identifier(child, cxt)
                res = cxt.get_identifier(child.children[0])
                if not res:
                    raise ValueError(f"Expression must return a value, at LINE: {expression.meta.line}.")
                op.datatype = res
                return res
            elif child.data == AstSymbols.bit_not_expression:
                res = self.handle_expression(child.children[1], cxt)
                if res.dtype not in (DT.INT8, DT.INT32, DT.INT64):
                    raise ValueError(
                        f"Bitwise not may only be used with Integer datatypes, at LINE: {child.children[1].meta.line}.")
                cxt.code.append(Op("bit_not", "bit_not", res))
                return res
            elif child.data == AstSymbols.not_expression:
                res = self.handle_expression(child.children[1], cxt)
                if res.dtype not in (DT.INT8, DT.INT32, DT.INT64):
                    raise ValueError(
                        f"Not may only be used with Integer datatypes, at LINE: {child.children[1].meta.line}.")
                cxt.code.append(Op("not", "not", res))
                return res
            elif child.data == AstSymbols.cast_expression:
                return self.handle_cast_expression(child, cxt)
            elif child.data == AstSymbols.record_initialization:
                return self.handle_record_initialization(child, cxt)
            elif child.data == AstSymbols.assign_variable:
                return self.handle_assign_variable(child, cxt)
            elif child.data == AstSymbols.push_record_field:
                res = self.handle_push_record_field(child, cxt)
                return res  # Datatype(res,0,None)
            elif child.data == AstSymbols.push_expression_field:
                res = self.handle_push_expression_field(child, cxt)
                return res  # Datatype(res,0,None)
            elif child.data == AstSymbols.push_expression_index:
                res = self.handle_push_expression_index(child, cxt)
                return res  # Datatype(res,0,None)
            elif child.data == AstSymbols.function_call:
                res = self.handle_function_call(child, cxt)
                if not res:
                    raise ValueError(f"Expression must return a value, at LINE: {expression.meta.line}.")
                return res
            elif child.data == AstSymbols.array_index:
                # here the array index is on right side, so is being assigned to
                # some other variable
                r = None
                idx_val = None
                for sub in child.children:
                    if isinstance(sub, Token):
                        idx_val = sub
                        op = Op("push_array_index", f"{idx_val}", None)
                        assignedvar = None

                        str_idx_val = str(idx_val)
                        if str_idx_val in cxt.local_vars.keys():
                            assignedvar = cxt.local_vars[str(idx_val)]
                            r = assignedvar.data_type
                            r.val = assignedvar
                            if r.dtype == DT.RECORD:
                                r.info = assignedvar.data_type.info
                        if not assignedvar:
                            assignedvar = cxt.find_param_for_name(str(idx_val))
                            r = assignedvar[0][1]
                        if not assignedvar:
                            raise ValueError(
                                f"Could not find the symbol {str_idx_val} in current context, at LINE: {expression.meta.line}.")

                    elif sub.data == AstSymbols.expression:
                        expr_r = self.handle_expression(sub, cxt)
                        # if datatype r has been found above, then do not use this expression
                        # returned value.
                        # in fact, this should never be needed as the expression here
                        # is actually an expression to get the index of the array to be used.
                        # which is always an int.
                        if not r:
                            r = expr_r

                # cxt.code.append(Op("push_i", idx_val, Datatype(DT.INT,0)))
                cxt.code.append(op)
                if not r:
                    raise ValueError(f"Expression must return a value at LINE: {expression.meta.line}.")
                else:
                    op.datatype = r
                return r

    def handle_assign_variable(self, av, cxt: Context) -> Datatype:
        varname = None
        exp_type = None
        for child in av.children:
            if isinstance(child, Token):
                varname = child

            elif child.data == AstSymbols.expression:
                exp_type = self.handle_expression(child, cxt)

        else:
            op = Op("set_local", f"{varname}", None)
            cxt.code.append(op)
        op.datatype = exp_type
        if str(varname) not in cxt.local_vars.keys():
            raise ValueError(f"Assignment of variable before declaration:  {varname} , at LINE: {av.meta.line}.")
        assigned_type = cxt.local_vars[str(varname)].data_type
        if assigned_type.dtype != exp_type.dtype:
            if assigned_type.dtype in (DT.INT8, DT.INT32, DT.INT64) and \
                    exp_type.dtype in (DT.INT8, DT.INT32, DT.INT64):
                exp_type.dtype = assigned_type.dtype
            elif assigned_type.dtype in (DT.FLOAT32, DT.FLOAT64) and \
                    exp_type.dtype in (DT.FLOAT32, DT.FLOAT64):
                exp_type.dtype = assigned_type.dtype
            else:
                raise ValueError(
                    f"Assignment of variable of wrong type:  {varname}, given: {assigned_type.dtype}, expected: {exp_type.dtype} , at LINE: {av.meta.line}.")

        cxt.local_vars[str(varname)].data_type = exp_type
        #    cxt.local_var_counter += 1
        return exp_type

    def handle_variable_declaration(self, vd: Tree, cxt: Context) -> None:
        for child in vd.children:
            if child.data == AstSymbols.scalar_var_declaration:
                self.handle_scalar_var_declaration(child, cxt)
            elif child.data == AstSymbols.array_var_declaration:
                self.handle_array_var_declaration(child, cxt)

    def handle_scalar_var_declaration(self, svd: Tree, cxt: Context):
        varname = None
        localvar = None
        assigned_type = None
        expression_data = None
        for child in svd.children:
            if isinstance(child, Token):
                varname = child
                localvar = LocalVar(cxt.local_var_counter, varname, None)
            elif child.data == AstSymbols.expression:
                expression_data = child

            elif child.data == AstSymbols.type_decl:
                assigned_type = self.handle_type_decl(child, cxt)

        if expression_data:
            r: Datatype = self.handle_expression(expression_data, cxt, assigned_type)
            if not isinstance(child.children[0], Token) and \
                    child.children[0].data != AstSymbols.record_initialization:
                r.initialized = True
            else:
                r.initialized = False
            if localvar:
                localvar.data_type = r
        if varname in [p[0] for p in cxt.params]:
            raise ValueError(
                f"Local variable {varname} is used as a parameter variable, at LINE: {svd.meta.line}.")

        if r.dtype == DT.RECORD:
            # if we are initializing the record in this statement then
            # we can use `set_local_r` but if we are aliasing an
            # existing record we should do `set_local`.
            if not r.initialized:
                cxt.code.append(Op("set_local_r", f"{varname}", r))
            else:
                cxt.code.append(Op("set_local", f"{varname}", r))
            r.initialized = True
            r.info = assigned_type.info
            localvar.content = r.val
            localvar.data_type = r
        else:
            cxt.code.append(Op("set_local", f"{varname}", r))
        if localvar.data_type.dtype != assigned_type.dtype:
            if assigned_type.dtype in (DT.INT8, DT.INT32, DT.INT64) and \
                    localvar.data_type.dtype in (DT.INT8, DT.INT32, DT.INT64):
                localvar.data_type.dtype = assigned_type.dtype
            elif assigned_type.dtype in (DT.FLOAT32, DT.FLOAT64) and \
                    localvar.data_type.dtype in (DT.FLOAT32, DT.FLOAT64):
                localvar.data_type.dtype = assigned_type.dtype
            else:
                raise ValueError(
                    f"Type mismatch for {varname}: {localvar.data_type.dtype} and {assigned_type.dtype} at LINE: {svd.meta.line}.")

        if str(varname) not in cxt.local_vars.keys():
            cxt.local_vars[str(varname)] = localvar
            cxt.local_var_counter += 1

    def handle_array_var_declaration(self, avd: Tree, cxt: Context):
        varname = None
        localvar = None
        array_size_child = None
        assigned_type = None
        is_decl_expression = False
        for child in avd.children:
            if isinstance(child, Token):
                if not varname:
                    varname = child
                    localvar = LocalVar(cxt.local_var_counter, varname, None)
                else:
                    assigned_type = infer_array_type(child)

            elif child.data == AstSymbols.array_size_declaration:
                array_size_child = child
            elif child.data == AstSymbols.array_decl_expression:
                is_decl_expression = True
                r = self.handle_expression(child, cxt)
                if localvar:
                    localvar.data_type = r

            elif child.data == AstSymbols.type_decl:
                assigned_type = self.handle_type_decl(child, cxt).dtype
            elif child.data == AstSymbols.array_decl:
                is_decl_expression = False
                r = self.handle_array_decl(child, cxt)
                localvar.data_type = r

        # If we are declaring an array with an array declaration then
        # we can use `set_local_a`, but if using some expression, we use
        # `set_local`.
        if not is_decl_expression:
            if not array_size_child:
                raise ValueError(
                    f"The explicit declaration of an array requires the size to be defined, at LINE: {avd.meta.line}.")
            self.handle_array_size_declaration(array_size_child, cxt)
            cxt.code.append(Op("set_local_a", f"{varname}", Datatype(assigned_type, 0, None)))
        else:
            cxt.code.append(Op("set_local", f"{varname}", Datatype(DT.INT32, 0, None)))

        if varname in [p[0] for p in cxt.params]:
            raise ValueError(
                f"Local variable {varname} is used as a parameter variable, at LINE: {avd.meta.line}.")

        if localvar.data_type.dtype != assigned_type:
            if assigned_type in (DT.INT8, DT.INT32, DT.INT64) and \
                    localvar.data_type.dtype in (DT.INT8, DT.INT32, DT.INT64):
                localvar.data_type.dtype = assigned_type
            elif assigned_type in (DT.FLOAT32, DT.FLOAT64) and \
                    localvar.data_type.dtype in (DT.FLOAT32, DT.FLOAT64):
                localvar.data_type.dtype = assigned_type
            else:
                raise ValueError(
                    f"Type mismatch {localvar.data_type.dtype} and {assigned_type}, at LINE: {avd.meta.line}.")
        localvar.data_type.info = "Array"
        if str(varname) not in cxt.local_vars.keys():
            cxt.local_vars[str(varname)] = localvar
            cxt.local_var_counter += 1

    def handle_array_size_declaration(self, asz: Tree, cxt: Context):
        for child in asz.children:
            # chould be a token
            if isinstance(child, Token):
                cxt.code.append(Op("push_i32", f"push_i32 {int(child)}", Datatype(DT.INT32, 0, None)))
            else:
                if child.data == AstSymbols.expression:
                    m = self.handle_expression(child, cxt)
                    if m.dtype not in (DT.INT32, DT.INT64, DT.INT32):
                        raise ValueError(
                            f"Error expression must return int type, {m.dtype} found at LINE: {child.meta.line}.")
                    if m.dtype != DT.INT32:
                        # TODO
                        # WARNING: array size should be int32
                        m.dtype = DT.INT32

    def handle_return_type_decl(self, rtd: Tree, cxt: Context) -> Datatype:
        for child in rtd.children:
            return Datatype(DT.from_string(str(child)), 0, str(child))

    def handle_function_declaration(self, fd: Tree, cxt: Context) -> Datatype:
        # switch context
        fcxt = FunContext()
        fcxt.parent = cxt

        cxt.contexts.append(fcxt)
        retval = None
        for child in fd.children:
            if isinstance(child, Token):
                fcxt.label = str(uuid.uuid4())

            else:
                if child.data == AstSymbols.varlist:
                    self.handle_varlist(child, fcxt)
                elif child.data == AstSymbols.return_type_decl:
                    fcxt.return_datatype = self.handle_return_type_decl(child, cxt)
                elif child.data == AstSymbols.fbody:
                    retval = self.handle_fbody(child, fcxt)
                elif child.data == AstSymbols.fname:
                    fcxt.name = child.children[0]

        if retval is None:
            raise ValueError(f"{fcxt.name} return value is none, at LINE: {fd.meta.line}.")
        cxt.functions.append((fcxt.name, fcxt.return_datatype))
        cxt.function_labels[fcxt.name] = fcxt.label
        return None

    def handle_fbody(self, fbody: Tree, cxt: FunContext) -> Datatype:
        retval = None
        for child in fbody.children:
            if child.data == AstSymbols.f_statement:
                retval = self.handle_f_statement(child, cxt)
        if retval is None:
            raise ValueError(f"Error missing return statement for function at LINE: {fbody.meta.line}.")
        return retval

    def handle_f_statement(self, fbody: Tree, cxt: FunContext) -> Datatype:
        retval = Datatype(DT.INT32, 0, None)
        for child in fbody.children:
            if child.data == AstSymbols.statement:
                r = self.handle_statement(child, cxt)

            elif child.data == AstSymbols.return_block:
                retval = self.handle_return_block(child, cxt)
        return retval

    def handle_return_block(self, rb: Tree, cxt: FunContext) -> Datatype:
        for child in rb.children:
            if isinstance(child, Token):
                pass
            elif child.data == AstSymbols.expression:
                r = self.handle_statement(child, cxt)
                if cxt.return_datatype.dtype not in allowed_types_for(r.dtype):
                    raise ValueError(
                        f"Error return type {r.dtype} does not match declared return type {cxt.return_datatype.dtype}, at LINE: {rb.meta.line}.")
                cxt.code.append(Op("ret", f"ret", cxt.return_datatype))
                cxt.return_blocks.append(Op("ret", f"ret", cxt.return_datatype))
                return r

    def handle_var_param(self, vp: Tree, cxt: FunContext):
        vname = vp.children[0]
        vtype = vp.children[1]
        cxt.params.append((str(vname), Datatype(DT.from_string(vtype), val=None, info=vtype)))

    def handle_varlist(self, vlist: Tree, cxt: FunContext) -> None:
        for child in vlist.children:

            if child.data == AstSymbols.var_param:
                self.handle_var_param(child, cxt)
            else:
                if child.data == AstSymbols.varlist:
                    self.handle_varlist(child, cxt)

    def handle_if_body(self, ib: Tree, cxt: Context):
        for child in ib.children:
            if child.data == AstSymbols.statement:
                self.handle_statement(child, cxt)

    def handle_if_block(self, ib: Tree, cxt: Context, final_label: str):
        has_else = False
        has_elif = False
        for child in ib.children:
            if child.data == AstSymbols.if_clause:
                next_label = str(uuid.uuid4())
                self.handle_if_clause(child, cxt, next_label, final_label)
                cxt.code.append(Op("label", f"{next_label}", None))

            elif child.data == AstSymbols.elif_clause:
                next_label = str(uuid.uuid4())
                self.handle_elif_clause(child, cxt, next_label, final_label)
                cxt.code.append(Op("label", f"{next_label}", None))

            elif child.data == AstSymbols.else_clause:
                self.handle_else_clause(child, cxt, final_label)

    def handle_elif_clause(self, ef: Tree, cxt: Context, next_clause_label: str, final_label: str):

        for child in ef.children:
            if isinstance(child, Token):
                pass
            else:
                if child.data == AstSymbols.expression:
                    self.handle_expression(child, cxt)
                    cxt.code.append(Op("jneq", f"{next_clause_label}", None))

                elif child.data == AstSymbols.if_body:
                    self.handle_if_body(child, cxt)
                    cxt.code.append(Op("goto", f"{final_label}", None))

    def handle_else_clause(self, ef: Tree, cxt: Context, final_label):
        for child in ef.children:
            if isinstance(child, Token):
                pass
            else:
                if child.data == AstSymbols.if_body:
                    self.handle_if_body(child, cxt)

    def handle_if_clause(self, ic: Tree, cxt: Context, next_clause_label: str, final_label: str):

        for child in ic.children:
            if isinstance(child, Token):
                pass
            else:
                if child.data == AstSymbols.expression:
                    self.handle_expression(child, cxt)
                    cxt.code.append(Op("jneq", f"{next_clause_label}", None))

                elif child.data == AstSymbols.if_body:
                    self.handle_if_body(child, cxt)
                    cxt.code.append(Op("goto", f"{final_label}", None))

    def handle_statement(self, statement: Union[str, Tree], cxt: Context) -> Datatype:
        retval = None
        if statement.data == AstSymbols.statement:
            for substatement in statement.children:
                r = self.handle_statement(substatement, cxt)
                if r:
                    retval = r
        elif statement.data == AstSymbols.variable_decl:
            r = self.handle_variable_declaration(statement, cxt)
            if r:
                retval = r
        elif statement.data == AstSymbols.function_decl:
            r = self.handle_function_declaration(statement, cxt)

        elif statement.data == AstSymbols.if_block:
            finlab = str(uuid.uuid4())
            r = self.handle_if_block(statement, cxt, finlab)
            if r:
                retval = r
            cxt.code.append(Op("label", f"{finlab}", None))
        elif statement.data == AstSymbols.while_block:
            startlab = str(uuid.uuid4())
            finlab = str(uuid.uuid4())
            cxt.code.append(Op("label", f"{startlab}", None))
            cxt.whiles.append((startlab, finlab))
            r = self.handle_while_block(statement, cxt, startlab, finlab)
            cxt.whiles.pop()
            if r:
                retval = r
            cxt.code.append(Op("label", f"{finlab}", None))
        elif statement.data == AstSymbols.for_block:
            startlab = str(uuid.uuid4())
            finlab = str(uuid.uuid4())
            # init expression must execute before loop start label
            for grandchild in statement.children:
                if not isinstance(grandchild, Token) and grandchild.data == AstSymbols.init_expression:
                    self.handle_init_expression(grandchild, cxt)
            cxt.code.append(Op("label", f"{startlab}", None))
            cxt.whiles.append((startlab, finlab))

            r = self.handle_for_block(statement, cxt, startlab, finlab)
            cxt.whiles.pop()
            if r:
                retval = r
            cxt.code.append(Op("label", f"{finlab}", None))
        elif statement.data == AstSymbols.expression:
            r = self.handle_expression(statement, cxt)
            if r:
                retval = r
        elif statement.data == AstSymbols.assign_array_index:
            self.handle_assign_array_index(statement, cxt)
        elif statement.data == AstSymbols.assign_record_field:
            self.handle_assign_record_field(statement, cxt)
        elif statement.data == AstSymbols.break_statement:
            self.handle_break_statement(statement, cxt)
        elif statement.data == AstSymbols.continue_statement:
            self.handle_continue_statement(statement, cxt)
        elif statement.data == AstSymbols.record_type_decl:
            self.handle_record_type_decl(statement, cxt)
        elif statement.data == AstSymbols.return_block:
            self.handle_return_block(statement, cxt)
        else:
            raise ValueError(f"Error handled statement type {statement.data} at LINE: {statement.meta.line}.")
        return retval

    def handle_while_body(self, wb, cxt) -> Datatype:
        retval = None
        for child in wb.children:
            if child.data == AstSymbols.statement:
                r = self.handle_statement(child, cxt)
                if r:
                    retval = r

    def handle_continue_statement(self, bs: Tree, cxt: Context):
        while_loop_labels = cxt.whiles[-1]
        start_label, final_label = while_loop_labels
        if not start_label or not final_label:
            raise Exception("Continue statement found without while loop")
        cxt.code.append(Op("goto", f"{start_label}", None))

    def handle_break_statement(self, bs: Tree, cxt: Context):
        while_loop_labels = cxt.whiles[-1]
        start_label, final_label = while_loop_labels
        if not start_label or not final_label:
            raise Exception("Break statement found without while loop")
        cxt.code.append(Op("goto", f"{final_label}", None))

    def handle_while_block(self, wb: Tree, cxt: Context, start_label: str, final_label: str) -> None:
        for child in wb.children:
            if isinstance(child, Token):
                pass
            else:
                if child.data == AstSymbols.expression:
                    self.handle_expression(child, cxt)
                    cxt.code.append(Op("jneq", f"{final_label}", None))

                elif child.data == AstSymbols.while_body:
                    self.handle_while_body(child, cxt)
                    cxt.code.append(Op("goto", f"{start_label}", None))

    def handle_type_decl(self, t: Tree, cxt: Context) -> Datatype:
        ty = t.children[0]
        return Datatype(DT.from_string(ty), None, ty)

    def handle_array_decl(self, ad, cxt) -> Datatype:
        for child in ad.children:
            if child.data == AstSymbols.expression_list:
                return self.handle_expression_list(child, cxt)
            elif child.data == AstSymbols.expression:
                return self.handle_expression(child, cxt)
            else:
                raise ValueError(
                    f"Unable to read array declaration at LINE: {ad.meta.line}.")

    def handle_expression_list(self, el, cxt) -> Datatype:
        dtypes = []
        for child in el.children:
            if child.data == AstSymbols.expression:
                r = self.handle_expression(child, cxt)
                dtypes.append(r)
            else:
                r = self.handle_expression_list(child, cxt)
                dtypes.append(r)
        res = infer_type_arr(el, dtypes, None)
        return Datatype(res, None, None)

    def handle_array_index(self, ai, cxt) -> str:
        varname = ""
        lv = None
        for child in ai.children:
            if isinstance(child, Token):
                varname = child
                lv = LocalVar(cxt.local_var_counter, varname, None)
            elif child.data == AstSymbols.expression:
                r = self.handle_expression(child, cxt)
                if lv:
                    lv.data_type = r
        if str(varname) not in cxt.local_vars.keys():
            cxt.local_vars[str(varname)] = lv
            cxt.local_var_counter += 1
        return str(varname)

    def handle_assign_array_index(self, aai, ctx) -> None:
        op = None
        for child in aai.children:
            if child.data == AstSymbols.array_index:
                vname = self.handle_array_index(child, ctx)
                v_data = ctx.find_param_for_name(vname)
                if v_data:
                    op = Op("set_param_idx", v_data[1], Datatype(DT.INT8, 0, None))
                else:
                    v_data = ctx.find_local_var_for_name(vname)
                    if v_data:
                        op = Op("set_local_idx", v_data[1], Datatype(DT.INT32, 0, None))
                if not v_data:
                    raise ValueError(
                        f"The variable {vname} could not be found in the current context {ctx.name} at LINE: {aai.meta.line}.")
            elif child.data == AstSymbols.expression:
                self.handle_expression(child, ctx)
        if op:
            ctx.code.append(op)

    def handle_assign_record_field(self, arf, cxt: Context) -> None:
        for child in arf.children:
            if isinstance(child, Token):
                fieldname = child
            elif child.data == AstSymbols.record_name:
                varname = child.children[0]
            elif child.data == AstSymbols.expression:
                r = self.handle_expression(child, cxt)

        if varname in cxt.local_vars:
            lv: LocalVar = cxt.local_vars[varname]

            record = lv.content
            for i in range(len(record.local_vars)):
                if record.local_vars[i][0] == fieldname:
                    field_index = i
                    op = Op("push", field_index, Datatype(DT.INT32, field_index, None))
                    cxt.code.append(op)
                    op = Op("set_record_idx", varname, None)
                    cxt.code.append(op)
        else:
            for i in range(len(cxt.params)):
                p, dt = cxt.params[i]
                if p == varname:
                    record: Record = cxt.get_record(dt.info)
                    for j in range(len(record.local_vars)):
                        if record.local_vars[j][0] == fieldname:
                            field_index = j
                            op = Op("push", field_index, Datatype(DT.INT32, field_index, None))
                            cxt.code.append(op)
                            op = Op("set_param_record_idx", varname, None)
                            cxt.code.append(op)

    def handle_record_initialization(self, ri, cxt: Context) -> Datatype:

        param_idx = 0
        record_type_name = None

        for child in ri.children:
            if isinstance(child, Token):
                pass
            elif child.data == AstSymbols.record_name:
                for gc in child.children:
                    record_type_name = gc
                    # TODO check if this exists in context
                    rec = cxt.get_record(record_type_name)

                    if not rec:
                        raise Exception(
                            f"Record {record_type_name} not found in the current context at LINE: {ri.meta.line}.")

            elif child.data == AstSymbols.record_arguments:
                self.handle_record_arguments(child, record_type_name, param_idx, cxt)

        cxt.code.append(Op("push", f"{len(rec.local_vars)}", Datatype(DT.INT32, len(rec.local_vars), None)))
        record = Record("")
        record.local_vars = rec.local_vars

        return Datatype(DT.RECORD, record, record_type_name)

    def handle_record_arguments(self, args: Tree, record_name: str, param_idx: int, cxt: Context) -> None:
        for child in args.children:
            if child.data == AstSymbols.expression:
                r = self.handle_expression(child, cxt)
                dd = cxt.get_record(record_name)
                rec_type = cxt.get_record(record_name).local_vars[param_idx][1]
                if r.dtype != rec_type.dtype:
                    if rec_type.dtype in (DT.INT8, DT.INT32, DT.INT64) and \
                            r.dtype in (DT.INT8, DT.INT32, DT.INT64):
                        r.dtype = rec_type.dtype
                    elif rec_type.dtype in (DT.FLOAT32, DT.FLOAT64) and \
                            r.dtype in (DT.FLOAT32, DT.FLOAT64):
                        r.dtype = rec_type.dtype
                    else:
                        raise ValueError(
                            f"Record Parameter type mismatch for {record_name}: {r.dtype} and {cxt.get_record(record_name).local_vars[param_idx][1].dtype}  at LINE: {args.meta.line}.")
                param_idx += 1

            elif child.data == AstSymbols.record_arguments:
                self.handle_record_arguments(child, record_name, param_idx, cxt)

    def handle_push_expression_index(self, pei: Tree, cxt: FunContext) -> DT:
        var = None
        for child in pei.children:
            if isinstance(child, Token):
                field_name = child
            elif child.data == AstSymbols.expression:
                if not var:
                    var = self.handle_expression(child, cxt)
                else:
                    d = self.handle_expression(child, cxt)

        op = Op("push_stack_a", None, None)
        cxt.code.append(op)
        # raise ValueError
        return var

    def handle_push_expression_field(self, rf: Tree, cxt: Context) -> DT:
        field_name = None
        for child in rf.children:
            if isinstance(child, Token):
                field_name = child
            elif child.data == AstSymbols.expression:
                var = self.handle_expression(child, cxt)
                record = cxt.get_record(var.info)

            for i in range(len(record.local_vars)):
                if record.local_vars[i][0] == field_name:
                    op = Op("push", i, Datatype(DT.INT32, i, None))
                    cxt.code.append(op)
                    op = Op("push_stack_r", field_name, None)
                    cxt.code.append(op)
        return var

    def handle_push_record_field(self, rf: Tree, cxt: Context) -> DT:
        field_name = None
        for child in rf.children:
            if isinstance(child, Token):
                field_name = child
            elif child.data == AstSymbols.record_name:
                var_name = child.children[0]
        if var_name in cxt.local_vars:
            lv: LocalVar = cxt.local_vars[var_name]

            record = cxt.get_record(lv.data_type.info)

            for i in range(len(record.local_vars)):
                if record.local_vars[i][0] == field_name:
                    op = Op("push", i, Datatype(DT.INT32, i, None))
                    cxt.code.append(op)
                    op = Op("push_local_r", var_name, None)
                    cxt.code.append(op)
                    return record.local_vars[i][1]
        else:
            for i in range(len(cxt.params)):
                p, dt = cxt.params[i]
                if p == var_name:
                    record: Record = cxt.get_record(dt.info)
                    for j in range(len(record.local_vars)):
                        if record.local_vars[j][0] == field_name:
                            op = Op("push", j, Datatype(DT.INT32, j, None))
                            cxt.code.append(op)
                            op = Op("push_param_r", var_name, None)
                            cxt.code.append(op)
                            return record.local_vars[i][1]

        return None

    def handle_record_type_decl(self, rd, cxt: Context):
        rec = None
        for child in rd.children:
            if isinstance(child, Token):
                continue
            if child.data == AstSymbols.record_name:
                name = child.children[0]
                rec = Record(str(name))
                cxt.records.append(rec)
            elif child.data == AstSymbols.record_var_list:
                self.handle_record_var_list(child, cxt, rec)

    def handle_record_var_list(self, rvl, cxt: Context, rec: Record):
        for child in rvl.children:

            if child.data == AstSymbols.record_var_param:
                self.handle_record_var_param(child, cxt, rec)
            else:
                if child.data == AstSymbols.record_var_list:
                    self.handle_record_var_list(child, cxt, rec)

    def handle_record_var_param(self, vp: Tree, cxt: Context, rec: Record) -> None:
        vname = vp.children[0]
        vtype = vp.children[1]
        rec.local_vars.append((str(vname), Datatype(DT.from_string(vtype), None, vtype)))

    def handle_for_block(self, fb: Tree, cxt: Context, start_label: str, final_label: str) -> None:
        iter_exp = None
        for child in fb.children:
            if isinstance(child, Token):
                pass
            elif child.data == AstSymbols.init_expression:
                pass
            elif child.data == AstSymbols.stop_expression:
                self.handle_stop_expression(child, cxt)
                cxt.code.append(Op("jneq", f"{final_label}", None))
            elif child.data == AstSymbols.iter_expression:
                iter_exp = child
                pass
            elif child.data == AstSymbols.for_body:
                self.handle_for_body(child, cxt)
                self.handle_iter_expression(iter_exp, cxt)
                cxt.code.append(Op("goto", f"{start_label}", None))

    def handle_init_expression(self, se: Tree, cxt: Context) -> None:
        for child in se.children:
            self.handle_expression(child, cxt)

    def handle_stop_expression(self, se: Tree, cxt: Context) -> None:
        for child in se.children:
            self.handle_expression(child, cxt)

    def handle_iter_expression(self, se: Tree, cxt: Context) -> None:
        for child in se.children:
            self.handle_expression(child, cxt)

    def handle_for_body(self, fb: Tree, cxt: Context) -> None:
        for child in fb.children:
            if child.data == AstSymbols.statement:
                r = self.handle_statement(child, cxt)

    def compile_program(self, program, cxt: Context):
        parse_tree = self.parser.parse(program)

        for statement in parse_tree.children:
            self.handle_statement(statement, cxt)
