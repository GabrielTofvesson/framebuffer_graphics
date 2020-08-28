#ifndef GFX_H
#define GFX_H

#include "rect.h"
#include "point.h"
#include "bitmap.h"

#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE(screen) (((screen).dims.width) * ((screen).dims.height) * ((screen).dims.bpp))

#define BLIT_F_DECL(type) void fblit_ ## type (type *pixelBuf, size_t width, struct point start, struct bitmap *content, type color, float scale);
#define BLIT_N_DECL(type) void iblit_ ## type (type *pixelBuf, size_t width, struct point start, struct bitmap *content, type color);
#define BLIT_DECL(type) \
  BLIT_F_DECL(type)\
  BLIT_N_DECL(type)

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


BLIT_DECL(uint8_t)
BLIT_DECL(uint16_t)
BLIT_DECL(uint32_t)
BLIT_DECL(uint64_t)

void draw_pixel (void *pixelBuf, int pixel, int bpp);

void draw_rect (struct screen screen, struct rect rect, int color);

void draw_rect_b (struct screen screen, struct rect rect, int color);

void draw_bitmap (struct screen screen, struct bitmap bitmap, struct point tl, int color, bool transparent_bg);

void screen_commit (struct screen screen);

struct screen open_screen ();

void close_screen (struct screen screen);

#endif
