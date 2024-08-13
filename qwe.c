#include <qwe.h>

#include <cterm.h>
#include <cvecs.h>
#include <cstring.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#define ESC 27

#define X_COORD_BUFFER_START    4

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
static StrVec qwe_editor_buf;

void qweStart() {
    enableRawMode();
    qwe_editor_state = NormalMode;
    qwe_editor_cursor = (EditorCursor){ 0, 0 };
    qwe_editor_buf = createStrVec();
    appendStrVec(&qwe_editor_buf, "Hello World");
    appendStrVec(&qwe_editor_buf, "Hello World");
    appendStrVec(&qwe_editor_buf, "Hello World");

    qweEventLoop();
    qweEnd();
}

void qweEnd() {
    clearScreen();
    disableRawMode();
}

void qweEventLoop() {
    while (true) {

        clearScreen();
        for (int i = 0; i < getTermHeight(); i++) {
            String line_nr = intToString(i + 1);
            if ((size_t)i >= qwe_editor_buf.count) setString(&line_nr, "~");
            putStrAt(line_nr.s, 0, i + 1); 
            putStrAt(qwe_editor_buf.vals[i], X_COORD_BUFFER_START, i + 1);
            freeString(line_nr);
        }
        putStrAt(qwe_editor_state == NormalMode ? "Normal" : "Insert", 0, getTermHeight());
        moveCursor(X_COORD_BUFFER_START, qwe_editor_cursor.y);

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

        // putStrAt(qwe_editor_buf.vals, qwe_editor_cursor.x, qwe_editor_cursor.y);
        
    }
}
