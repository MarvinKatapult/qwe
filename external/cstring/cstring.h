/*
    MIT License

    Copyright (c) 2024 Marvin Katapult

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the Software), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef __C_STRING__
#define __C_STRING__

#define C_STRING_DEFAULT_CAP      32
#define C_STRING_SPRINTF_BUF_SIZE 2048

#include <stddef.h>
#include <stdbool.h>

typedef struct String {
    char * s;
    size_t cap;
} String;

/** Creating Strings **/
String createString(void);
String createStringExt(const char * s);
// Frees Memory and Creates New String
String createNewString(String str); 
String createNewStringExt(String str, const char * s); 

/** Memory Manipulation **/
void freeString(String str);
// Frees multiple Strings
void freeStringExt(int count, ...); 

void setStringCap(String * str, size_t new_cap);

/** String Manipulation **/
void setString(String * dest, const char * src);
void appendString(String * dest, const char * src);
// Returns true, if action was completed
bool insertString(String * dest, const char * src, size_t index);
bool removeString(String * dest, const char * src, bool only_first);
bool removeStringExt(String * dest, size_t start, size_t len);
bool removeSymbols(String * dest, const char * symbols);
bool replaceString(String * dest, const char * replace, const char * with, bool only_first);

/** String Utils **/
bool stringCompare(String str, String str2);
bool containsString(String str, const char * needle);
long findString(String haystack, const char * needle);
long findStringExt(String haystack, const char * needle, size_t index);
size_t stringLen(String str);

/** sprintf **/
// Max length for result is C_STRING_SPRINTF_BUF_SIZE
String c_str_printf(const char * format, ...);

/** Conversion **/
String intToString(int n);
String longToString(long n);
String size_tToString(size_t n);
String floatToString(float f);
String doubleToString(double f);
String charToString(char c);

#endif // __C_STRING__
