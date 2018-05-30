all: A4_e1 sudoku sudoku-orig partial partial-orig 

A4_e1:
	gcc -Wall -O2 -std=c11 -pedantic -fopenmp -o A4_e1.o A4_e1.c
sudoku:
	gcc -Wall -O2 -std=c11 -pedantic -fopenmp -o sudoku.o sudoku.c -lm
sudoku-orig:
	gcc -Wall -O2 -std=c11 -pedantic -fopenmp -o sudoku-orig.o sudoku-orig.c -lm
partial:
	gcc -Wall -O3 -std=c11 -pedantic -fopenmp -o partial.o partial.c -lm
partial-orig:
	gcc -Wall -O3 -std=c11 -pedantic -fopenmp -o partial-orig.o partial-orig.c -lm	
