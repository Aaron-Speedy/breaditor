#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

typedef uint32_t char_t;

typedef struct {
  char_t *str;
  size_t len;
  size_t cap;
} Line;

typedef struct {
  Line *lines;
  size_t num_lines;
  size_t cap_lines;

  char_t *line_buf;
  size_t line_buf_len;
  size_t line_buf_cap;
} Buffer;

#define UTILS_IMPL
#include "utils.h"

int main(void) {
  Buffer buf = {
    .num_lines = 0,
    .cap_lines = 1,
    .line_buf_len = 0,
    .line_buf_cap = 1000,
  };
  buf.lines = malloc(sizeof(Line) * buf.cap_lines);
  buf.line_buf = malloc(sizeof(char_t) * buf.line_buf_cap);

  for (int i = 0; i < 26; i++) {
    insert_line(&buf, i);
    buf.lines[0].str[i] = 'A' + i;
  }

  char_t str[] = { 'H', 'e', 'l', 'l', 'o' };
  insert_str(&buf, 0, 0, str, 5);

  for (int i = 0; i < buf.num_lines; i++) {
    write(1, buf.lines[i].str, sizeof(char_t) * buf.lines[i].len);
    printf("\n");
  }

  return 0;
}
