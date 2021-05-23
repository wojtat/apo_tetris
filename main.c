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
#include <time.h>

#include "mzapo_parlcd.h"
#include "mzapo_api.h"
#include "bitmap.h"
#include "game.h"
#include "font_types.h"

float
get_current_time(void)
{
    clock_t current_time = clock();
    if(current_time == (clock_t)-1)
    {
        return 0.f;
    }
    return (float)current_time / CLOCKS_PER_SEC * 1000.f;
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

    bitmap frame = make_bitmap(480, 320);
    game g = {0};
    g.state = GAME_PLAYING;

    float last_frame_time = get_current_time();
    float target_millis_per_frame = 1000.f / 20.f;

    int quit = 0;
    while(!quit)
    {
        float current_time = last_frame_time;
        while(last_frame_time + target_millis_per_frame > current_time)
        {
            current_time = get_current_time();
        }

        input in = {0};
        key_id key;
        while((key = apo_read_key_input()) != KEY_NONE)
        {
            in.keys[key] = 1;
        }

        fill_bitmap(frame, 0);
        quit = update_game(&g, &in, frame);

        apo_lcd_draw_frame(frame);
        last_frame_time = current_time;
    }

/*
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
*/

    free_bitmap(&frame);
    return 0;
}
