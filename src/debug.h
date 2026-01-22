#ifndef SUDOKU_DEBUG_H
#define SUDOKU_DEBUG_H

#define debug 0

constexpr int MAX_BUFFER = 128;
extern char *buffer;
extern int bufferIndex;

void debugText(const char *text);

#endif //SUDOKU_DEBUG_H