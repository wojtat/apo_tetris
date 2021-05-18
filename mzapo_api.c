/*******************************************************************
  Provides an interface for operating with the hardware.
  Allows you to write RGB colors to the LEDs, write to the
  LCD display.

  mzapo_api.c

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "mzapo_api.h"

static uint8_t *led_base = NULL;
static uint8_t *lcd_base = NULL;

static void
init_led(void)
{
    if(led_base)
    {
        // Already initialised
        return;
    }
    led_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if(!led_base)
    {
        fprintf(stderr, "Failed to init memory!\n");
        exit(1);
    }
}

static void
init_lcd(void)
{
    if(lcd_base)
    {
        // Already initialised
        return;
    }
    lcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if(!lcd_base)
    {
        fprintf(stderr, "Failed to init memory!\n");
        exit(1);
    }
    parlcd_hx8357_init(lcd_base);
}

static uint16_t
rgb888_to_rgb565(uint32_t rgb888)
{
    uint32_t r = rgb888 >> 16;
    uint32_t g = (rgb888 >> 8) & 0xff;
    uint32_t b = rgb888 & 0xff;

    r = ((r >> 3) & 0x1f) << 11;
    g = ((g >> 2) & 0x3f) << 5;
    b = (b >> 3) & 0x1f;
    return r | g | b;
}

void
apo_lcd_draw_frame(bitmap frame)
{
    parlcd_write_cmd(lcd_base, 0x2c);

    const int height = 320;
    const int width = 480;

    uint32_t *src_line = frame.pixels;
    for(int y = 0; y < frame.height && y < height; ++y)
    {
        uint32_t *src_pixel = src_line;
        for(int x = 0; x < frame.width && x < width; ++x)
        {
            parlcd_write_data(lcd_base, rgb888_to_rgb565(*src_pixel));
            ++src_pixel;
        }
        src_line += frame.width;
    }
}

void
apo_initialise(void)
{
    init_led();
    init_lcd();
}

void
apo_led_set_color(int led_id, uint32_t color)
{
    switch(led_id)
    {
        case 1:
        {
            *(volatile uint32_t *)(led_base + SPILED_REG_LED_RGB1_o) = color;
        } break;

        case 2:
        {
            *(volatile uint32_t *)(led_base + SPILED_REG_LED_RGB2_o) = color;
        } break;

        default:
        {
            printf("Attempted to write color %x to LED number %d, which isn't supported\n", color, led_id);
        } break;
    }
}