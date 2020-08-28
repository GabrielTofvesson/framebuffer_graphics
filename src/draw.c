#include "gfx.h"
#include "rect.h"
#include "bitmap.h"
#include "charmap.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include <unistd.h>


int main()
{
  struct screen screen = open_screen ();

  if (screen.buffer == NULL)
    return 1;


  struct charmap_meta meta = charmap_load_all ("res/", "maps.meta");
  struct charmap *map_0 = charmap_find (meta, '0');
  struct charmap *map_1 = charmap_find (meta, '1');

  // Virtual rectangle representing character
  struct rect rect_0_virt = RECT(map_0->bitmap.width, map_0->bitmap.height);
  RECT_CENTER(rect_0_virt, screen.dims.width / 2, screen.dims.height / 2, 2);

  struct rect rect_1_virt = RECT(map_1->bitmap.width, map_1->bitmap.height);
  RECT_CENTER(rect_1_virt, screen.dims.width / 2, screen.dims.height / 2, 2);


  // Make a 100x100 rectangle and center it in on the screen
  struct rect window = RECT(100, 100);
  RECT_CENTER(window, screen.dims.width / 2, screen.dims.height / 2, 1);
  

  // Clear screen
  memset (screen.buffer, 0, BUFFER_SIZE(screen));

  // Draw stuff 
  for (int j = 1; j <= 10; ++j) {
    int stretch = (j % 2) - ((j % 2) ^ 1);
    for (int i = 0; i < 200; ++i) {
      draw_rect_b (screen, window, 0x00);
      RECT_STRETCH (window, stretch, stretch);
      draw_rect_b (screen, window, 0xFF);

      fblit_uint16_t (screen.buffer, screen.dims.width, (stretch - 1 ? rect_0_virt : rect_1_virt).tl, &(stretch - 1 ? map_0 : map_1)->bitmap, 0x0000, 2.0f);
      //draw_bitmap (screen, (stretch - 1 ? map_0 : map_1)->bitmap, rect_0_virt.tl, 0x0000, true);

      screen_commit (screen);
      //usleep (5000);
    }
  }

  float scale = 1.0f;
  struct point previous_end = {100, 100};

  for (int i = 0; i < 10; ++i) {
    struct rect bounds = bitmap_to_rect (map_0->bitmap, scale);

    bounds.tl.x += 100 + previous_end.x;
    bounds.tl.y += 100 - (int)(map_0->bitmap.height * scale / 2.0f);

    previous_end.x += map_0->bitmap.width * scale + 5;

    fblit_uint16_t (screen.buffer, screen.dims.width, bounds.tl, &map_0->bitmap, 0xFFFF, scale);
    scale += 0.5f;
  }

  screen_commit (screen);


  close_screen (screen);

  return 0;
}
