/*******************************************************************
  Defines an api for the game struct

  game.h

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#ifndef GAME_H
#define GAME_H

#include "bitmap.h"
#include "mzapo_api.h"
#include "menu.h"

enum
{
    TETROMINO_COUNT = 7,

    FIELD_WIDTH = 10,
    FIELD_HEIGHT = 22,
    FIELD_VISIBLE_HEIGHT = 20,
};

typedef struct tetromino_desc
{
    int side_length;
    uint8_t *values;
} tetromino_desc;

extern const tetromino_desc tetrominoes[TETROMINO_COUNT];

typedef struct tetromino
{
    int xoff;
    int yoff;
    int rotation;
    int tetromino_desc_index;
    int frames_since_drop;
} tetromino;

typedef enum game_state
{
    GAME_PLAYING,
    GAME_LINEFILL,
    GAME_GAMEOVER,
} game_state;

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
        int frames_left;
        int filled_lines_count;
        uint8_t filled_lines[FIELD_HEIGHT];
        uint32_t led_word;
    } linefill;

    struct
    {
        menu m;
    } gameover;
} game;

int update_game(game *g, input *in, bitmap frame);

#endif
