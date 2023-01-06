import click

from compile.code_writer import write_data, write_binary_data
from compile.codegen import CodeGenerator
from compile.compiler import Compiler
from tachyonvm.tachyonvm import TachyonVM


@click.command()
@click.option('--program', type=str, prompt='Program file path',
              help='The path to the program file.')
@click.option("--trace", "-t", type=bool, default=False,
              help='Show Python stacktrace on error.')
@click.option("--vm", "-v", type=bool, default=False,
              help='Run the compiled code in the test VM after compilation.')
@click.option("--debug", "-d", type=bool, default=False,
              help='If running the code in the test VM, produce debug output.')
@click.option("--readable", "-r", type=bool, default=False,
              help='If true, the compiler will output no only the binary byte code, '
                   'but also a readable text file showing the opcodes .')
def run(program, trace, vm, debug, readable) -> None:
    with open(program) as p:
        prog = p.read()

        with open("./grammar/tachyon_grammar.txt", "r") as f:
            grammar = f.read()
            if debug:
                print(grammar)
            # parser = Lark(grammar)
            compiler = Compiler(grammar, trace=trace)
            if debug:
                print(compiler.parser.parse(prog).pretty())
                print("\n\n\n")
            compiler.compile(prog)
            # ops = compiler.cxt.show()
            cg = CodeGenerator(compiler.cxt)
            ops = cg.generate()
            for op in ops:
                print(f"{op.as_text()}")
            code = '\n'.join([o.as_text() for o in ops])

            binary_code = b''.join([o.as_binary() for o in ops])

            if readable:
                write_data(program, code)
            write_binary_data(program, binary_code)
            if vm:
                tvm = TachyonVM(code, debug=debug)
                tvm.run()


if __name__ == "__main__":
    run()
