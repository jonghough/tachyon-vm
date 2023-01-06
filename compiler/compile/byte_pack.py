import zlib
import zipfile
import os
import pathlib


def pack_to_ty_code(program: str):
    s = bytes(program, 'utf-8')  # Or other appropriate encoding
    code = zlib.compress(program.encode('utf-8'))
    x = zlib.decompress(code)
    return code


def zipdir(file_path, zip_file : zipfile.ZipFile):
    # ziph is zipfile handle
    if os.path.isdir(file_path):
        for root, dirs, files in os.walk(file_path):
            for file in files:
                zip_file.write(os.path.join(root, file),
                               os.path.relpath(os.path.join(root, file),
                                               os.path.join(file_path, '..')))
    else:
        arc=file_path.split("/")[-1]
        zip_file.write(file_path,arc)


def pack_to_ty_code_zipped(program_file: str, output_name: str):
    with zipfile.ZipFile(output_name, 'w', zipfile.ZIP_DEFLATED) as zipf:
        zipdir(program_file, zipf)


