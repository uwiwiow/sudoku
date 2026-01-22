#include "debug.h"

#include <string.h>

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
