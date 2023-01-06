class MissingReturnStatement(Exception):

    def __init__(self, func: str):
        self.message = f"Function {func} lacks at least one return statement."
        super().__init__(self.message)