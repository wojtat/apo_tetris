
#include <stddef.h>

#include "menu.h"

void
menu_start_update(menu *m, input *in, bitmap frame, int item_count)
{
    m->in = in;
    m->frame = frame;
    m->item_count = item_count;
    m->current_xoff = 0;
    m->current_yoff = 0;
    m->current_item = 0;

    if(in->keys[KEY_UP])
    {
        m->selected = (m->selected + item_count - 1) % item_count;
    }
    if(in->keys[KEY_DOWN])
    {
        m->selected = (m->selected + 1) % item_count;
    }
}

void
menu_do_title(menu *m, char *title_string)
{
    int height = 0;
    draw_string(m->frame,
        m->current_xoff, m->current_yoff,
        NULL, &height, m->font, m->scale_large,
        title_string, m->base_color);
    m->current_yoff += height;
}

interaction_type
menu_do_item(menu *m, char *display_string)
{
    interaction_type interaction = INTERACTION_NONE;
    uint32_t color = m->base_color;
    if(m->selected == m->current_item)
    {
        color = m->highlight_color;
        if(m->in->keys[KEY_ENTER])
        {
            interaction = INTERACTION_ENTER;
        }
        else if(m->in->keys[KEY_LEFT])
        {
            interaction = INTERACTION_LEFT;
        }
        else if(m->in->keys[KEY_RIGHT])
        {
            interaction = INTERACTION_RIGHT;
        }
    }

    int height = 0;
    draw_string(m->frame,
        m->current_xoff, m->current_yoff,
        NULL, &height, m->font, m->scale_small,
        display_string, color);
    m->current_yoff += height;

    ++m->current_item;
    return interaction;
}

menu
make_menu(font_descriptor_t *font, int scale_large, int scale_small, uint32_t base_color, uint32_t highlight_color)
{
    menu m = {0};
    m.scale_large = scale_large;
    m.scale_small = scale_small;
    m.font = font;
    m.base_color = base_color;
    m.highlight_color = highlight_color;
    return m;
}
