#include "cstring.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

static char sprintf_buffer[C_STRING_SPRINTF_BUF_SIZE];

String createString() {
    String str = {
        .s = calloc(C_STRING_DEFAULT_CAP, sizeof(char)),
        .cap = C_STRING_DEFAULT_CAP,
    };

    if (!str.s) {
        fprintf(stderr, "Calloc Failed: %s %d", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    return str;
}

String createStringExt(const char * s) {
    String str = {
        .cap = 0,
        .s = NULL,
    };
    setString(&str, s);

    return str;
}

String createNewString(String str) {
    freeString(str);
    return createString();
}

String createNewStringExt(String str, const char * s) {
    freeString(str);
    return createStringExt(s);
}

void freeString(String str) {
    free( str.s );
}

void freeStringExt(int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        freeString(va_arg(args, String));
    }

    va_end(args);
}

void setStringCap(String * str, size_t new_cap) {
    void * res = realloc(str->s, new_cap);
    if (!res) {
        fprintf(stderr, "Realloc Failed: %s %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    str->s = res;
    str->cap = new_cap;

}

void setString(String * dest, const char * src) {
    if (!dest || !src) return;

    if (dest->cap < strlen(src) + 1) {
        setStringCap(dest, strlen(src) + 1);
    }

    strcpy(dest->s, src);
}

void appendString(String * dest, const char * src) {
    if (!dest) return;
    
    const size_t src_len = strlen(src);
    if (strlen(dest->s) + src_len + 1 > dest->cap) {
        setStringCap(dest, dest->cap + src_len + 1);
    }

    strcat(dest->s, src);
}

bool insertString(String * dest, const char * src, size_t index) {
    if (!dest || !dest->s || !src || strlen(dest->s) < index) return false;

    const size_t src_len = strlen(src);
    const size_t new_len = src_len + strlen(dest->s) + 1;
    if (dest->cap < new_len) {
        setStringCap(dest, dest->cap + (new_len < C_STRING_DEFAULT_CAP ? C_STRING_DEFAULT_CAP : new_len));
    }

    const size_t bytes_to_move = strlen(dest->s + index) + 1;
    memmove(dest->s + index + src_len, dest->s + index, bytes_to_move);
    for (size_t i = index; i < index + strlen(src); i++) {
        dest->s[i] = src[i - index];
    }

    return true;
}

bool insertStringC(String * dest, char c, size_t index) {
    char buf[2] = { c, '\0' };
    return insertString(dest, buf, index);
}

bool removeString(String * dest, const char * src, bool only_first) {
    if (!dest || !dest->s || !src) return false;
    bool ret = false;

    char * s = NULL;
    while ((s = strstr(dest->s, src))) {
        ret = true;
        size_t start = s - dest->s;
        removeStringExt(dest, start, strlen(src));

        if (only_first) break;
    }

    return ret;
}

bool removeStringExt(String * dest, size_t start, size_t len) {
    if (!dest || !dest->s) return false;

    size_t new_cap = start + 1;
    bool del_from_end = start + len != strlen(dest->s);
    String temp;
    if (del_from_end) {
        temp = createStringExt(dest->s + start + len);
    }

    dest->s = realloc(dest->s, new_cap);
    if (!dest->s) {
        fprintf(stderr, "Realloc Failed: %s %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    dest->cap = new_cap;
    fprintf(stderr, "newStringCap:%ld\n", new_cap);
    fprintf(stderr, ">%s<\n", dest->s);
    dest->s[new_cap - 1] = '\0';

    if (del_from_end) {
        appendString(dest, temp.s);
        freeString(temp);
    }

    return true;
}

bool removeSymbols(String * dest, const char * symbols) {
    bool ret = false;
    for (size_t i = 0; i < strlen(symbols); i++) {
        String symbol = charToString(symbols[i]);
        if (removeString(dest, symbol.s, false)) ret = true;
        freeString(symbol);
    }
    
    return ret;
}

bool replaceString(String * dest, const char * replace, const char * with, bool only_first) {
    if (!dest || !replace || !with ) return false;

    bool ret = false;

    char * needle;
    while ((needle = strstr(dest->s, replace))) {
        if (removeString(dest, replace, true)) ret = true;
        insertString(dest, with, needle - dest->s);

        if (only_first) break;
    }


    return ret;
}

bool containsString(String str, const char * needle) {
    return strstr(str.s, needle);
}

bool stringCompare(String str, String str2) {
    return strcmp(str.s, str2.s) == 0;
}

long findString(String haystack, const char * needle) {
    return findStringExt(haystack, needle, 0);
}

long findStringExt(String haystack, const char * needle, size_t index) {
    if (!needle || strlen(haystack.s) < index) return -1;

    char * find = strstr(haystack.s + index, needle);
    if (!find) return -1;
    return find - haystack.s;
}

size_t stringLen(String str) {
    return strlen(str.s);
}

String c_str_printf(const char * format, ...) {
    
    va_list args;
    va_start(args, format);
    vsprintf(sprintf_buffer, format, args);
    va_end(args);

    return createStringExt(sprintf_buffer);
}

String intToString(int n) {
    return c_str_printf("%d", n);
}

String longToString(long n) {
    return c_str_printf("%ld", n);
}

String size_tToString(size_t n) {
    return c_str_printf("%ld", n);
}

String floatToString(float f) {
    return c_str_printf("%f", f);
}

String doubleToString(double d) {
    return c_str_printf("%d", d);
}

String charToString(char c) {
    return c_str_printf("%c", c);
}
