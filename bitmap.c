
#include <stdlib.h>
#include <stdio.h>

#include "bitmap.h"

bitmap
make_bitmap(int width, int height)
{
    bitmap b;

    b.height = height;
    b.width = width;
    b.pixels = malloc(4*height*width);

    return b;
}

void
free_bitmap(bitmap *b)
{
    free(b->pixels);
    b->pixels = NULL;
    b->width = b->height = 0;
}

void
fill_bitmap(bitmap b, uint32_t color)
{
    draw_rect(b, 0, 0, b.width, b.height, color);
}

void
draw_rect(bitmap b, int x0, int y0, int x1, int y1, uint32_t color)
{
    // Clamp
    y0 = y0 < 0 ? 0 : y0;
    y1 = y1 > b.height ? b.height : y1;
    x0 = x0 < 0 ? 0 : x0;
    x1 = x1 > b.width ? b.width : x1;

    uint32_t *line = b.pixels + y0*b.width;
    for(int y = y0; y < y1; ++y)
    {
        uint32_t *pixel = line + x0;
        for(int x = x0; x < x1; ++x)
        {
            *pixel = color;
            ++pixel;
        }
        line += b.width;
    }
}

void
draw_shaded_rect(bitamp b, int x0, int y0, int x1, int y1, uint32_t color_base, uint32_t color_dark, uint32_t color_light)
{
    draw_rect(b, x0, y0, x1, y1, color_dark);
    draw_rect(b, x0+1, y0, x1, y1-1, color_light);
    draw_rect(b, x0-1, y0-1, x1-1, y1-1, color_base);
}

static int
get_char_width(font_descriptor_t *font, char c)
{
    int char_width;
    if(font->width)
    {
        int glyph_index = c - font->firstchar;
        char_width = font->width[glyph_index];
    }
    else
    {
        char_width = font->maxwidth;
    }
    return char_width;
}

static int
draw_char(bitmap b, int x0, int y0, font_descriptor_t *font, int scale, char c, uint32_t color)
{
    if(c < font->firstchar || c - font->firstchar >= font->size)
    {
        return 0;
    }

    const font_bits_t *char_bits;
    int glyph_index = c - font->firstchar;

    if(font->offset)
    {
        char_bits = font->bits + font->offset[glyph_index];
    }
    else
    {
        char_bits = font->bits + font->height * ((font->maxwidth+15)/16) * glyph_index;
    }

    int char_width = get_char_width(font, c);

    for(int y = 0; y < font->height; ++y)
    {
        font_bits_t val = char_bits[y];
        for(int x = 0; x < char_width; ++x)
        {
            if((val & 0x8000) != 0)
            {
                draw_rect(b, x0 + x*scale, y0 + y*scale, x0 + (x+1)*scale, y0 + (y+1)*scale, color);
            }
            val <<= 1;
        }
    }

    return char_width*scale;
}

void
get_string_size(font_descriptor_t *font, int scale, char *string, int *out_width, int *out_height)
{
    int max_width = 0;
    int current_width = 0;
    int height = font->height;
    while(*string)
    {
        if(*string == '\n')
        {
            height += font->height;
            if(current_width > max_width)
            {
                max_width = current_width;
            }
            current_width = 0;
        }
        else
        {
            current_width += get_char_width(font, *string);
        }
        ++string;
    }
    if(current_width > max_width)
    {
        max_width = current_width;
    }
    
    if(out_width)
    {
        *out_width = max_width * scale;
    }
    if(out_height)
    {
        *out_height = height * scale;
    }
}

void
draw_string(bitmap b, int x, int y, int *out_x, int *out_y, font_descriptor_t *font, int scale, char *string, uint32_t color)
{
    int current_xoff = 0;
    int current_yoff = 0;

    while(*string)
    {
        if(*string == '\n')
        {
            current_yoff += font->height*scale;
            current_xoff = 0;
        }
        else
        {
            current_xoff += draw_char(b, x + current_xoff, y + current_yoff, font, scale, *string, color);
        }
        ++string;
    }

    if(out_x)
    {
        *out_x = current_xoff;
    }
    if(out_y)
    {
        *out_y = current_yoff;
    }
}
