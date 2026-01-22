#include "annotations.h"

void note_toggle(int x, int y, int n) {
    sudokuNotes[y][x] ^= NOTE_BIT(n);
}

void removeNotes(uint16_t sudokuNotes[9][9], int x, int y, int n) {
    uint16_t mask = ~NOTE_BIT(n);

    for (int cx = 0; cx < 9; cx++) {
        sudokuNotes[y][cx] &= mask;
    }

    for (int cy = 0; cy < 9; cy++) {
        sudokuNotes[cy][x] &= mask;
    }

    int boxX = (x / 3) * 3;
    int boxY = (y / 3) * 3;

    for (int dy = 0; dy < 3; dy++) {
        for (int dx = 0; dx < 3; dx++) {
            sudokuNotes[boxY + dy][boxX + dx] &= mask;
        }
    }

    sudokuNotes[y][x] = 0;
}