
#include "tetrominoes.h"

static uint8_t piece_values_I[] = {
    0,0,0,0,
    0,0,0,0,
    1,1,1,1,
    0,0,0,0,
};

static uint8_t piece_values_T[] = {
    0,2,0,
    2,2,2,
    0,0,0,
};

static uint8_t piece_values_S[] = {
    0,0,0,
    0,3,3,
    3,3,0,
};

static uint8_t piece_values_Z[] = {
    0,0,0,
    4,4,0,
    0,4,4,
};

static uint8_t piece_values_O[] = {
    5,5,
    5,5,
};

static uint8_t piece_values_J[] = {
    6,0,0,
    6,6,6,
    0,0,0,
};

static uint8_t piece_values_L[] = {
    0,0,7,
    7,7,7,
    0,0,0,
};

const tetromino_desc tetrominoes[TETROMINO_COUNT] = {
    // I piece
    {
        4,
        piece_values_I
    },
    // T piece
    {
        3,
        piece_values_T
    },
    // S piece
    {
        3,
        piece_values_S
    },
    // Z piece
    {
        3,
        piece_values_Z
    },
    // O piece
    {
        2,
        piece_values_O
    },
    // L piece
    {
        3,
        piece_values_L
    },
    // J piece
    {
        3,
        piece_values_J
    },
};

const uint32_t tetromino_colors[] = {
    0x21d7de,
    0xe651de,
    0x21d729,
    0xff3d29,
    0xffe729,
    0xe69629,
    0x0010ce
};

uint8_t
get_tetromino_value(tetromino *piece, int x, int y)
{
    const tetromino_desc *piece_desc = tetrominoes + piece->tetromino_desc_index;
    int side = piece_desc->side_length;
    switch(piece->rotation)
    {
        case 0:
        {
            return piece_desc->values[y*side + x];
        };

        case 1:
        {
            return piece_desc->values[x*side + side - y - 1];
        };

        case 2:
        {
            return piece_desc->values[(side - y - 1)*side + side - x - 1];
        };

        case 3:
        {
            return piece_desc->values[(side - x - 1)*side + y];
        };
    }
    return 0;
}
