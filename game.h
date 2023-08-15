/*******************************************************************
  Defines an api for the game struct and game related logic

  game.h

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#ifndef GAME_H
#define GAME_H

#include "tetrominoes.h"
#include "bitmap.h"
#include "menu.h"

// Constants for related to the game
enum
{
    FIELD_WIDTH = 10,
    FIELD_HEIGHT = 22,
    FIELD_VISIBLE_HEIGHT = 20,
};

// Defines possible states of the game/application
typedef enum game_state
{
    GAME_START,
    GAME_PLAYING,
    GAME_LINEFILL,
    GAME_PAUSE,
    GAME_GAMEOVER,
} game_state;

// The whole game/application structure
typedef struct game
{
    game_state state;

    uint8_t field[FIELD_WIDTH*FIELD_HEIGHT];
    int start_level;
    int level;
    int total_lines;
    int score;
    tetromino active;

    font_descriptor_t *font;
    int scale_large;
    int scale_small;

    struct
    {
        menu m;
    } start;

    struct
    {
        int frames_left;
        int filled_lines_count;
        uint8_t filled_lines[FIELD_HEIGHT];
        uint32_t led_word;
        uint32_t led_colors[2];
    } linefill;

    struct
    {
        menu m;
    } pause;

    struct
    {
        menu m;
    } gameover;
} game;

// Update the game according to the current state
// and input and render it to the frame
int game_update(game *g, input *in, bitmap frame);

#endif
