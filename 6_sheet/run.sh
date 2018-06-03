#!/bin/bash
echo "real;user;sys;" >> time-o2.txt
echo "real;user;sys;" >> time-o3.txt

for i in {1..50}; do
	/usr/bin/time -o time-o2.txt -a -f "%E;%U;%S;" mpirun -n 2 ./v3-conduct-o2.o
	/usr/bin/time -o time-o3.txt -a -f "%E;%U;%S;" mpirun -n 2 ./v3-conduct-o3.o
	echo "Done $i of 50"
done

