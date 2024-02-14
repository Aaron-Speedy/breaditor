#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define ST_IMPL
#include "stui.h"

typedef uint32_t char_t;

typedef struct {
  char_t *str;
  size_t len;
  size_t cap;
} Line;

typedef struct {
  int x, y;
  int sx;
} Cursor;

typedef struct {
  Line *lines;
  size_t num_lines;
  size_t cap_lines;

  char_t *line_buf;
  size_t line_buf_len;
  size_t line_buf_cap;

  int bound_x, bound_y; // Rightmost x and top y

  Cursor cur;

  enum {
    MODE_NORMAL = 0,
    MODE_INSERT,
  } mode;
} Buffer;

#define UTILS_IMPL
#include "utils.h"

int main(void) {
  Buffer buf = {
    .num_lines = 0,
    .cap_lines = 1,
    .line_buf_len = 0,
    .line_buf_cap = 1000000000, // TODO
    .bound_x = 0,
    .bound_y = 0,
    .mode = 0,
    .cur = { 0, 0 },
  };
  buf.lines = malloc(sizeof(Line) * buf.cap_lines);
  buf.line_buf = malloc(sizeof(char_t) * buf.line_buf_cap);

  for (int i = 0; i < 100; i++) {
    insert_line(&buf, 0);
    char_t str = (i % 26) + 'A';
    insert_str(&buf, 0, 0, &str, 1);
  }

  st_init();

  while (true) {
    /* ===== Drawing ===== */

    st_clear();

    int height = min(buf.num_lines, st_height());

    int max_digits = floor(log10(buf.num_lines + 1));
    for (int y = buf.bound_y; y < buf.bound_y + height; y++) {
      int width = min(st_width(), buf.lines[y].len);

      int num_digits = max_digits - floor(log10(y + 1));
      printf("\n");
      printf("\x1b[38;5;244m"); // TODO: Add colors to stui.h
      for (int i = 0; i < num_digits; i++) {
        printf(" ");
      }
      printf("%d│", y + 1);

      printf("\x1b[38;5;255m");

      for (int x = buf.bound_x; x < buf.bound_x + width; x++) {
        write(1, &buf.lines[y].str[x], 1); // TODO: magic number
      }
    }
    st_set_cursor(buf.cur.x + max_digits + 2 - buf.bound_x, buf.cur.y - buf.bound_y);

    /* ==== Input ==== */
    {
      Line *line = &buf.lines[buf.cur.y];
      char_t input; // TODO: unicode
      read(0, &input, 1); // TODO: magic number
      switch (buf.mode) {
      case MODE_NORMAL: {
        switch (input) {
        case 'q': goto exit; break;
        case 'h': {
          if (buf.cur.x > 0) {
            dec_cur_x(line, &buf.cur);
          }
        } break;
        case 'j': {
          if (buf.cur.y < buf.num_lines - 1) {
            if (buf.cur.y + 1 == buf.bound_y + height) {
              buf.bound_y += 1;
            }
            inc_cur_y(&buf, &buf.cur);
          }
        } break;
        case 'k': {
          if (buf.cur.y > 0) {
            if (buf.cur.y == buf.bound_y) {
              buf.bound_y -= 1;
            }
            dec_cur_y(&buf, &buf.cur);
          }
        } break;
        case 'l': {
          if (buf.cur.x < line->len) {
            inc_cur_x(line, &buf.cur);
          }
        } break;
        case 'd': {
          delete_range(line, buf.cur.x, 1);
        } break;
        case 'o': {
          insert_line(&buf, buf.cur.y + 1);
          inc_cur_y(&buf, &buf.cur);
          // could putting this into a function be used to implement hooks?
          buf.mode = MODE_INSERT;
        }
        case 'i': buf.mode = MODE_INSERT; break;
        }
      } break;
      case MODE_INSERT: {
        if (input >= ' ' && input <= '~') { // TODO: unicode
          insert_str(&buf, buf.cur.x, buf.cur.y, &input, 1);
          inc_cur_x(line, &buf.cur);
        }
        if (input == '\x1b') buf.mode = 0;
      } break;
      default:
        assert(!"Unimplemented");
      }
    }
  }

exit:
  st_cleanup();
  return 0;
}
