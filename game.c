
#include "game.h"

#include <stdio.h>
#include <stdlib.h>

enum
{
    FIELD_TILE_SIZE = 16,
};

static uint8_t piece_values_I[] =
{
    0,0,0,0,
    0,0,0,0,
    1,1,1,1,
    0,0,0,0,
};

static uint8_t piece_values_T[] =
{
    0,0,0,
    0,2,0,
    2,2,2,
};

static uint8_t piece_values_S[] =
{
    0,0,0,
    0,3,3,
    3,3,0,
};

static uint8_t piece_values_Z[] =
{
    0,0,0,
    4,4,0,
    0,4,4,
};

static uint8_t piece_values_O[] =
{
    5,5,
    5,5,
};

const tetromino_desc tetrominoes[TETROMINO_COUNT] =
{
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
};

const uint32_t colors_base[] = {
    0x282828,
    0x2D9999,
    0x99992D,
    0x992D99,
    0x2D9951,
};

const uint32_t colors_dark[] = {
    0x282828,
    0x1E6666,
    0x66661E,
    0x661E66,
    0x1E6636,
};

const uint32_t colors_light[] = {
    0x282828,
    0x44E5E5,
    0xE5E544,
    0xE544E5,
    0x44E57A,
};

static int
get_frames_between_drops(int level)
{
    static int lut[30] =
    {
        48,43,38,33,28,23,18,13,8,6,5,5,5,4,4,4,3,3,3,2,2,2,2,2,2,2,2,2,2,1
    };
    if(level > 29)
    {
        level = 29;
    }
    return lut[level];
}

static uint8_t
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

static int
piece_collides(game *g)
{
    const tetromino_desc *piece_desc = tetrominoes + g->active.tetromino_desc_index;

    for(int y = 0; y < piece_desc->side_length; ++y)
    {
        for(int x = 0; x < piece_desc->side_length; ++x)
        {
            if(get_tetromino_value(&g->active, x, y))
            {
                int actual_x = x + g->active.xoff;
                int actual_y = y + g->active.yoff;
                if(actual_x < 0 || actual_x >= FIELD_WIDTH || actual_y < 0 || actual_y >= FIELD_HEIGHT)
                {
                    return 1;
                }
                if(g->field[actual_y*FIELD_WIDTH + actual_x])
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

static void
merge_piece(game *g)
{
    const tetromino_desc *piece_desc = tetrominoes + g->active.tetromino_desc_index;

    for(int y = 0; y < piece_desc->side_length; ++y)
    {
        for(int x = 0; x < piece_desc->side_length; ++x)
        {
            uint8_t piece_value = get_tetromino_value(&g->active, x, y);
            if(piece_value)
            {
                int actual_x = x + g->active.xoff;
                int actual_y = y + g->active.yoff;
                g->field[actual_y*FIELD_WIDTH + actual_x] = piece_value;
            }
        }
    }
}

static int
soft_drop(game *g)
{
    g->active.frames_since_drop = 0;
    g->active.yoff += 1;
    if(piece_collides(g))
    {
        g->active.yoff -= 1;
        merge_piece(g);
        return 0;
    }
    return 1;
}

static void
hard_drop(game *g)
{
    while(soft_drop(g));
}

static void
rotate(game *g)
{
    int old_rotation = g->active.rotation;
    g->active.rotation = (g->active.rotation + 1) % 4;
    if(piece_collides(g))
    {
        g->active.rotation = old_rotation;
    }
}

static void
move_sideways(game *g, int delta)
{
    int old_xoff = g->active.xoff;
    g->active.xoff = (g->active.xoff + delta);
    if(piece_collides(g))
    {
        g->active.xoff = old_xoff;
    }
}

static void
spawn_new_piece(game *g)
{
    int piece_index = rand() % TETROMINO_COUNT;
    g->active.xoff = 0;
    g->active.yoff = 0;
    g->active.rotation = 0;
    g->active.frames_since_drop = 0;
    g->active.tetromino_desc_index = piece_index;
}

static void
draw_piece(game *g, bitmap frame)
{
    const tetromino_desc *piece_desc = tetrominoes + g->active.tetromino_desc_index;

    int top_logical = FIELD_HEIGHT - FIELD_VISIBLE_HEIGHT;
    int top = -top_logical*FIELD_TILE_SIZE;
    int left = (frame.width - FIELD_TILE_SIZE*FIELD_WIDTH) / 2;

    for(int y = 0; y < piece_desc->side_length; ++y)
    {
        for(int x = 0; x < piece_desc->side_length; ++x)
        {
            uint8_t value = get_tetromino_value(&g->active, x, y);
            if(value)
            {
                int actual_x = x + g->active.xoff;
                int actual_y = y + g->active.yoff;
                draw_shaded_rect(frame,
                    left + actual_x*FIELD_TILE_SIZE, top + actual_y*FIELD_TILE_SIZE,
                    left + (actual_x+1)*FIELD_TILE_SIZE, top + (actual_y+1)*FIELD_TILE_SIZE,
                    colors_base[value-1], colors_dark[value-1], colors_light[value-1]);
            }
        }
    }
}

static void
draw_playing_field(game *g, bitmap frame)
{
    int top = (FIELD_VISIBLE_HEIGHT - FIELD_HEIGHT)*FIELD_TILE_SIZE;
    int left = (frame.width - FIELD_TILE_SIZE*FIELD_WIDTH) / 2;

    for(int y = FIELD_HEIGHT - FIELD_VISIBLE_HEIGHT; y < FIELD_HEIGHT; ++y)
    {
        for(int x = 0; x < FIELD_WIDTH; ++x)
        {
            uint8_t value = g->field[y*FIELD_WIDTH + x];
            if(value)
            {
                draw_shaded_rect(frame,
                    left + x*FIELD_TILE_SIZE, top + y*FIELD_TILE_SIZE,
                    left + (x+1)*FIELD_TILE_SIZE, top + (y+1)*FIELD_TILE_SIZE,
                    colors_base[value-1], colors_dark[value-1], colors_light[value-1]);
            }
        }
    }
}

static int
update_game_playing(game *g, input *in, bitmap frame)
{
    if(in->keys[KEY_UP])
    {
        rotate(g);
    }
    if(in->keys[KEY_LEFT])
    {
        move_sideways(g, -1);
    }
    if(in->keys[KEY_RIGHT])
    {
        move_sideways(g, 1);
    }
    if(in->keys[KEY_DOWN])
    {
        if(!soft_drop(g))
        {
            spawn_new_piece(g);
        }
    }
    if(in->keys[KEY_SPACE])
    {
        hard_drop(g);
        spawn_new_piece(g);
    }

    g->active.frames_since_drop += 1;
    if(g->active.frames_since_drop >= get_frames_between_drops(g->level))
    {
        if(!soft_drop(g))
        {
            spawn_new_piece(g);
        }
    }

    draw_playing_field(g, frame);
    draw_piece(g, frame);

    if(in->keys[KEY_ESCAPE])
    {
        return 1;
    }
    return 0;
}

int
update_game(game *g, input *in, bitmap frame)
{
    switch(g->state)
    {
        case GAME_PLAYING:
        {
            return update_game_playing(g, in, frame);
        } break;
    }
    return 0;
}
