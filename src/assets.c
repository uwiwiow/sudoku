#include "assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 1024

static char assets_base[MAX_PATH_LEN];

static const char* MakePath(const char* name) {
    static char buf[MAX_PATH_LEN];

    const size_t base_len = strlen(assets_base);
    const size_t name_len = strlen(name);

    if (base_len + 1 + name_len + 1 > sizeof(buf)) {
        TraceLog(LOG_ERROR, "Asset path too long: %s/%s", assets_base, name);
        return nullptr;
    }

    int written = snprintf(buf, sizeof(buf), "%s/%s", assets_base, name);

    if (written < 0 || (size_t)written >= sizeof(buf)) {
        TraceLog(LOG_ERROR, "Failed to format asset path: %s/%s", assets_base, name);
        return nullptr;
    }

    return buf;
}


void Assets_Init(void) {
    const char* env = getenv("SUDOKU_SIMPLE_ASSETS");
    if (env && (int)DirectoryExists(env)) {
        strncpy(assets_base, env, sizeof(assets_base));
        return;
    }

    if (DirectoryExists("assets")) {
        strcpy(assets_base, "assets");
        return;
    }

    strcpy(assets_base, "/usr/share/sudoku-simple/assets");
    if (!DirectoryExists(assets_base)) {
        TraceLog(LOG_ERROR, "Assets directory not found: %s", assets_base);
    }
}

Font Assets_LoadFont(const char* name) { return LoadFont(MakePath(name)); }

Texture2D Assets_LoadTexture(const char* name) { return LoadTexture(MakePath(name)); }

Image Assets_LoadImage(const char* name) { return LoadImage(MakePath(name)); }

Music Assets_LoadMusic(const char* name) { return LoadMusicStream(MakePath(name)); }
