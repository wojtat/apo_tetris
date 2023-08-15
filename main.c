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
#include "game.h"
#include "font_types.h"

// Get the current time in millis
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
    // Initialise the hardware control module
    mz_initialise();

    // Initialise the game and bitmap frame
    bitmap frame = bitmap_make(480, 320);
    game g = {0};
    g.state = GAME_START;
    g.font = &font_winFreeSystem14x16;
    g.scale_large = 4;
    g.scale_small = 2;
    g.start.m = g.pause.m = g.gameover.m = menu_make(g.font, g.scale_large, g.scale_small, 0xffffff, 0xff0000);

    float last_frame_time = get_current_time();

    // Run at 20 FPS, ideally
    float target_millis_per_frame = 1000.f / 20.f;

    int quit = 0;
    while(!quit)
    {
        // Wait until the time has come to update
        // Maybe sleeping would be more efficient
        float current_time = last_frame_time;
        while(last_frame_time + target_millis_per_frame > current_time)
        {
            current_time = get_current_time();
        }

        // Get the input from raw stdin
        input in = {0};
        key_id key;
        while((key = mz_read_key_input()) != KEY_NONE)
        {
            in.keys[key] = 1;
        }

        // Clear the frame and update and render the game
        bitmap_fill(frame, 0);
        quit = game_update(&g, &in, frame);

        // Draw the frame to the LCD display
        mz_lcd_draw_frame(frame);
        last_frame_time = current_time;
    }

    // Clear the display
    bitmap_fill(frame, 0);
    mz_lcd_draw_frame(frame);

    // Cleanup
    bitmap_free(&frame);

    return 0;
}
