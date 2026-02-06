#ifndef SUDOKU_ASSETS_H
#define SUDOKU_ASSETS_H

#include <raylib.h>

void Assets_Init(void);

Font Assets_LoadFont(const char *name);
Texture2D Assets_LoadTexture(const char *name);
Image Assets_LoadImage(const char *name);
Music Assets_LoadMusic(const char *name);

#endif //SUDOKU_ASSETS_H