#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "generator.h"

#include "debug.h"
char *buffer = nullptr;
int bufferIndex = 0;

#include "annotations.h"
#include <stdint.h>
uint16_t sudokuNotes[9][9];

typedef struct Vector2i {
    int x;                // Vector x component
    int y;                // Vector y component
} Vector2i;


void updateSelectedRec(Rectangle *vSelectedArea, Rectangle *hSelectedArea, Rectangle *boxSelectedArea, Vector2i selector, int cellSize, Rectangle gridRect) {
    vSelectedArea->x = (float) (selector.x * cellSize + (int) gridRect.x);
    vSelectedArea->y = gridRect.y;
    hSelectedArea->y = (float) (selector.y * cellSize + (int) gridRect.y);
    hSelectedArea->x = gridRect.x;
    boxSelectedArea->x = (float)(selector.x / 3 * (int) boxSelectedArea->width + (int) gridRect.x );
    boxSelectedArea->y = (float)(selector.y / 3 * (int) boxSelectedArea->width + (int) gridRect.y);
}

int main(int argc, char **argv) {

    buffer = (char *) malloc(MAX_BUFFER * sizeof(char));
    if (!buffer) {
        TraceLog(LOG_ERROR, "Out of memory");
        return 1;
    }
    buffer[0] = '\0';
    bufferIndex = 0;


    int screenWidth = 720;
    int screenHeight = 800;

    int cellSize = 80;
    Rectangle gridRect = {0, 0 + (float) cellSize, (float) cellSize * 9, (float) cellSize * 9};

    const Color backgroundColor = {32, 33, 36, 255};
    const Color selectedLineColor = {42, 91, 176, 255};
    const Color selectedColor = {28, 57, 107, 255};
    const Color selectedAreaColor = {71, 71, 71, 255};
    const auto boxColor = GRAY;
    const auto lineColor = DARKGRAY;
    const auto numberColor = GRAY;
    const Color numberAnnotationColor = { 100, 100, 100, 255 };

    Rectangle hSelectedArea = {gridRect.x, gridRect.y, gridRect.width, (float) cellSize};
    Rectangle vSelectedArea = {gridRect.x, gridRect.y, (float) cellSize, gridRect.height};
    Rectangle boxSelectedArea = {gridRect.x, gridRect.y, (float) cellSize * 3, (float) cellSize * 3};


    srand((unsigned int)time(nullptr));
    int k = 40;

    int _grid[N][N];
    GridPtr _sudoku = _grid;
    sudokuGenerator(_sudoku);
    int grid[N][N];
    memcpy(grid, _grid, sizeof(int[N][N]));
    GridPtr sudoku = grid;
    removeDigits(sudoku, k);
    bool annotationMode = false;


    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Sudoku");
    InitAudioDevice();
    SetTargetFPS(200);


    const Font font = LoadFont("assets/lexend-latin-600-normal.ttf");


    Music music = LoadMusicStream("assets/Troubadeck 12 Good King.ogg");
    PlayMusicStream(music);
    music.looping = true;
    bool musicPlaying = true;


    Image _heart = LoadImage("assets/heart.png");
    ImageResize(&_heart, 60, 60);
    const Texture2D heart = LoadTextureFromImage(_heart);
    UnloadImage(_heart);
    int lifes = 3;

    const Texture2D pen = LoadTexture("assets/pen.png");

    Image _help = LoadImage("assets/help.png");
    ImageResize(&_help, 60, 60);
    const Texture2D helpTexture = LoadTextureFromImage(_help);
    UnloadImage(_help);
    Rectangle helpRect = {(float) screenWidth - 70, 10, 60, 60};


    Vector2i selector = {0, 0};
    bool selected = false;


    bool win = true;
    bool help = false;



    while (!WindowShouldClose()) {

        UpdateMusicStream(music);

        if (IsWindowResized()) {
            screenHeight = GetScreenHeight();
            screenWidth = GetScreenWidth();
            cellSize = screenHeight - cellSize < screenWidth ? (screenHeight - cellSize) / 9 : screenWidth / 9;
            gridRect = (Rectangle) {((float) screenWidth - gridRect.width) / 2, ((float) screenHeight - (float) cellSize - gridRect.height) / 2 + (float) cellSize, (float) cellSize * 9, (float) cellSize * 9};

            helpRect.x = (float) screenWidth - 70;

            hSelectedArea.width = gridRect.width;
            hSelectedArea.height = (float) cellSize;
            vSelectedArea.width = (float) cellSize;
            vSelectedArea.height = gridRect.height;
            boxSelectedArea.width = (float) cellSize * 3;
            boxSelectedArea.height = (float) cellSize * 3;

            updateSelectedRec(&vSelectedArea, &hSelectedArea, &boxSelectedArea, selector, cellSize, gridRect);
        }

        if (IsKeyPressed(KEY_P)) {
            musicPlaying ? PauseMusicStream(music) : ResumeMusicStream(music);
            musicPlaying = !musicPlaying;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            const Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, gridRect)) {
                selected = true;

                selector.x = (int) (mousePos.x - gridRect.x) / ( cellSize);
                selector.y = (int) (mousePos.y - gridRect.y) / ( cellSize);

                updateSelectedRec(&vSelectedArea, &hSelectedArea, &boxSelectedArea, selector, cellSize, gridRect);

                debugText(TextFormat("%d %d", selector.x, selector.y));
            }

            if (CheckCollisionPointRec(mousePos, helpRect)) help = !help;

        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            selected = false;
            if (CheckCollisionPointRec(GetMousePosition(), helpRect)) help = !help;
        }


        if (IsKeyPressed(KEY_R)) {
            selected = false;
            lifes = 3;
            sudokuGenerator(_sudoku);
            memcpy(grid, _grid, sizeof(int[N][N]));
            memset(sudokuNotes, 0, sizeof(sudokuNotes));
            removeDigits(sudoku, k);
        }

        if (IsKeyPressed(KEY_C)) {

            for (int y = 0; y < 9; y++) {
                for (int x = 0; x < 9; x++) {
                    if (sudoku[y][x] == 0) {
                        sudokuNotes[y][x] = 0x1FFu;
                    } else {
                        sudokuNotes[y][x] = 0;
                    }
                }
            }

            for (int y = 0; y < 9; y++) {
                for (int x = 0; x < 9; x++) {
                    if (sudoku[y][x] != 0) {
                        int value = sudoku[y][x];
                        removeNotes(sudokuNotes, x, y, value);
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_B)) {
            memset(sudokuNotes, 0, sizeof(sudokuNotes));
        }


        if (IsKeyPressed(KEY_MINUS)) k--;
        if (IsKeyPressed(KEY_EQUAL)) k++;

        if (k < 1) k = 1;
        if (k > 81) k = 81;


        const int step = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) ? 3 : 1;
        if (IsKeyPressed(KEY_UP)) {
            if (selector.y > -1 + step) selector.y -= step;
            updateSelectedRec(&vSelectedArea, &hSelectedArea, &boxSelectedArea, selector, cellSize, gridRect);
            debugText(TextFormat("%d %d", selector.x, selector.y));
        }
        if (IsKeyPressed(KEY_DOWN)) {
            if (selector.y < 9 - step)  selector.y += step;
            updateSelectedRec(&vSelectedArea, &hSelectedArea, &boxSelectedArea, selector, cellSize, gridRect);
            debugText(TextFormat("%d %d", selector.x, selector.y));
        }
        if (IsKeyPressed(KEY_LEFT)) {
            if (selector.x > -1 + step) selector.x -= step;
            updateSelectedRec(&vSelectedArea, &hSelectedArea, &boxSelectedArea, selector, cellSize, gridRect);
            debugText(TextFormat("%d %d", selector.x, selector.y));
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            if (selector.x < 9 - step)  selector.x += step;
            updateSelectedRec(&vSelectedArea, &hSelectedArea, &boxSelectedArea, selector, cellSize, gridRect);
            debugText(TextFormat("%d %d", selector.x, selector.y));
        }

        if (IsKeyPressed(KEY_A)) annotationMode = !annotationMode;

        int keyPressed = GetKeyPressed();
        if (keyPressed >= KEY_ONE && keyPressed <= KEY_NINE && selected) {
            int value = keyPressed - KEY_ONE + 1;
            int row = selector.y;
            int col = selector.x;

            if (sudoku[row][col] == 0) {
                if (!annotationMode) {
                    if (_sudoku[row][col] == value) {

                        sudoku[row][col] = value;

                        removeNotes(sudokuNotes, col, row, value);

                    } else lifes--;
                }
                else {
                    note_toggle(selector.x, selector.y, value);
                }
            }
        }


        BeginDrawing();
        ClearBackground(backgroundColor);


        // selector
        if (selected) {
            DrawRectangleRec(hSelectedArea, selectedAreaColor);
            DrawRectangleRec(vSelectedArea, selectedAreaColor);
            DrawRectangleRec(boxSelectedArea, selectedAreaColor);

            DrawRectangleRec((Rectangle){((float) selector.x + gridRect.x / (float) cellSize) * (float) cellSize, ((float) selector.y + gridRect.y / (float) cellSize) * (float) cellSize, (float) cellSize, (float) cellSize}, selectedColor);
            DrawRectangleLinesEx((Rectangle){((float) selector.x + gridRect.x / (float) cellSize) * (float) cellSize, ((float) selector.y + gridRect.y / (float) cellSize) * (float) cellSize, (float) cellSize, (float) cellSize}, 4, selectedLineColor);
        }


        // numbers
        win = true;
        for (int y = 0; y < 9; y++) {
            for (int x = 0; x < 9; x++) {
                if (sudoku[y][x] != 0) {
                    if (selected && sudoku[y][x] == sudoku[selector.y][selector.x]) {
                        DrawRectangleRec((Rectangle){((float) x + gridRect.x / (float) cellSize) * (float) cellSize, ((float) y + gridRect.y / (float) cellSize) * (float) cellSize, (float) cellSize, (float) cellSize}, selectedColor);
                        DrawRectangleLinesEx((Rectangle){((float) x + gridRect.x / (float) cellSize) * (float) cellSize, ((float) y + gridRect.y / (float) cellSize) * (float) cellSize, (float) cellSize, (float) cellSize}, 4, selectedLineColor);
                    }
                    const Vector2 textPos = {(float)x * (float)cellSize + (float)cellSize/3 + gridRect.x, (float)y * (float)cellSize + (float)cellSize/4 + gridRect.y};
                    DrawTextEx(font, TextFormat("%d", sudoku[y][x]), textPos, (float) cellSize / 2, 0, numberColor);
                }
                else {
                    win = false;

                    // annotation numbers
                    uint16_t mask = sudokuNotes[y][x];
                    while (mask) {
                        int bit = __builtin_ctz(mask);
                        int n = bit + 1;

                        const Vector2 textPos = {(float)x * (float)cellSize + (float) ((n - 1) % 3 + 1) * (float) cellSize / 5 + gridRect.x, (float)y * (float)cellSize + (float) ((n - 1) / 3 + 1) * (float) cellSize / 5 + gridRect.y};
                        DrawTextEx(font, TextFormat("%d", n), textPos, (float) cellSize / 4, 0, sudoku[selector.y][selector.x] == n ? WHITE : numberAnnotationColor);

                        mask &= mask - 1;
                    }
                }
            }
        }


        // lines
        for (int i = 0; i < 10; i++) {
            const Color drawColor = (i) % 3 == 0 ? boxColor : lineColor;
            DrawLine(cellSize * (i) + (int) gridRect.x, (int) gridRect.y,
                cellSize * (i) + (int) gridRect.x, (int) gridRect.height + (int) gridRect.y, drawColor);

            DrawLine((int) gridRect.x, cellSize * (i) + (int) gridRect.y,
                (int) gridRect.width + (int) gridRect.x, cellSize * (i) + (int) gridRect.y, drawColor);
        }


        // lifes
        for (int i = 0; i < 3; i++) DrawTexture(heart, 10 + i * 70, 10, lifes <= i ? GRAY : RED);

        // pen
        if (annotationMode) DrawTexture(pen, 220, 10, WHITE);

        // numbers
        DrawTextEx(font, TextFormat("N = %d", k), (Vector2) {(float) screenWidth - (float) MeasureText(TextFormat("N = %d", k), 60) - 70, 10}, 60, 0, WHITE);

        // help
        if (help) {
            DrawRectangle(0, 0, screenWidth, screenHeight, (Color) {0, 0, 0, 128});
            DrawTextEx(font, "Mouse\nLeft Click -> Select a cell\nRight Click -> Clear selection\n\nNavigation\nArrow Keys -> Move selection\nCtrl + Arrows -> Move faster (3 cells)\n\nNumbers\n1 - 9 -> Place number in selected cell\nA -> Toggle annotation mode\n  Normal mode: place a number\n  Annotation mode: place a number as an annotation\n\nGame Actions\nP -> Play / Pause music\nR -> Restart puzzle\nC -> Automatically calculate all notes\nB -> Clear all notes\n\nDifficulty\n- -> Decrease empty cells\n= -> Increase empty cells", (Vector2) {10, 10}, 32, 0, WHITE);
        }
        DrawTextureV(helpTexture, (Vector2) {helpRect.x, helpRect.y}, WHITE);

        // game conditions
        if (lifes <= 0) DrawTextEx(font, "   PRESS R\nTO RESTART", (Vector2) {  (float) screenWidth / 2 - MeasureTextEx(font, "   PRESS R\nTO RESTART", 60, 0).x / 2 , (float) screenHeight / 2 - 60}, 60, 0, RED);
        if (win) DrawTextEx(font, "YOU WON", (Vector2) { (float) screenWidth / 2 - MeasureTextEx(font, "YOU WON", 100, 0).x / 2 , (float) screenHeight / 2}, 100, 0, GOLD);

        if (debug) DrawText(buffer, 10, 10, 20, DARKGREEN);

        EndDrawing();

    }

    UnloadMusicStream(music);
    UnloadFont(font);
    UnloadTexture(heart);
    UnloadTexture(pen);
    UnloadTexture(helpTexture);
    CloseAudioDevice();
    CloseWindow();

    free(buffer);
    buffer = nullptr;
    return 0;
}