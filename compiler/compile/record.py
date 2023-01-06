from compile.local_vars import LocalVar


class Record:
    def __init__(self, name) -> None:
        self.call_index = -1
        self.name = name
        self.local_vars: list[(str, LocalVar)] = []