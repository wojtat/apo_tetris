/*******************************************************************
  Defines all the possible game pieces
  and their properties used in the game

  tetrominoes.h

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#ifndef TETROMINOES_H
#define TETROMINOES_H

#include <stdint.h>

enum
{
    TETROMINO_COUNT = 7,
};

typedef struct tetromino_desc
{
    int side_length;
    uint8_t *values;
} tetromino_desc;

typedef struct tetromino
{
    int xoff;
    int yoff;
    int rotation;
    int tetromino_desc_index;
    int frames_since_drop;
} tetromino;

extern const uint32_t tetromino_colors[TETROMINO_COUNT];
extern const tetromino_desc tetrominoes[TETROMINO_COUNT];

uint8_t get_tetromino_value(tetromino *piece, int x, int y);

#endif
