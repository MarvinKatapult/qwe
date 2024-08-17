#include <qwe.h>

#include <cterm.h>
#include <cvecs.h>
#include <cstring.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#define ESC         '\x1b'
#define BACKSPACE   127
#define ENTER       '\n'

#define X_COORD_BUFFER_START    4

typedef enum {
    NormalMode = 0,
    InsertMode,
} QweState;

typedef struct {
    long x;
    long y;
} EditorCursor;

static QweState qwe_editor_state;
static EditorCursor qwe_editor_cursor;
static StrVec qwe_editor_buf;

void qweMoveCursor(int x, int y);

void qweStart() {
    enableRawMode();
    qwe_editor_state = NormalMode;
    qwe_editor_cursor = (EditorCursor){ 1, 1 };
    qwe_editor_buf = createStrVec();
    appendStrVec(&qwe_editor_buf, "Hello World");
    appendStrVec(&qwe_editor_buf, "Hello World");
    appendStrVec(&qwe_editor_buf, "Hello World");
    clearScreen();
    qweMoveCursor(X_COORD_BUFFER_START, 1);

    qweEventLoop();
    qweEnd();
}

void qweEnd() {
    clearScreen();
    disableRawMode();
    freeStrVec(qwe_editor_buf);
}

void qweMoveCursor(int x, int y) {
    qwe_editor_cursor.x = x;
    qwe_editor_cursor.y = y;
    moveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y);
}

void qweEventLoop() {
    while (true) {

        for (int i = 0; i < getTermHeight(); i++) {
            String line_nr = intToString(i + 1);
            if ((size_t)i >= qwe_editor_buf.count) setString(&line_nr, "~");
            putStrAt(line_nr.s, 0, i + 1); 
            putStrAt(qwe_editor_buf.vals[i].s, X_COORD_BUFFER_START, i + 1);
            freeString(line_nr);
        }
        putStrAt(qwe_editor_state == NormalMode ? "Normal" : "Insert", 1, getTermHeight());
        qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y);

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
            switch (c) {
                case 'h':
                case 'D':
                    if (qwe_editor_cursor.x <= X_COORD_BUFFER_START) break;
                    qweMoveCursor(qwe_editor_cursor.x - 1, qwe_editor_cursor.y);
                    break;
                case 'j':
                case 'B':
                    if (qwe_editor_cursor.y >= getTermHeight() - 1) break;
                    qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y + 1);
                    break;
                case 'k':
                case 'A':
                    if (qwe_editor_cursor.y - 1 < 1) break;
                    qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y - 1);
                    break;
                case 'l':
                case 'C':
                    if (qwe_editor_cursor.x >= getTermWidth() - 1) break;
                    qweMoveCursor(qwe_editor_cursor.x + 1, qwe_editor_cursor.y);
                    break;
            }
        } else if (qwe_editor_state == InsertMode) {
            if (isCtrlChar(c)) {
                switch (c) {
                    case 'D':
                        if (qwe_editor_cursor.x <= X_COORD_BUFFER_START) break;
                        qweMoveCursor(qwe_editor_cursor.x - 1, qwe_editor_cursor.y);
                        break;
                    case 'B':
                        if (qwe_editor_cursor.y >= getTermHeight() - 1) break;
                        qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y + 1);
                        break;
                    case 'A':
                        if (qwe_editor_cursor.y - 1 < 1) break;
                        qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y - 1);
                        break;
                    case 'C':
                        if (qwe_editor_cursor.x >= getTermWidth() - 1) break;
                        qweMoveCursor(qwe_editor_cursor.x + 1, qwe_editor_cursor.y);
                        break;
                }
                if (c == BACKSPACE) {
                    if(qwe_editor_cursor.x > X_COORD_BUFFER_START) {
                        removeStringExt(&qwe_editor_buf.vals[qwe_editor_cursor.y - 1], qwe_editor_cursor.x - X_COORD_BUFFER_START - 1, 1);
                        qweMoveCursor(qwe_editor_cursor.x - 1, qwe_editor_cursor.y);
                        clearScreen();
                    }
                    continue;
                }
                if (c == ENTER) {
                                    
                }
            }
            // Default Input
            insertStringC(&qwe_editor_buf.vals[qwe_editor_cursor.y - 1], c, qwe_editor_cursor.x - X_COORD_BUFFER_START);
            qweMoveCursor(qwe_editor_cursor.x + 1, qwe_editor_cursor.y);
        }

    }
}
