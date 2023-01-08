# Tachyon VM and Programming Language

This repository is a POC, test stack based VM, and accompanying programming
language, with a compiler. The purpose of this project is not to create
a real-world usable programming language and VM, but merely for educaitonal
purposes, and fun! 

## VM

The Virtual Machine is written in C. For simplicity it only has 3 fundamental
data types:

1. ints (8/32/64 bit integers)
2. floats (32/64 bit floats)
3. strings 

as well as
4. arrays, of homogeneous data items. (e.g. an int array, float array etc)
5. records, similar to C structs.

The VM reads bytecode and runs the encoded instructions.
A single stack is used, along with a few registers.

# Tachyon Virtual Machine



### running example

Note the `tachyon` virtual machine executable is assumed located in the
`./virtual_machine/build` directory, which is the location it is put when
compiled.

```
./build/tachyon -s 600 -h 1000 -p ../compiler/output/countdown.btyarc -d 1
```

### Options and arguments

`-s` is the max stack size, in "cells", which are 4 bytes. 

`-h` is the max heap size in bytes (unused), waiting for a custom allocator.

`-p` is path to the file to be run in the VM.

`-d` is debugging flag. If 1, then some debug output will be displayed. If 0 then no debug output.


## Language and Compiler

The `Tachyon` programming language ius a simple language, that compiles into
the Tachyon VM bytecode.

### Compiler

The compiler is written in Python, and uses the `Lark` library for parsing the
Tachyon `.ty` files and then compiles the code into `tachyon bytecode`. The
result is a `btyarc` file, which can be read by the tachyon virtual machine.


# Tachyon VM Compiler

## Compiler
The parser and compiler are written in Python. The Parser uses `Lark` to aprse the grammar for the toy **Tachyon** language.

### Compiling Source Code

```commandline
python main.py --program /path/to/source.ty -v 1
```


### Syntax

```
function main() -> i32 {
 ...
 1;
}

main();
```
## Datatypes

The  main datatypes are 
1. `i8`, `i32`, `i64` : 8,32,64-bit signed integers.
2. `f32`, `f64`: 32,64-bit floating point numbers.
3. `str`: Strings, delimited by the `'` (single quote) character, not double quotes (`"`).

The other datatypes are 
1. arrays
2. records

Array size must be given at compile time. Array items must all have the same type.
Records are similar to C structs. They can hold different item types.

All variables are passed by value and there are no pointers or references, limiting the language somewhat.

### Defining an array

```
arr : i8[10] = [1,2,3,4,5,6,7,8,9,10];
```
### Defining a record type
```
record vector (
 x : f32,
 y : f32,
 z : f32
);
```

### Declaring a record
```
a : vector = rec vector(0.0,0.0,0.0);
```

## Example programs

```

function xyz(var1 : i32, var2 : i32) -> f32 {

    f : f32 = 0.0;
    f = (!f32!var1) + (!f32!var2);
    f;
}

function main() ->i32 {
    f : f32 = xyz(1,10);
    print(f);
    1;
}

main();
```


