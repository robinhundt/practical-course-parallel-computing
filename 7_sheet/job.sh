#!/bin/sh

#BSUB -q mpi-short

#BSUB -W 00:10

#BSUB -o ex7_nd.%J

#BSUB -n 2

#BSUB -R span[ptile=1]

#BSUB -a intelmpi

mpirun.lsf  ./benchmark -o data.out