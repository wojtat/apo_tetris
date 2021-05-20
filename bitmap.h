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

typedef struct bitmap
{
    int width;
    int height;
    uint32_t *pixels;
} bitmap;

bitmap make_bitmap(int width, int height);

void free_bitmap(bitmap *b);

void fill_bitmap(bitmap b, uint32_t color);

void draw_rect(bitmap b, int x0, int y0, int x1, int y1, uint32_t color);

void get_string_size(font_descriptor_t *font, int scale, char *string, int *out_width, int *out_height);

void draw_string(bitmap b, int x, int y, int *out_x, int *out_y, font_descriptor_t *font, int scale, char *string, uint32_t color);

void draw_shaded_rect(bitamp b, int x0, int y0, int x1, int y1, uint32_t color_base, uint32_t color_dark, uint32_t color_light);

#endif
