#include "generator.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool unUsedInBox(GridPtr grid, int rowStart, int colStart, int num) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[rowStart + i][colStart + j] == num) {
                return false;
            }
        }
    }
    return true;
}

void fillBox(GridPtr grid, int row, int col) {
    int num;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            do {
                num = (rand() % 9) + 1;
            } while (!unUsedInBox(grid, row, col, num));
            grid[row + i][col + j] = num;
        }
    }
}

bool unUsedInRow(GridPtr grid, int i, int num) {
    for (int j = 0; j < N; j++) {
        if (grid[i][j] == num) {
            return false;
        }
    }
    return true;
}

bool unUsedInCol(GridPtr grid, int j, int num) {
    for (int i = 0; i < N; i++) {
        if (grid[i][j] == num) {
            return false;
        }
    }
    return true;
}

bool checkIfSafe(GridPtr grid, int i, int j, int num) {
    return (unUsedInRow(grid, i, num) &&
            unUsedInCol(grid, j, num) &&
            unUsedInBox(grid, i - i % 3, j - j % 3, num));
}

void fillDiagonal(GridPtr grid) {
    for (int i = 0; i < N; i = i + 3) {
        fillBox(grid, i, i);
    }
}

bool fillRemaining(GridPtr grid, int i, int j) {
    if (i == N - 1 && j == N) {
        return true;
    }

    if (j == N) {
        i++;
        j = 0;
    }

    if (grid[i][j] != 0) {
        return fillRemaining(grid, i, j + 1);
    }

    for (int num = 1; num <= 9; num++) {
        if (checkIfSafe(grid, i, j, num)) {
            grid[i][j] = num;
            if (fillRemaining(grid, i, j + 1)) {
                return true;
            }
            grid[i][j] = 0;
        }
    }

    return false;
}

void countSolutions(GridPtr grid, int *count) {
    if (*count > 1) return;

    int row = -1, col = -1;
    bool isEmpty = false;

    // Buscar celda vacía
    for (int i = 0; i < N && !isEmpty; i++) {
        for (int j = 0; j < N && !isEmpty; j++) {
            if (grid[i][j] == 0) {
                row = i;
                col = j;
                isEmpty = true;
            }
        }
    }

    if (!isEmpty) {
        (*count)++;
        return;
    }

    for (int num = 1; num <= 9; num++) {
        if (checkIfSafe(grid, row, col, num)) {
            grid[row][col] = num;
            countSolutions(grid, count);
            grid[row][col] = 0;
        }
    }
}

void removeDigits(GridPtr grid, int k) {
    int positions[N * N];
    for (int i = 0; i < N * N; i++) positions[i] = i;

    for (int i = (N * N) - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }

    int countEmpty = 0;

    for (int i = 0; i < N * N; i++) {
        if (countEmpty >= k) break;

        int cellId = positions[i];
        int r = cellId / N;
        int c = cellId % N;

        if (grid[r][c] != 0) {
            int backup = grid[r][c];
            grid[r][c] = 0;

            int solutions = 0;
            countSolutions(grid, &solutions);

            if (solutions != 1) {
                grid[r][c] = backup;
            } else {
                countEmpty++;
            }
        }
    }
}

void sudokuGenerator(GridPtr grid) {
    if (!grid) return;

    memset(grid, 0, sizeof(int[N][N]));

    fillDiagonal(grid);

    if (!fillRemaining(grid, 0, 3)) {
        TraceLog(LOG_ERROR, "Can't generate the sudoku");
        exit(EXIT_FAILURE);
    }
}
