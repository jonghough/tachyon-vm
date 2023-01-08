from pathlib import Path

from compile.byte_pack import pack_to_ty_code_zipped, pack_to_ty_code


def write_data(program: str, code: str, output_dir: Path) -> None:
    name = program.split("/")[-1].split(".")[0]
    tcode_file = output_dir / f"{name}/.tcode"
    tycode_file = output_dir / f"{name}/.tycode"
    with open(str(tcode_file), "w") as tcode:
        tcode.write(code)
    with open(str(tycode_file), "wb") as tbcode:
        tbcode.write(pack_to_ty_code(code))


def write_binary_data(program: str, code: bytes, output_dir: Path) -> None:
    name = program.split("/")[-1].split(".")[0]

    btcode_file = output_dir / f"{name}.btcode"
    btyarc_file = output_dir / f"{name}.btyarc"
    with open(str(btcode_file), "wb") as tcode:
        tcode.write(code)
    pack_to_ty_code_zipped(str(btcode_file), str(btyarc_file))
