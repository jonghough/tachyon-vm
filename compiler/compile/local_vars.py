from enum import Enum


class DT(Enum):
    INT8 = 1
    INT32 = 2
    INT64 = 3
    FLOAT32 = 4
    FLOAT64 = 5
    STR = 6
    RECORD = 7

    @staticmethod
    def from_string(name):
        if name == "i8" or name == "i8[]":
            return DT.INT8
        elif name == "i32" or name == "i32[]":
            return DT.INT32
        elif name == "i64" or name == "i64[]":
            return DT.INT64
        elif name == "f32" or name == "f32[]":
            return DT.FLOAT32
        elif name == "f64" or name == "f64[]":
            return DT.FLOAT64
        elif name == "str" or name == "str[]":
            return DT.STR
        elif len(name) > 0:
            return DT.RECORD
        else:
            raise ValueError(f"datatype unknown for {name}")

    @staticmethod
    def to_string(dt):
        if dt == DT.INT8:
            return "i8"
        if dt == DT.INT32:
            return "i32"
        if dt == DT.INT64:
            return "i64"
        elif dt == DT.FLOAT32:
            return "f32"
        elif dt == DT.FLOAT64:
            return "f64"
        elif dt == DT.STR:
            return "str"
        raise ValueError(f"Cannot infer the type {dt}")


class Datatype:

    def __init__(self, dtype: DT, val: any, info: any, initialized: bool = False):
        self.dtype = dtype
        self.val = val
        self.info = info
        self.initialized = initialized
        if isinstance(dtype, Datatype):
            raise Exception


class LocalVar:

    def __init__(self, index: int, variable_name: str,
                 data_type: Datatype, content=None):
        if isinstance(data_type, DT):
            raise Exception("Datatype is not correct type")
        if index is None:
            raise ValueError("Local variables need an explicit index")
        self.index = index
        self.variable_name = variable_name
        self.data_type = data_type
        self.is_scalar = True
        self.content = content
