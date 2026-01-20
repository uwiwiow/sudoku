#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "generator.h"

#define debug 0

constexpr int MAX_BUFFER = 128;
char *buffer = nullptr;
int bufferIndex = 0;

typedef struct Vector2i {
    int x;                // Vector x component
    int y;                // Vector y component
} Vector2i;

void debugText(const char *text) {
    int len = (int)strlen(text);

    if (bufferIndex + len + 2 >= MAX_BUFFER) {
        bufferIndex = 0;
        buffer[0] = '\0';
    }

    if (bufferIndex != 0) {
        buffer[bufferIndex++] = '\n';
    }

    memcpy(buffer + bufferIndex, text, len);
    bufferIndex += len;
    buffer[bufferIndex] = '\0';
}

int main(int argc, char **argv) {

    buffer = (char *) malloc(MAX_BUFFER * sizeof(char));
    if (!buffer) {
        TraceLog(LOG_ERROR, "Out of memory");
        return 1;
    }
    buffer[0] = '\0';
    bufferIndex = 0;


    constexpr int screenWidth = 720;
    constexpr int screenHeight = 880;

    constexpr int gridWidth = 720;
    constexpr int gridHeight = 720;
    const Rectangle gridRect = {0, 80, gridWidth, gridHeight};
    constexpr int cellSize = gridWidth / 9;

    const Color backgroundColor = {32, 33, 36, 255};
    const Color selectedLineColor = {42, 91, 176, 255};
    const Color selectedColor = {28, 57, 107, 255};
    const Color selectedAreaColor = {71, 71, 71, 255};
    const auto boxColor = GRAY;
    const auto lineColor = DARKGRAY;
    const auto numberColor = WHITE;


    srand((unsigned int)time(nullptr));
    int k = 40;

    int _grid[N][N];
    GridPtr _sudoku = _grid;
    sudokuGenerator(_sudoku);
    int grid[N][N];
    memcpy(grid, _grid, sizeof(int[N][N]));
    GridPtr sudoku = grid;
    removeDigits(sudoku, k);


    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Sudoku");
    InitAudioDevice();
    SetTargetFPS(200);


    const Font font = LoadFont("assets/lexend-latin-600-normal.ttf");


    Music music = LoadMusicStream("assets/Troubadeck 12 Good King.ogg");
    PlayMusicStream(music);
    music.looping = true;


    Image _heart = LoadImage("assets/heart.png");
    ImageResize(&_heart, 60, 60);
    const Texture2D heart = LoadTextureFromImage(_heart);
    UnloadImage(_heart);
    int lifes = 3;


    Vector2i selector = {0, 0};
    bool selected = false;



    while (!WindowShouldClose()) {

        UpdateMusicStream(music);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            const Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, gridRect)) {
                selected = true;

                selector.x = (int) mousePos.x / ( cellSize);
                selector.y = (int) (mousePos.y - 80) / ( cellSize);

                debugText(TextFormat("%d %d", selector.x, selector.y));

            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) selected = false;


        if (IsKeyPressed(KEY_R)) {
            selected = false;
            lifes = 3;
            sudokuGenerator(_sudoku);
            memcpy(grid, _grid, sizeof(int[N][N]));
            removeDigits(sudoku, k);
        }


        if (IsKeyPressed(KEY_MINUS)) k--;
        if (IsKeyPressed(KEY_EQUAL)) k++;


        const int step = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) ? 3 : 1;
        if (IsKeyPressed(KEY_UP)) {     if (selector.y > -1 + step) selector.y -= step; debugText(TextFormat("%d %d", selector.x, selector.y));}
        if (IsKeyPressed(KEY_DOWN)) {   if (selector.y < 9 - step)  selector.y += step; debugText(TextFormat("%d %d", selector.x, selector.y));}
        if (IsKeyPressed(KEY_LEFT)) {   if (selector.x > -1 + step) selector.x -= step; debugText(TextFormat("%d %d", selector.x, selector.y));}
        if (IsKeyPressed(KEY_RIGHT)) {  if (selector.x < 9 - step)  selector.x += step; debugText(TextFormat("%d %d", selector.x, selector.y));}


        int keyPressed = GetKeyPressed();
        if (keyPressed >= KEY_ONE && keyPressed <= KEY_NINE && selected) {
            int value = keyPressed - KEY_ONE + 1;
            int row = selector.y;
            int col = selector.x;

            if (sudoku[row][col] == 0) {
                if (_sudoku[row][col] == value) {
                    sudoku[row][col] = value;
                } else lifes--;
            }
        }



        BeginDrawing();
        ClearBackground(backgroundColor);


        // selector
        if (selected) {
            DrawRectangleRec((Rectangle){(float) selector.x * 80, (float) (selector.y + 1) * 80, cellSize, cellSize}, selectedColor);
            DrawRectangleLinesEx((Rectangle){(float) selector.x * 80, (float) (selector.y + 1) * 80, cellSize, cellSize}, 4, selectedLineColor);
        }

        // lines
        for (int i = 0; i < 10; i++) {
            const Color drawColor = (i) % 3 == 0 ? boxColor : lineColor;
            DrawLine(cellSize * (i), 80,
                cellSize * (i), gridHeight + 80, drawColor);
            DrawLine(0, cellSize * (i) + 80, gridWidth,
                cellSize * (i) + 80, drawColor);
        }


        // numbers
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (sudoku[i][j] != 0) {
                    const Vector2 textPos = {(float)j * (float)cellSize + (float)cellSize/3, (float)i * (float)cellSize + (float)cellSize/4 + 80};
                    DrawTextEx(font, TextFormat("%d", sudoku[i][j]), textPos, 40, 0, numberColor);
                }
            }
        }


        // lifes
        for (int i = 0; i < 3; i++) DrawTexture(heart, 10 + i * 70, 10, lifes <= i ? GRAY : RED);
        if (lifes <= 0) DrawText("   PRESS R\nTO RESTART", (screenWidth / 2) - (MeasureText("   PRESS R\nTO RESTART", 60) / 2), screenHeight / 2 - 60, 60, GOLD);


        if (debug) DrawText(buffer, 10, 10, 20, DARKGREEN);

        EndDrawing();

    }

    UnloadMusicStream(music);
    UnloadFont(font);
    UnloadTexture(heart);
    CloseAudioDevice();
    CloseWindow();

    free(buffer);
    buffer = nullptr;
    return 0;
}