/*******************************************************************
  Defines an api for the game struct

  game.h

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#ifndef GAME_H
#define GAME_H

#include "bitmap.h"
#include "mzapo_api.h"

enum
{
    TETROMINO_COUNT = 5,

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
} game_state;

typedef struct game
{
    game_state state;

    int level;
    tetromino active;
    uint8_t field[FIELD_WIDTH*FIELD_HEIGHT];
} game;

typedef struct input
{
    int keys[KEY_COUNT];
} input;

int update_game(game *g, input *in, bitmap frame);

#endif
