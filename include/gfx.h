#ifndef GFX_H
#define GFX_H

#include "rect.h"
#include "point.h"
#include "bitmap.h"

#include <stdbool.h>

#define BUFFER_SIZE(screen) (((screen).dims.width) * ((screen).dims.height) * ((screen).dims.bpp))

struct screen_dims {
  int width;
  int height;
  int bpp;
};

struct screen {
  struct screen_dims dims;
  void *buffer;             // Double-buffer
  void *realBuffer;         // Actual framebuffer
  int buflen;
  int fd;
};

struct triangle {
  struct point p1;
  struct point p2;
  struct point p3;
};

void draw_pixel (void *pixelBuf, int pixel, int bpp);

void draw_rect (struct screen screen, struct rect rect, int color);

void draw_rect_b (struct screen screen, struct rect rect, int color);

void draw_bitmap (struct screen screen, struct bitmap bitmap, struct point tl, int color, bool transparent_bg);

void screen_commit (struct screen screen);

struct screen open_screen ();

void close_screen (struct screen screen);

#endif
