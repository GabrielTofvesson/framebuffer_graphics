#include "bitmap.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define BITS_TO_BYTES(bits) (((bits) / 8) + (((bits) & 7) ? 1 : 0))

// Internal write function where integrity of bit is ensured (0 or 1)
static void _write_bit (struct bitmap bitmap, size_t x, size_t y, char bit);


struct bitmap bitmap_load_readable (const char *path) {
  struct bitmap result = {
    NULL,
    0,
    0,
    NULL
  };
  int fd = open (path, O_RDONLY);

  if (fd >= 0) {
    off_t len = lseek (fd, 0, SEEK_END);
    lseek (fd, 0, SEEK_SET);

    char *data = mmap (NULL, len, PROT_READ, MAP_SHARED, fd, 0);

    if (data && data != MAP_FAILED) {
      // Find width of bitmap
      int width = -1;
      for (int i = 0; i < len; ++i)
        if (data[i] == '\n') {
          width = i;
          break;
        }

      // Compute height of bitmap
      int height = (len + (data[len - 1] == '\n' ? 0 : 1)) / (width + 1);

      result = bitmap_parse_readable (data, width, height);

      munmap (data, len);
      close (fd);
    } else {
      perror ("mmap");
      close (fd);
    }
  } else {
    perror ("open");
  }

  return result;
}

struct bitmap bitmap_load_raw (const char *path) {
  struct bitmap result = {
    NULL,
    0,
    0,
    NULL
  };
  int fd = open (path, O_RDONLY);

  if (fd >= 0) {
    off_t len = lseek (fd, 0, SEEK_END);
    lseek (fd, 0, SEEK_SET);

    if (len >= 8) {
      void *data = mmap (NULL, len, PROT_READ, MAP_SHARED, fd, 0);
      if (data && data != MAP_FAILED) {
        memcpy (&result.width, data, 4);
        memcpy (&result.height, data + 4, 4);
        
        size_t reportedLen = BITS_TO_BYTES(result.width * result.height);
        if (reportedLen == len - 8) {
          result.data = malloc (reportedLen);
          memcpy (result.data, data + 8, reportedLen);
        } else {
          fputs ("Reported file size is incorrect", stderr);
        }

        munmap (data, len);
        close (fd);
      } else {
        perror ("mmap");
        close (fd);
      }
    } else {
      fputs ("File is too small", stderr);
      close (fd);
    }
  } else {
    perror ("open");
  }

  return result;
}

struct bitmap bitmap_parse_readable (const char *data, int width, int height) {
  struct bitmap result = {
    NULL,
    0,
    0,
    NULL
  };

  int len = (width + 1) * height;

  // Verify integrity of data
  for (int i = 0; i < len; ++i) {
    int cindex = (i + 1) % (width + 1);
    if ((cindex && data[i] != '0' && data[i] != '1') || (!cindex && data[i] != '\n')) {
      perror ("integrity");
      return result;
    }
  }

  int contentLen = BITS_TO_BYTES(width * height);
  result.data = malloc (contentLen);
  if (contentLen > 0)
    ((char *)result.data)[contentLen - 1] = 0; // Zero-out possible stray/unused bits
  result.width = width;
  result.height = height;

  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      _write_bit (result, x, y, data[x + (y * (width + 1))] - '0');

  return result;
}

void bitmap_save_readable (const char *path, struct bitmap bitmap) {
  int fd = open (path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

  if (fd >= 0) {
    char nl = '\n';
    for (int y = 0; y < bitmap.height; ++y) {
      for (int x = 0; x < bitmap.width; ++x) {
        char bit = '0' + bitmap_read_bit (bitmap, x, y);
        write (fd, &bit, 1);
      }
      write (fd, &nl, 1);
    }

    close (fd);
  } else {
    perror ("open");
  }
}

void bitmap_save_raw (const char *path, struct bitmap bitmap) {
  int fd = open (path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

  if (fd >= 0) {
    size_t len = BITS_TO_BYTES(bitmap.width * bitmap.height);
    write (fd, (const char*)&bitmap.width, 4);
    write (fd, (const char*)&bitmap.height, 4);
    write (fd, bitmap.data, len);

    close (fd);
  } else {
    perror ("open");
  }
}


void bitmap_compress (const char *readable_path, const char *raw_path) {
  struct bitmap bmp = bitmap_load_readable (readable_path);
  if (bmp.data) {
    bitmap_save_raw (raw_path, bmp);
    bitmap_discard (bmp);
  }
}

void bitmap_decompress (const char *raw_path, const char *readable_path) {
  struct bitmap bmp = bitmap_load_raw (raw_path);
  if (bmp.data) {
    bitmap_save_readable (readable_path, bmp);
    bitmap_discard (bmp);
  }
}

char bitmap_read_bit (struct bitmap bitmap, size_t x, size_t y) {
  int byteIdx = (x + (y * bitmap.width)) / 8;
  int bitIdx = (x + (y * bitmap.width)) % 8;
  return ((((char*)bitmap.data)[byteIdx]) >> bitIdx) & 1;
}

static void _write_bit (struct bitmap bitmap, size_t x, size_t y, char bit) {
  int byteIdx = (x + (y * bitmap.width)) / 8;
  int bitIdx = (x + (y * bitmap.width)) % 8;
  ((char *)bitmap.data)[byteIdx] = (((char*)bitmap.data)[byteIdx] & ((1 << bitIdx) ^ 0xFF)) | (bit << bitIdx);
}

void bitmap_write_bit (struct bitmap bitmap, size_t x, size_t y, char bit) {
  _write_bit (bitmap, x, y, bit ? 1 : 0);
}

void bitmap_discard (struct bitmap bitmap) {
  free (bitmap.data);
  bitmap.data = NULL;

  if (bitmap.cache) {
    blitmap_discard (*bitmap.cache);
    free (bitmap.cache);
    bitmap.cache = NULL;
  }
}

void bitmap_load_blitmap (struct bitmap *bitmap, int bpp) {
  // Check if an existing blit-map can be reused
  if (bitmap->cache) {
    if (bitmap->cache->bpp == bpp)
      return;

    // Discard old cached blit map an generate a new one
    blitmap_discard (*bitmap->cache);
  }

  bitmap->cache = malloc (sizeof (struct blitmap));
  bitmap->cache->data = malloc (bitmap->width * bitmap->height * bpp);
  bitmap->cache->bpp = bpp;

  for (int y = 0; y < bitmap->height; ++y)
    for (int x = 0; x < bitmap->width; ++x)
      memset (
          bitmap->cache->data + (x + y * bitmap->width) * bpp,
          bitmap_read_bit(*bitmap, x, y) ? 0x00 : 0xFF,
          bpp
      );
}

void blitmap_discard (struct blitmap blitmap) {
  free (blitmap.data);
  blitmap.data = NULL;
}

struct rect bitmap_to_rect (const struct bitmap bitmap, float scale) {
  struct rect result = RECT((int)(bitmap.width * scale), (int)(bitmap.height * scale));
  return result;
}
