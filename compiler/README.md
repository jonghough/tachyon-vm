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
 return 1;
}

main();
```
## Datatypes

The six main datatypes are 
1. `i8` : 8-bit signed integers.
1. `i32` : 32-bit signed integers.
2. `i64` : 64-bit signed integers.
2. `f32`: 32-bit floating point numbers.
2. `f64`: 64-bit floating point numbers.
3. `str`: Strings, delimited by the `'` (single quote) character, not double quotes (`"`).

The other datatypes are 
1. arrays - fixed sized homogeneous arrays.
2. records - multiple types allowed (similar to C structs)

Array size must be given at compile time. Array items must all have the same type.
Records are similar to C structs. They can hold different item types.

All variables are passed by value and there are no pointers or references, limiting the language somewhat.

### Defining an array

```
arr : i32[10] = [1,2,3,4,5,6,7,8,9,10];
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

function xyz(var1 : i32, var2 : i64) -> f64 {

    f : f64 = 0.0;
    f = (!f64!var1) + (!f64!var2);
    return f;
}

function main() ->i32 {
    f : f32 = xyz(1,10);
    print(f);
    return 1;
}

main();
```

### Loops


#### While loops

While loop example

```
function exampleLoop(a : f32) -> i32 {
    i : i32 = 0;
    while (i < a) {
        print(i);
        i = i+1;
    }
    return 1;
}

function main () -> i32 {
    exampleLoop(12.5);
    return 1;
}

main();
```

#### For loops

For loop example

```
function exampleLoop() -> i32 {
    i : i32 = 0;
    j : i32 = 0;
    for(i = 0; i< 10; i=i+1){
        for(j = 0; j < i; j=j+1){
            print("i + j is "+(!str!(i+j)));
        }
    }
    return 1;
}

function main () -> i32 {
    exampleLoop();
    return 1;
} 
main();
```

### Comments

There are no comments, single line or multi-line.