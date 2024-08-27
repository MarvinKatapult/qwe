#include <qwe.h>

#include <cterm.h>
#include <cvecs.h>
#include <cstring.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ESC         '\x1b'
#define BACKSPACE   127
#define ENTER       '\r'

typedef enum {
    NormalMode = 0,
    InsertMode,
    CommandMode,
} QweState;

typedef struct {
    long x;
    long y;
} EditorCursor;

static QweState qwe_editor_state;
static EditorCursor qwe_editor_cursor;
static StrVec qwe_editor_buf;
static String qwe_cmd_buf;
static String qwe_file_name;
static size_t qwe_buf_start_y;
static size_t qwe_buf_start_x;

void qweMoveCursor(int x, int y);
void qweEnd(void);
void qweMoveCursor(int x, int y);
void qweEventLoop(void);
int qweProcessCmd(const char * str);
void qweSaveBuffer(void);
void qweParseFile(const char * file);

void qweStart(int argc, char * argv[]) {
    enableRawMode();
    qwe_editor_state = NormalMode;
    qwe_editor_cursor = (EditorCursor){ 1, 1 };
    qwe_editor_buf = createStrVec();
    qwe_cmd_buf = createString();
    qwe_buf_start_x = 4;
    appendStrVec(&qwe_editor_buf, "");
    clearScreen();
    qweMoveCursor(qwe_buf_start_x, 1);

    if (argc > 1) {
        qwe_file_name = createStringExt(argv[1]);
        qweParseFile(argv[1]);
    } else {
        qwe_file_name = createStringExt("a.out");
    }

    qweEventLoop();
    qweEnd();
}

void qweEnd() {
    clearScreen();
    putStr("\r");
    disableRawMode();
    freeStrVec(qwe_editor_buf);
    freeString(qwe_cmd_buf);
    freeString(qwe_file_name);
}

void qweMoveCursor(int x, int y) {
    qwe_editor_cursor.x = x;
    qwe_editor_cursor.y = y;
    moveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y);
}

void qweEventLoop() {
    while (true) {

        clearScreen();

        for (int i = qwe_buf_start_y; i < qwe_buf_start_y + getTermHeight() - 1; i++) {
            String line_nr = intToString(i + 1);
            if ((size_t)i >= qwe_editor_buf.count) setString(&line_nr, "~");

            const size_t text_y = i + 1 - qwe_buf_start_y;
            putStrAt(line_nr.s, 0, text_y); 
            putStrAt(qwe_editor_buf.vals[i].s, qwe_buf_start_x, text_y);
            freeString(line_nr);
        }
        switch (qwe_editor_state) {
            case NormalMode:
                putStrAt("Normal", 1, getTermHeight());
                qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y);
                break;
            case InsertMode:
                putStrAt("Insert", 1, getTermHeight());
                qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y);
                break;
            case CommandMode:
                putStrAt(":", 1, getTermHeight());
                putStrAt(qwe_cmd_buf.s, 2, getTermHeight());
                moveCursor(strlen(qwe_cmd_buf.s) + 2, getTermHeight());
                break;
        }

        char c = readTermInput();
        String * current_str = &qwe_editor_buf.vals[qwe_editor_cursor.y-1];

        if (c == ESC) {
            qwe_editor_state = NormalMode;
            continue;
        } 
        if (qwe_editor_state == NormalMode) {
            putStrAt("Normal", 1, getTermHeight());
            if (c == 'q') break;
            if (c == ':') {
                qwe_editor_state = CommandMode;            
                continue;
            }
            if (c == 'i') {
                qwe_editor_state = InsertMode;
                continue;
            } 
            if (c == 'a') {
                qwe_editor_state = InsertMode;
                qwe_editor_cursor.x++;
                continue;
            }
            if (c == '$') {
                qwe_editor_cursor.x = strlen(current_str->s) + qwe_buf_start_x - 1;
                continue;
            }
            if (c == '0') {
                qwe_editor_cursor.x = qwe_buf_start_x;
                continue;
            }

            switch (c) {
                // Moving Left
                case 'h':
                case 'D':
                    if (qwe_editor_cursor.x <= qwe_buf_start_x) break;
                    qweMoveCursor(qwe_editor_cursor.x - 1, qwe_editor_cursor.y);
                    break;
                // Moving Down 
                case 'j':
                case 'B':
                    if (qwe_editor_cursor.y + qwe_buf_start_y > qwe_editor_buf.count - 1) break;
                    if (qwe_editor_cursor.y == getTermHeight() - 1) {
                        qwe_buf_start_y++;
                        break;
                    }
                    qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y + 1);
                    break;
                // Moving Up
                case 'k':
                case 'A':
                    if (qwe_editor_cursor.y - 1 < 1 && qwe_buf_start_y == 0) break;
                    if (qwe_editor_cursor.y == 1) {
                        qwe_buf_start_y--;
                        break;
                    }
                    qweMoveCursor(qwe_editor_cursor.x, qwe_editor_cursor.y - 1);
                    break;
                // Moving Right
                case 'l':
                case 'C':
                    if (qwe_editor_cursor.x >= current_str->s + qwe_buf_start_x - 1) break;
                    qweMoveCursor(qwe_editor_cursor.x + 1, qwe_editor_cursor.y);
                    break;
                case 'o':
                    insertStrVec(&qwe_editor_buf, "", qwe_editor_cursor.y);
                    qweMoveCursor(qwe_buf_start_x, qwe_editor_cursor.y + 1);
                    qwe_editor_state = InsertMode;
                    break;
            }
        } else if (qwe_editor_state == InsertMode) {
            if (isCtrlChar(c)) {
                if (c == ENTER) {
                    insertStrVec(&qwe_editor_buf, current_str->s + (qwe_editor_cursor.x - qwe_buf_start_x), qwe_editor_cursor.y);

                    const int index_of_br = qwe_editor_cursor.x - qwe_buf_start_x;
                    removeStringExt(current_str, index_of_br, strlen(current_str->s + index_of_br));

                    bool appended_last_line = qwe_editor_cursor.y == getTermHeight() - 1;
                    if (appended_last_line) qwe_buf_start_y++;
                    else                    qweMoveCursor(qwe_buf_start_x, qwe_editor_cursor.y + 1);
                    continue;
                }
                if (c == BACKSPACE) {
                    if(qwe_editor_cursor.x > qwe_buf_start_x) {
                        removeStringExt(current_str, qwe_editor_cursor.x - qwe_buf_start_x - 1, 1);
                        qweMoveCursor(qwe_editor_cursor.x - 1, qwe_editor_cursor.y);
                    } 
                    continue;
                }
            }
            // Default Input
            insertStringC(current_str, c, qwe_editor_cursor.x - qwe_buf_start_x + qwe_buf_start_y);
            qweMoveCursor(qwe_editor_cursor.x + 1, qwe_editor_cursor.y);
        } else if (qwe_editor_state == CommandMode) {
            if (c == ENTER) {
                if (!qweProcessCmd(qwe_cmd_buf.s)) break;
                qwe_editor_state = NormalMode;
                continue;
            }
            if (c == BACKSPACE) {
                removeStringExt(&qwe_cmd_buf, strlen(qwe_cmd_buf.s) - 1, 1);
                continue;
            }
            insertStringC(&qwe_cmd_buf, c, strlen(qwe_cmd_buf.s));
        }

    }
}

int qweProcessCmd(const char * str) {

    const int len = strlen(str);
    for (int i = 0; i < len; i++) {
        switch (str[i]) {
            case 'w':
                qweSaveBuffer();
                break;    
            case 'q':
                return false;
        }
    }

    setString(&qwe_cmd_buf, "");
    return true;
}

void qweSaveBuffer() {
    FILE * fp = fopen(qwe_file_name.s, "w");
    if (!fp) {
        fprintf(stderr, "Could not save file\n");
        return;
    }
    for (size_t i = 0; i < qwe_editor_buf.count; i++) {
        fprintf(fp, qwe_editor_buf.vals[i].s);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void qweParseFile(const char * file) {
    FILE * fp = fopen(file, "r");
    char c;
    int count = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            appendStrVec(&qwe_editor_buf, "");
            count++;
            continue;
        }
        
        String * str = &qwe_editor_buf.vals[count];
        insertStringC(str, c, strlen(str->s));
    }
}
