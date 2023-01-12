# Tachyon Virtual Machine



### running example

```
./build/tachyon -s 600 -h 1000 -p ../compiler/output/countdown.btyarc -d 0
```

### Options and arguments

`-s` is the max stack size, in "cells", which are 4 bytes. 

`-h` is the max heap size in bytes (unused), waiting for a custom allocator.

`-p` is path to the file to be run in the VM.

`-d` is debugging flag. If 1, then some debug output will be displayed. If 0 then no debug output.


## Compiling

use `cmake` (optionally set compiler. I used clang below)

```
cmake . -DCMAKE_C_COMPILER=clang
```

to create the Makefile. Then simply use `make`

```
make
```
to compile. 

The output executable `tachyon` will be found in the `build/` subdirectory.

### Platforms

The comilation will probably only work on **Linux**, using GCC (Ubuntu 9.4.0-1ubuntu1~20.04.1) and Clang (10.0.0-4ubuntu1) and has only been tested on Linux. 