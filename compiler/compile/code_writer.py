from compile.byte_pack import pack_to_ty_code_zipped, pack_to_ty_code


def write_data(program : str, code : str) -> None:
    name = program.split("/")[-1].split(".")[0]

    with open(f"./output/{name}.tcode", "w") as tcode:
        tcode.write(code)
    with open(f"./output/{name}.tycode", "wb") as tbcode:
        tbcode.write(pack_to_ty_code(code))

def write_binary_data(program : str, code : bytes) -> None:
    name = program.split("/")[-1].split(".")[0]

    with open(f"./output/{name}.btcode", "wb") as tcode:
        tcode.write(code)
    pack_to_ty_code_zipped(f"./output/{name}.btcode", f"./output/{name}.btyarc")
