
#include <cterm.h>

#include <stdbool.h>
#include <stdio.h>

int main() {

    enableRawMode();

    while (1) {
    char c = readTermInput();
    if (isCtrlChar(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q') break;
  }

    disableRawMode();
    return 0;
}
