#ifndef SUDOKU_ANNOTATIONS_H
#define SUDOKU_ANNOTATIONS_H

#define NOTE_BIT(n) (1u << ((n) - 1))   // n = 1..9
#include <stdint.h>
extern uint16_t sudokuNotes[9][9];

void note_toggle(int x, int y, int n);
void removeNotes(uint16_t sudokuNotes[9][9], int x, int y, int n);

#endif //SUDOKU_ANNOTATIONS_H