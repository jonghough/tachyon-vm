#!/bin/bash

set -ex
pwd=`pwd`
cd ../virtual_machine
for file in $pwd/output/*.btyarc; do
    ./tachyon -s 600 -h 1000 -p $file -d 0
done