#include "gfx.h"
#include "rect.h"
#include "bitmap.h"
#include "charmap.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include <unistd.h>


//void hide_tty0_cursor() {
//  FILE *file = fopen ("/dev/tty0", "w");
//  if (file != NULL) {
//    fputs ("\e[?25l", file);
//
//    fclose (file);
//  }
//}
//
//void show_tty0_cursor() {
//  FILE *file = fopen ("/dev/tty0", "w");
//  if (file != NULL) {
//    fputs ("\e[?25h", file);
//
//    fclose (file);
//  }
//}

struct rect center_offset (struct rect to_center, struct point center_on) {
  struct rect result = {
    { center_on.x - ((to_center.br.x - to_center.tl.x) / 2), center_on.y - ((to_center.br.y - to_center.tl.y) / 2) },
    { center_on.x + ((to_center.br.x - to_center.tl.y) / 2), center_on.y + ((to_center.br.y - to_center.tl.y) / 2) }
  };

  return result;
}

int main()
{
  struct screen screen = open_screen ();

  if (screen.buffer == NULL)
    return 1;


  struct charmap_meta meta = charmap_load_all ("res/", "maps.meta");
  const struct charmap *map_0 = charmap_find (meta, '0');
  const struct charmap *map_1 = charmap_find (meta, '1');

  // Virtual rectangle representing character
  struct rect rect_0_virt = RECT(map_0->bitmap.width, map_0->bitmap.height);
  RECT_CENTER(rect_0_virt, screen.dims.width / 2, screen.dims.height / 2);


  // Make a 100x100 rectangle and center it in on the screen
  struct rect window = RECT(100, 100);
  RECT_CENTER(window, screen.dims.width / 2, screen.dims.height / 2);
  

  // Clear screen
  memset (screen.buffer, 0, BUFFER_SIZE(screen));

  // Draw stuff 
  for (int j = 1; j <= 10; ++j) {
    int stretch = (j % 2) - ((j % 2) ^ 1);
    for (int i = 0; i < 200; ++i) {
      draw_rect_b (screen, window, 0x00);
      RECT_STRETCH (window, stretch, stretch);
      draw_rect_b (screen, window, 0xFF);
      draw_bitmap (screen, (stretch - 1 ? map_0 : map_1)->bitmap, rect_0_virt.tl, 0x0000, true);

      screen_commit (screen);
      usleep (5000);
    }
  }

  struct point tl = {100, 100};

  for (int i = 0; i < 10; ++i) {
    draw_bitmap (screen, map_0->bitmap, tl, 0xFFFF, true);
    tl.x += (int)(map_0->bitmap.width * 1.5f);
  }

  screen_commit (screen);


  close_screen (screen);

  return 0;
}
