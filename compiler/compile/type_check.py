import re

from lark import Tree

from compile.local_vars import Datatype, DT


def push_for_type(dtype: DT) -> str:
    if dtype == DT.STR:
        return "push_s"
    elif dtype == DT.FLOAT32:
        return "push_f32"
    elif dtype == DT.FLOAT64:
        return "push_f64"
    elif dtype == DT.INT8:
        return "push_i8"
    elif dtype == DT.INT32:
        return "push_i32"
    elif dtype == DT.INT64:
        return "push_i64"
    else:
        raise ValueError(f"Cannot resolve type {dtype}.")


def check_binary_op(left: DT, right: DT) -> DT:
    if left == DT.INT:
        if right == DT.INT:
            return DT.INT
        elif right == DT.FLOAT:
            return DT.FLOAT
        elif right == DT.STR:
            raise ValueError(f"Type conflict between operation: {left} and {right}.")
    elif left == DT.FLOAT:
        if right == DT.INT:
            return DT.FLOAT
        elif right == DT.FLOAT:
            return DT.FLOAT
        elif right == DT.STR:
            raise ValueError(f"Type conflict between operation: {left} and {right}.")
    elif left == DT.STR:
        if right == DT.STR:
            return DT.STR
        else:
            raise ValueError(f"Type conflict between operation: {left} and {right}.")


def infer_type_arr(branch: Tree, data: list[Datatype], operator=None) -> DT:
    if operator:
        if operator in {"EQ", "NEQ", "GT", "GTE", "LT", "LTE"}:
            return DT.INT32
    if len(data) == 0:
        raise ValueError(f"Unknown datatype in array construction at LINE: {branch.meta.line}")
    s = set()
    for dt in data:
        s.add(dt.dtype)
    if len(s) == 1:
        return s.pop()

    if (DT.INT32 in s or DT.INT8 in s or DT.INT64) and (DT.FLOAT32 in s or DT.FLOAT64 in s) and DT.STR not in s:
        return DT.FLOAT64
    if len(s.intersection({DT.STR, DT.FLOAT64, DT.FLOAT32})) == 0 and \
            len(s.intersection({DT.INT32, DT.INT64, DT.INT8})) > 0:
        return DT.INT32
    if len(s.intersection({DT.STR, DT.INT8, DT.INT32, DT.INT64})) == 0 and \
            len(s.intersection({DT.FLOAT64, DT.FLOAT32})) > 0:
        return DT.FLOAT64

    err_str = ', '.join([str(x.dtype) for x in data])

    raise ValueError(f"Array has multiple incompatible datatypes. {err_str}, at LINE: {branch.meta.line}")


def infer_type(data: str) -> Datatype:
    if isinstance(data, int):
        return Datatype(DT.INT32, int(data), None)
    elif isinstance(data, float):
        return Datatype(DT.FLOAT64, float(data), None)
    elif len(data) >= 2 and data[0] == "\'" and data[-1] == "\'":
        return Datatype(DT.STR, data, None)

    elif '.' in data:
        if re.match('^-?[0-9.]*$', data):
            return Datatype(DT.FLOAT64, float(data), None)
    if re.match('^-?[0-9]*$', data):
        return Datatype(DT.INT32, float(data), None)
    alpha = re.search('[a-zA-Z]', data)
    specialchars = re.search('[$&+,:;=?@#|\'<>.^*()%!-]', data)
    alphau = re.search(r'[\u3040-\u309F]+', data, re.UNICODE)
    if alpha or specialchars or alphau:
        return Datatype(DT.STR, data, None)
    # probably redundant checks here
    if "." in data:
        return Datatype(DT.FLOAT64, float(data), None)
    num = re.search('[0-9]', data)
    if num:
        return Datatype(DT.INT32, int(data), None)
    if data == "\'\'":
        return Datatype(DT.STR, data, None)
    ws = re.search('\\s+', data)
    if ws:
        return Datatype(DT.STR, data, None)
    raise ValueError(f"Cannot infer the type for value {data}")


def infer_array_type(s: str) -> DT:
    if s == "i8[]":
        return DT.INT8
    if s == "i32[]":
        return DT.INT32
    if s == "i64[]":
        return DT.INT64
    if s == "f32[]":
        return DT.FLOAT32
    if s == "f64[]":
        return DT.FLOAT64
    if s == "str[]":
        return DT.STR
    return DT.RECORD


def check_cast(pre_cast_type: Datatype, post_cast_type: Datatype) -> bool:
    # if info exists then the type is array or record and cannot be cast (to or from)
    # if pre_cast_type.info or post_cast_type.info:
    #     return False
    return True
