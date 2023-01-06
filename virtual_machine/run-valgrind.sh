#!/bin/bash


valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all `pwd`/build/tachyon -s 600 -h 1000 -p ../compiler/output/quicksort.btyarc  -d 0  