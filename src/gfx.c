#include "gfx.h"

#include <string.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/fb.h>

#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

int to_index (int x, int y, struct screen_dims dims) {
  return (x * dims.bpp) + (y * dims.width * dims.bpp);
}

void draw_pixel (void *pixelBuf, int pixel, int bpp) {
  memcpy (pixelBuf, &pixel, bpp);
}

void draw_rect (struct screen screen, struct rect rect, int color) {
  for (int y = rect.tl.y; y < rect.br.y; ++y)
    for (int x = rect.tl.x; x < rect.br.x; ++x)
      draw_pixel (screen.buffer + to_index (x, y, screen.dims), color, screen.dims.bpp);
}

void draw_rect_b (struct screen screen, struct rect rect, int color) {
  for (int y = rect.tl.y; y < rect.br.y; ++y)
    memset (screen.buffer + to_index (rect.tl.x, y, screen.dims), color, (rect.br.x - rect.tl.x) * screen.dims.bpp);
}

void draw_bitmap (struct screen screen, struct bitmap bitmap, struct point tl, int color, bool transparent_bg) {

  for (int y = 0; y < bitmap.height; ++y)
    for (int x = 0; x < bitmap.width; ++x) {
      char bit = bitmap_read_bit (bitmap, x, y);
      if (bit)
        memcpy (screen.buffer + to_index (x + tl.x, y + tl.y, screen.dims), &color, screen.dims.bpp);
      else if (!transparent_bg)
        memset (screen.buffer + to_index (x + tl.x, y + tl.y, screen.dims), 0, screen.dims.bpp);
    }
}

struct screen open_screen () {
  struct fb_var_screeninfo screen_info;
  struct fb_fix_screeninfo fixed_info;
  char *buffer = NULL;
  size_t buflen;
  int fd = -1;

  fd = open("/dev/fb0", O_RDWR);
  if (fd >= 0)
  {
    if (!ioctl(fd, FBIOGET_VSCREENINFO, &screen_info) &&
        !ioctl(fd, FBIOGET_FSCREENINFO, &fixed_info))
    {
      buflen = screen_info.yres_virtual * fixed_info.line_length;
      buffer = mmap(NULL,
                    buflen,
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    0);
      if (buffer != MAP_FAILED)
      {
        void *doubleBuffer = mmap (NULL,
                                  buflen,
                                  PROT_READ|PROT_WRITE,
                                  MAP_PRIVATE|MAP_ANONYMOUS,
                                  -1,
                                  0);

        if (doubleBuffer != MAP_FAILED) {
          int bytesPerPixel = screen_info.bits_per_pixel / 8;

          struct screen_dims dims = {
            fixed_info.line_length / bytesPerPixel,
            screen_info.yres_virtual,
            bytesPerPixel
          };


          struct screen result = {
            dims,
            doubleBuffer,
            buffer,
            buflen,
            fd
          };

          return result;
        }
        else {
          perror ("mmap dbuf");
        }

        munmap (buffer, buflen);
      }
      else
      {
         perror("mmap");
      }
    }
    else
    {
       perror("ioctl");
    }

    close (fd);
  }
  else
  {
     perror("open");
  }

  struct screen scr = {
    { 0, 0, 0 },
    NULL,
    NULL,
    0,
    -1
  };

  return scr;
}

void screen_commit (struct screen screen) {
  memcpy (screen.realBuffer, screen.buffer, screen.buflen);
}

void close_screen (struct screen screen) {
  if (screen.buffer && screen.buffer != MAP_FAILED)
    munmap (
      screen.buffer,
      screen.buflen
    );

  if (screen.realBuffer && screen.realBuffer != MAP_FAILED)
    munmap (
      screen.realBuffer,
      screen.buflen
    );

  if (screen.fd >= 0)
    close (screen.fd);
}
