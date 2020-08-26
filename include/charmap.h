#ifndef CHARMAP_H
#define CHARMAP_H

#include "bitmap.h"

struct charmap {
  char c;
  struct bitmap bitmap;
};

struct charmap_meta {
  const char *root;   // Root directory for maps
  struct charmap *maps;
  size_t map_count;
  char mapped[16];    // Bitmap of mapped characters
};

// Character map operations
struct charmap charmap_load (const char *path, char mapping);
struct charmap charmap_load_readable (const char *path, char mapping);
void charmap_unload (struct charmap map);

// Character map bulk operations
struct charmap_meta charmap_load_all (const char *root, const char *meta);
void charmap_unload_all (struct charmap_meta meta);

const struct charmap * charmap_find (struct charmap_meta meta, char mapping);

#endif
