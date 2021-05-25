/*******************************************************************
  Defines a standard xrgb 32 bit bitmap
  (the most significant byte is unused)
  and operations on such bitmaps

  bitmap.h

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#ifndef BITMAP_H
#define BITMAP_H

#include "font_types.h"

// The xrgb 32-bit per pixel bitmap structure
typedef struct bitmap
{
    int width;
    int height;
    uint32_t *pixels;
} bitmap;

// Allocate a bitmap of specified size
bitmap bitmap_make(int width, int height);

// Free a previouslt allocated bitmap
void bitmap_free(bitmap *b);

// Fill the whole bitmap with a color
void bitmap_fill(bitmap b, uint32_t color);

// Draw an axis aligned rectangle on the bitmap of a certain color
void bitmap_draw_rect(bitmap b, int x0, int y0, int x1, int y1, uint32_t color);

// Draw a string on the bitmap
// Can handle \n, but ignores \r and other control characters
// Stops rendering when \0 is encountered
void bitmap_draw_string(bitmap b, int x, int y, int *out_x, int *out_y, font_descriptor_t *font, int scale, char *string, uint32_t color);

#endif
