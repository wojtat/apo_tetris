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
#include "bitmap.h"

#include "font_types.h"

int main(int argc, char *argv[])
{
    printf("Hello world\n");
    apo_initialise();
    uint32_t colors[3] = {
        0x00ffff,
        0xffff00,
        0xff00ff
    };

    bitmap frame = make_bitmap(480, 320);

    for(int i = 0;; ++i)
    {
        apo_led_set_color(1, colors[i % (sizeof(colors)/sizeof(colors[0]))]);
        apo_led_set_color(2, colors[(i+1) % (sizeof(colors)/sizeof(colors[0]))]);
        fill_bitmap(frame, 0);

        char *string = "Hello,\nWorld.";
        int width, height;
        get_string_size(&font_winFreeSystem14x16, 4, string, &width, &height);
        draw_rect(frame, 100, 100, 100+width, 100+height, 0x0000ff00);
        draw_string(frame, 100, 100, NULL, NULL, &font_winFreeSystem14x16, 4, string, 0x00ff0000);
        
        apo_lcd_draw_frame(frame);
        printf("CHANGING COLOR AND DRAWING BITMAP!\n");
        sleep(4);
    }

    free_bitmap(&frame);
    return 0;
}
