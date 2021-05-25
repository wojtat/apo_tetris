/*******************************************************************
  Defines an immediate GUI style menu, which displays
  menu items and provides an interface for
  interacting with them.

  menu.h

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#include "font_types.h"
#include "mzapo_api.h"
#include "bitmap.h"

// Defines the types of interactions the user
// may have with a menu item
typedef enum interaction_type
{
    INTERACTION_NONE = 0,
    INTERACTION_ENTER,
    INTERACTION_LEFT,
    INTERACTION_RIGHT,
} interaction_type;

// Defines the structure that holds the menu specific date
typedef struct menu
{
    int selected;

    int scale_large;
    int scale_small;
    font_descriptor_t *font;
    uint32_t base_color;
    uint32_t highlight_color;

    input *in;
    bitmap frame;
    int item_count;
    int current_xoff;
    int current_yoff;
    int current_item;
} menu;

// Called at the beginning of every update, sets up the menu for the frame
void menu_start_update(menu *m, input *in, bitmap frame, int item_count);

// Draws a large unselectable text as part of the menu
void menu_do_title(menu *m, char *title_string);

// Draws a menu item text and returns the type of
// interaction the user had with the item
interaction_type menu_do_item(menu *m, char *display_string);

// Makes a menu with the specified properties
menu menu_make(font_descriptor_t *font, int scale_large, int scale_small, uint32_t base_color, uint32_t highlight_color);

#endif
