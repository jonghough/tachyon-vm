#!/bin/bash

set -ex 
for file in $PWD/../compiler/output/*.btyarc; do 
    rp=$(realpath $file);
    ./build/tachyon -s 600 -h 1000 -p $rp -d 0
done