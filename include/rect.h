#ifndef RECT_H
#define RECT_H

#include "point.h"

#define RECT_WIDTH(r) (((r).br.x) - (r).tl.x)
#define RECT_HEIGHT(r) (((r).br.y) - (r).tl.y)
#define RECT_AREA(r) (((r).br.x - (r).tl.x) * ((r).br.y - (r).tl.y))
#define RECT_OFFSET(r, _x, _y) {\
  (r).tl.x += (_x);\
  (r).br.x += (_x);\
  (r).tl.y += (_y);\
  (r).br.y += (_y);\
}
#define RECT_ALIGN_TL(r, _x, _y) {\
  int _w = (_x);\
  int _h = (_y);\
  (r).br.x += (_w) - (r).tl.x;\
  (r).br.y += (_h) - (r).tl.y;\
  (r).tl.x = (_w);\
  (r).tl.y = (_h);\
}
#define RECT_ALIGN_BR(r, _x, _y) {\
  int _w = (_x);\
  int _h = (_y);\
  (r).tl.x += (_w) - (r).br.x;\
  (r).tl.y += (_h) - (r).br.y;\
  (r).br.x = (_w);\
  (r).br.y = (_h);\
}
//#define RECT_CENTER(r, _x, _y) {\
//  int _w = RECT_WIDTH((r));\
//  int _h = RECT_HEIGHT((r));\
//  RECT_ALIGN_TL((r), (_x) - (_w / 2), (_y) - (_h / 2));\
//}
#define RECT_CENTER(r, _x, _y) RECT_ALIGN_TL((r), (_x) - (RECT_WIDTH((r)) / 2), (_y) - (RECT_HEIGHT((r)) / 2))
#define RECT_STRETCH(r, _x, _y) {\
  (r).br.x += (_x);\
  (r).tl.x -= (_x);\
  (r).br.y += (_x);\
  (r).tl.y -= (_x);\
}
#define RECT(w, h) { {0, 0}, {w ,h} }

struct rect {
  struct point tl;
  struct point br;
};

#endif
