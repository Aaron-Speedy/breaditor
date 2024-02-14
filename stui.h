
// See end of file for licensing

#ifndef STUI_H
#define STUI_H

void st_set_cursor(int x, int y);
void st_show_cursor();
void st_hide_cursor();
void st_clear();
void st_init(void);
void st_cleanup(void);
unsigned short st_width();
unsigned short st_height();

#endif

#ifdef ST_IMPL

#include <sys/ioctl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>

struct termios st_initial_termios;
struct termios st_current_termios;
struct winsize st_winsize;

void st_set_cursor(int x, int y) {
  printf("\x1b[%d;%dH", y + 1, x + 1);
}

void st_show_cursor() {
  printf("\x1b[?25h");
}

void st_hide_cursor() {
  printf("\x1b[?25l");
}

void st_clear() {
  printf("\x1b[2J");
  st_set_cursor(0, 0);
}

void st_init(void) {
  setvbuf(stdout, NULL, _IONBF, 0); // Turn off buffering

  printf("\x1b[?1049h"); // Switch to alternate buffer
  st_clear();
  st_set_cursor(0, 0);

  if (tcgetattr(1, &st_current_termios)) {
    perror("tcgetattr"); 
    exit(EXIT_FAILURE);
  }

  st_initial_termios = st_current_termios;
  st_current_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | IXON);
  st_current_termios.c_cflag |=  (CS8);
  st_current_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG | INPCK);
  st_current_termios.c_cc[VMIN] = 1;
  st_current_termios.c_cc[VTIME] = 1;

  if (tcsetattr(1, TCSANOW, &st_current_termios)) {
    perror("tcsetattr");
    exit(EXIT_FAILURE);
  }

  ioctl(1, TIOCGWINSZ, &st_winsize);
}

void st_cleanup(void) {
  st_show_cursor();
  printf("\x1b[?1049h");
  st_clear();
  printf("\x1b[?1049l");
  tcsetattr(1, TCSANOW, &st_initial_termios);
}

unsigned short st_width() {
  return st_winsize.ws_col;
}
unsigned short st_height() {
  return st_winsize.ws_row;
}

#endif

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License

Copyright (c) 2023 Aaron Speedy

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (Unlicense)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/>
*/
