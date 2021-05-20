/*******************************************************************
  Provides an interface for operating with the hardware.
  Allows you to write RGB colors to the LEDs and LED lines, write to the
  LCD display.

  mzapo_api.h

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#ifndef MZAPO_API_H
#define MZAPO_API_H

#include "bitmap.h"

typedef enum key_id
{
    KEY_NONE = -1,
    KEY_ESCAPE,
    KEY_ENTER,
    KEY_LEFT,
    KEY_UP,
    KEY_RIGHT,
    KEY_DOWN,
    KEY_SPACE,

    KEY_COUNT
} key_id;

void apo_initialise(void);

void apo_led_set_color(int led_id, uint32_t color);

void apo_lcd_draw_frame(bitmap frame);

key_id apo_read_key_input(void);

#endif