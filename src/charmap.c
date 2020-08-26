#include "charmap.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


// simple strcat with allocation
static char *to_path_full (
    const char *root,
    size_t r_off,
    size_t r_len,
    const char *file_name,
    size_t f_off,
    size_t f_len
) {
  // Allocate string
  char *path = malloc (r_len + f_len + 1);

  // Copy substrings
  memcpy (path, root + r_off, r_len);
  memcpy (path + r_len, file_name + f_off, f_len);

  // Null-terminate
  path[r_len + f_len] = 0;

  return path;
}

static char * to_path (const char *root, const char *file_name) {
  return to_path_full (root, 0, strlen(root), file_name, 0, strlen(file_name));
}

static int index_of (const char *str, char find, int offset, int len) {
  for (int i = offset; i < len; ++i)
    if (str[i] == find)
      return i;
  return -1;
}

struct charmap charmap_load (const char *path, char mapping) {
  struct charmap result = {
    mapping,
    bitmap_load_raw (path)
  };

  return result;
}

struct charmap charmap_load_readable (const char *path, char mapping) {
  struct charmap result = {
    mapping,
    bitmap_load_readable (path)
  };

  return result;
}

void charmap_unload (struct charmap map) {
  bitmap_discard (map.bitmap);
}


struct charmap_meta charmap_load_all (const char *root, const char *meta_file) {
  struct charmap_meta meta;
  meta.root = root;
  meta.maps = NULL;
  meta.map_count = 0;
  memset (meta.mapped, 0, 16);

  char *meta_path = to_path (root, meta_file);

  int fd = open (meta_path, O_RDONLY);

  // Path string is no longer needed
  free (meta_path);

  if (fd >= 0) {
    off_t len = lseek (fd, 0, SEEK_END);
    lseek (fd, 0, SEEK_SET);

    char *data = mmap (NULL, len, PROT_READ, MAP_SHARED, fd, 0);
    if (data && data != MAP_FAILED) {
      for (int i = 0; i < len; ++i) {
        char mapping = data[i];
        int nl = index_of (data, '\n', i + 1, len - 1);
        if (nl == -1)
          nl = len - 1;
        char *map_path = to_path_full (root, 0, strlen(root), data, i + 1, nl - i - 1);
        printf ("Opening: '%s'\n", map_path);
        struct charmap map = charmap_load (map_path, mapping);
        free (map_path);
        if (map.bitmap.data) {
          ++meta.map_count;
          struct charmap *new_maps = realloc (meta.maps, sizeof(struct charmap) * meta.map_count);

          if (new_maps) {
            meta.maps = new_maps;
            memcpy (&new_maps[meta.map_count - 1], &map, sizeof(struct charmap));

            // Mark character as mapped
            meta.mapped[mapping >> 3] |= 1 << (mapping & 7);
          } else {
            --meta.map_count;
            perror ("realloc");
          }
        }

        i = nl;
      }
    } else {
      perror ("mmap");
    }
  } else {
    perror ("open");
  }
  return meta;
}

const struct charmap * charmap_find (struct charmap_meta meta, char mapping) {
  if ((meta.mapped[mapping >> 3] >> (mapping & 7)) & 1) {
    for (int i = 0; i < meta.map_count; ++i)
      if (meta.maps[i].c == mapping)
        return &meta.maps[i];
  }
  return NULL;
}
