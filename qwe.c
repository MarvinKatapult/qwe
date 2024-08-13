#include <qwe.h>

#include <cterm.h>
#include <cstring.h>

#include <stdbool.h>
#include <stdio.h>

#define ESC 27

typedef enum {
    NormalMode = 0,
    InsertMode,
} QweState;

typedef struct {
    unsigned long x;
    unsigned long y;
} EditorCursor;

static QweState qwe_editor_state;
static EditorCursor qwe_editor_cursor;
static String qwe_editor_buf;

void qweStart() {
    enableRawMode();
    qwe_editor_state = NormalMode;
    qwe_editor_cursor = (EditorCursor){ 0, 0 };
    qwe_editor_buf = createStringExt("Hello World");

    qweEventLoop();
    disableRawMode();
}

void qweEventLoop() {
    while (true) {

        clearScreen();
        putStrAt(qwe_editor_state == NormalMode ? "Normal" : "Insert", 0, getTermHeight() - 1);
        moveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y);

        char c = readTermInput();

        if (c == ESC) {
            qwe_editor_state = NormalMode;
            continue;
        } else if (qwe_editor_state != InsertMode && c == 'i') {
            qwe_editor_state = InsertMode;
            continue;
        }

        if (qwe_editor_state == NormalMode) {
            if (c == 'q') break;
        } 

        putStrAt(qwe_editor_buf.s, qwe_editor_cursor.x, qwe_editor_cursor.y);
        
    }
}
