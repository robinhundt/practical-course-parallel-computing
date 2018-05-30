#!/bin/bash

export NUM_THREADS=4
for i in {1..5000}; do
    # unset OMP_SCHEDULE
    ./sudoku-orig.o  >> sudoku-orig-4th.data
    # export OMP_SCHEDULE="static"
    # ./sudoku.o  >> sudoku-4th-static.data
    # export OMP_SCHEDULE="dynamic"
    # ./sudoku.o  >> sudoku-4th-dynamic.data
    # export OMP_SCHEDULE="guided"
    # ./sudoku.o  >> sudoku-4th-guided.data
    # export OMP_SCHEDULE="auto"
    # ./sudoku.o  >> sudoku-4th-auto.data
    echo "Done $i"
done
