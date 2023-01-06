#!/bin/bash

set -ex
for file in $(pwd)/programs/*.ty; do
  echo $file
  python main.py --program $file -v 1
done
