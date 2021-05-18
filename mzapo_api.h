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

void apo_initialise(void);

void apo_led_set_color(int led_id, uint32_t color);

void apo_lcd_draw_frame(bitmap frame);

#endif