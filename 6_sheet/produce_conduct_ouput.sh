#!/bin/bash

# Run with number of processes as argument to produce output.

mpirun -n $1 ./v1-conduct.o -o v1-out
mpirun -n $1 ./v2-conduct.o -o v2-out
mpirun -n $1 ./v3-conduct.o -o v3-out