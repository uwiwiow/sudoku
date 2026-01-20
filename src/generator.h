#ifndef SUDOKU_GENERATOR_H
#define SUDOKU_GENERATOR_H

#define N 9
typedef int (*GridPtr)[N];

void sudokuGenerator(GridPtr grid);

void removeDigits(GridPtr grid, int k);

#endif //SUDOKU_GENERATOR_H