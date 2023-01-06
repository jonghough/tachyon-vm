from dataclasses import dataclass
from compile.local_vars import Datatype


@dataclass
class Op:
    op_type: str
    operation: str
    datatype: Datatype
    index: int = 0
