/*******************************************************************
  The entry point of the application

  main.c      - main file

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_api.h"

static void fill_bitmap(bitmap b, uint32_t color)
{
    uint32_t *line = b.pixels;
    for(int y = 0; y < b.height; ++y)
    {
        uint32_t *pixel = line;
        for(int x = 0; x < b.width; ++x)
        {
            *pixel = color;
            ++pixel;
        }
        line += b.width;
    }
}

int main(int argc, char *argv[])
{
    printf("Hello world\n");
    apo_initialise();
    uint32_t colors[3] = {
        0x00ffff,
        0xffff00,
        0xff00ff
    };

    bitmap frame;
    frame.width = 480;
    frame.height = 320;
    frame.pixels = malloc(4*frame.width*frame.height);

    for(int i = 0;; ++i)
    {
        apo_led_set_color(1, colors[i % (sizeof(colors)/sizeof(colors[0]))]);
        apo_led_set_color(2, colors[(i+1) % (sizeof(colors)/sizeof(colors[0]))]);
        fill_bitmap(frame, colors[i % (sizeof(colors)/sizeof(colors[0]))]);
        apo_lcd_draw_frame(frame);
        printf("CHANGING COLOR AND DRAWING BITMAP!\n");
        sleep(4);
    }

    return 0;
}
