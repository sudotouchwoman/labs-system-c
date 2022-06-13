#!/bin/bash

[ "$#" -eq 1 ] && NUM_WORKERS=$1 || NUM_WORKERS=2

mpicc main.c -o main
mpiexec -n $NUM_WORKERS ./main
gnuplot gnu.config
