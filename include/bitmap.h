#ifndef BITMAP_H
#define BITMAP_H

#include "rect.h"

#include <stddef.h>

struct bitmap {
  void *data;
  size_t width;
  size_t height;
  struct blitmap *cache;
};

struct blitmap {
  void *data;
  int bpp;
};

struct bitmap bitmap_load_readable (const char *path);
struct bitmap bitmap_load_raw (const char *path);
struct bitmap bitmap_parse_readable (const char *data, int width, int height);
void bitmap_save_readable (const char *path, struct bitmap bitmap);
void bitmap_save_raw (const char *path, struct bitmap bitmap);

void bitmap_compress (const char *readable_path, const char *raw_path);
void bitmap_decompress (const char *raw_path, const char *readable_path);

char bitmap_read_bit (struct bitmap bitmap, size_t x, size_t y);
void bitmap_write_bit (struct bitmap bitmap, size_t x, size_t y, char bit);

void bitmap_discard (struct bitmap bitmap);

void bitmap_load_blitmap (struct bitmap *bitmap, int bpp);
void blitmap_discard (struct blitmap blitmap);

struct rect bitmap_to_rect (const struct bitmap bitmap, float scale);

#endif
