/*******************************************************************
  Defines all the possible game pieces
  and their properties used in the game

  tetrominoes.h

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#ifndef TETROMINOES_H
#define TETROMINOES_H

#include <stdint.h>

// The number of tetrominoes that exist
enum
{
    TETROMINO_COUNT = 7,
};

// Describes an individual tetromino shape and size
typedef struct tetromino_desc
{
    int side_length;
    uint8_t *values;
} tetromino_desc;

// Represents an actively moving tetromino piece
typedef struct tetromino
{
    int xoff;
    int yoff;
    int rotation;
    int tetromino_desc_index;
    int frames_since_drop;
} tetromino;

// The colors associated with the tetrominoes
extern const uint32_t tetromino_colors[TETROMINO_COUNT];
// The shapes and sizes associated with the tetrominoes
extern const tetromino_desc tetrominoes[TETROMINO_COUNT];

// Get the value at the specified x an y of an actively moving tetromino
// taking into account the rotation as well
uint8_t tetromino_get_value(tetromino *piece, int x, int y);

#endif
