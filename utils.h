#ifndef UTILS_H
#define UTILS_H

void insert_line(Buffer *buf, int y);
void insert_str(Buffer *buf, int x, int y, char_t *str, char_t str_len);
void delete_range(Line *line, int x, int len);

#endif

#ifdef UTILS_IMPL

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void insert_line(Buffer *buf, int y) {
  assert(y <= buf->num_lines && "Out of bounds");
  assert(y >= 0 && "Out of bounds");

  buf->num_lines += 1;
  if (buf->num_lines > buf->cap_lines) {
    buf->cap_lines = buf->num_lines * 2;
    buf->lines = realloc(buf->lines, sizeof(Line) * buf->cap_lines);
    if (buf->lines == NULL) {
      perror("Could not realloc buf->lines");
      exit(EXIT_FAILURE);
    }
  }

  memmove(
    &buf->lines[y + 1], 
    &buf->lines[y],
    sizeof(Line) * (buf->num_lines - y - 1)
  );

  if (y < buf->num_lines - 1) {
    size_t n = 0;

    for (int i = y + 1; i < buf->num_lines; i++) {
      buf->lines[i].str += 1;
      n += buf->lines[i].cap;
    }

    memmove(
      &buf->lines[y + 1].str[0],
      &buf->lines[y + 1].str[-1],
      sizeof(char_t) * n
    );

    buf->lines[y].str = buf->lines[y + 1].str - 1;
  }
  else {
    buf->lines[y].str = &buf->line_buf[buf->line_buf_len];
  }

  buf->lines[y].len = 1;
  buf->lines[y].cap = 1;

  buf->line_buf_len += 1;
}

void insert_str(Buffer *buf, int x, int y, char_t *str, char_t str_len) {
  Line *line = &buf->lines[y];

  assert(y < buf->num_lines && "Out of bounds");
  assert(y >= 0 && "Out of bounds");
  assert(x <= line->len && "Out of bounds");
  assert(x >= 0 && "Out of bounds");

  line->len += str_len;

  if (line->len > line->cap) {
    size_t diff = line->cap;
    line->cap = line->len * 2;
    diff = line->cap - diff;

    size_t n = 0;

    for (int i = y + 1; i < buf->num_lines; i++) {
      buf->lines[i].str += diff;
      n += buf->lines[i].cap;
    }

    assert(n < buf->line_buf_cap && "TODO"); // TODO: resize line_buf

    memmove(
      &buf->lines[y + 1].str[0],
      &buf->lines[y + 1].str[-diff],
      sizeof(char_t) * n
    );
  }

  memmove(
    &line->str[x + str_len],
    &line->str[x],
    sizeof(char_t) * (line->len - x)
  );
  for (int i = 0; i < str_len; i++) {
    line->str[x + i] = str[i];
  }
}

void delete_range(Line *line, int x, int len) {
  assert(x + len <= line->len && "Out of bounds");

  if (x + len < line->len) {
    memmove(
      &line->str[x],
      &line->str[x + len],
      sizeof(char_t) * (line->len - x - len)
    );
  }
  line->len -= len;
}

#endif
