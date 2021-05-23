
#include "game.h"

#include <stdio.h>
#include <stdlib.h>

enum
{
    FIELD_TILE_SIZE = 16,
};

static uint8_t piece_values_I[] = {
    0,0,0,0,
    0,0,0,0,
    1,1,1,1,
    0,0,0,0,
};

static uint8_t piece_values_T[] = {
    0,0,0,
    0,2,0,
    2,2,2,
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

const uint32_t colors_base[] = {
    0x21d7de,
    0xe651de,
    0x21d729,
    0xff3d29,
    0xffe729,
    0xe69629,
    0x0010ce
};

static int
get_frames_between_drops(int level)
{
    static int lut[30] = {
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
    if(piece_collides(g))
    {
        g->state = GAME_GAMEOVER;
    }
}

static void
remove_filled_lines(game *g)
{
    int source_row_index = FIELD_HEIGHT - 1;
    int destination_row_index = FIELD_HEIGHT - 1;
    while(source_row_index >= 0)
    {
        if(g->linefill.filled_lines[source_row_index])
        {
            --source_row_index;
            continue;
        }
        uint8_t *src = g->field + source_row_index*FIELD_WIDTH;
        uint8_t *dst = g->field + destination_row_index*FIELD_WIDTH;
        for(int x = 0; x < FIELD_WIDTH; ++x)
        {
            dst[x] = src[x];
        }
        --source_row_index, --destination_row_index;
    }

    // Fill the rest of the field with 0
    while(destination_row_index >= 0)
    {
        uint8_t *dst = g->field + destination_row_index*FIELD_WIDTH;
        for(int x = 0; x < FIELD_WIDTH; ++x)
        {
            dst[x] = 0;
        }
        --destination_row_index;
    }

    g->total_lines += g->linefill.filled_lines_count;
    switch(g->linefill.filled_lines_count)
    {
        case 1:
        {
            g->score += (g->level+1) * 40;
        } break;
        case 2:
        {
            g->score += (g->level+1) * 100;
        } break;
        case 3:
        {
            g->score += (g->level+1) * 300;
        } break;
        case 4:
        {
            g->score += (g->level+1) * 1200;
        } break;
    }
    if(g->total_lines >= 10*(g->level - g->start_level + 1))
    {
        ++g->level;
    }
}

static int
get_filled_lines(game *g, uint8_t *filled_lines_array)
{
    int filled_lines_count = 0;
    for(int y = 0; y < FIELD_HEIGHT; ++y)
    {
        uint8_t filled = 1;
        for(int x = 0; x < FIELD_WIDTH; ++x)
        {
            if(!g->field[y*FIELD_WIDTH + x])
            {
                filled = 0;
                break;
            }
        }
        filled_lines_count += filled;
        filled_lines_array[y] = filled;
    }
    return filled_lines_count;
}

static void
handle_merged_piece(game *g)
{
    g->linefill.filled_lines_count = get_filled_lines(g, g->linefill.filled_lines);
    if(g->linefill.filled_lines_count == 0)
    {
        spawn_new_piece(g);
    }
    else
    {
        g->state = GAME_LINEFILL;
        g->linefill.frames_left = 20;
        g->linefill.led_word = 0x55555555;
    }
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
                draw_rect(frame,
                    left + actual_x*FIELD_TILE_SIZE, top + actual_y*FIELD_TILE_SIZE,
                    left + (actual_x+1)*FIELD_TILE_SIZE, top + (actual_y+1)*FIELD_TILE_SIZE,
                    colors_base[value-1]);
            }
        }
    }
}

static void
draw_playing_field(game *g, bitmap frame)
{
    int top = (FIELD_VISIBLE_HEIGHT - FIELD_HEIGHT)*FIELD_TILE_SIZE;
    int left = (frame.width - FIELD_TILE_SIZE*FIELD_WIDTH) / 2;

    draw_rect(frame, left, top, left + FIELD_WIDTH*FIELD_TILE_SIZE, top + FIELD_HEIGHT*FIELD_TILE_SIZE, 0x333333);

    for(int y = FIELD_HEIGHT - FIELD_VISIBLE_HEIGHT; y < FIELD_HEIGHT; ++y)
    {
        for(int x = 0; x < FIELD_WIDTH; ++x)
        {
            uint8_t value = g->field[y*FIELD_WIDTH + x];
            if(value)
            {
                draw_rect(frame,
                    left + x*FIELD_TILE_SIZE, top + y*FIELD_TILE_SIZE,
                    left + (x+1)*FIELD_TILE_SIZE, top + (y+1)*FIELD_TILE_SIZE,
                    colors_base[value-1]);
            }
        }
    }
}

static void
draw_gameover(game *g, bitmap frame)
{
    char *string = "GAME OVER";
    int width;
    get_string_size(g->font, g->scale_large, string, &width, NULL);
    int start_x = (frame.width - width) / 2;
    int start_y = frame.height / 2;
    draw_string(frame, start_x, start_y, NULL, NULL, g->font, g->scale_large, string, 0xffffff);
}

static void
draw_statistics(game *g, bitmap frame)
{
    int top = 0;
    int left = (frame.width + FIELD_TILE_SIZE*FIELD_WIDTH) / 2;

    char stats[64];
    sprintf(stats, "LEVEL\n%06d\n\nLINES\n%06d\n\nSCORE\n%06d", g->level, g->total_lines, g->score);
    draw_string(frame, left, top, NULL, NULL, g->font, g->scale_small, stats, 0xffffff);
}

static void
draw_game(game *g, bitmap frame)
{
    switch(g->state)
    {
        case GAME_PLAYING:
        case GAME_LINEFILL:
        {
            draw_playing_field(g, frame);
            draw_piece(g, frame);
            draw_statistics(g, frame);
        } break;

        case GAME_GAMEOVER:
        {
            draw_gameover(g, frame);
        } break;
    }
}

static int
update_game_linefill(game *g, input *in)
{
    --g->linefill.frames_left;
    if(g->linefill.frames_left <= 0)
    {
        apo_led_line_set_word(0);
        g->state = GAME_PLAYING;
        remove_filled_lines(g);
        spawn_new_piece(g);
        return 0;
    }

    apo_led_line_set_word(g->linefill.led_word);
    g->linefill.led_word = ~g->linefill.led_word;

    return 0;
}

static int
update_game_playing(game *g, input *in)
{
    if(in->keys[KEY_UP])
    {
        rotate(g);
    }
    else if(in->keys[KEY_LEFT])
    {
        move_sideways(g, -1);
    }
    else if(in->keys[KEY_RIGHT])
    {
        move_sideways(g, 1);
    }
    else if(in->keys[KEY_DOWN])
    {
        if(!soft_drop(g))
        {
            handle_merged_piece(g);
            return 0;
        }
    }
    else if(in->keys[KEY_SPACE])
    {
        hard_drop(g);
        handle_merged_piece(g);
        return 0;
    }

    g->active.frames_since_drop += 1;
    if(g->active.frames_since_drop >= get_frames_between_drops(g->level))
    {
        if(!soft_drop(g))
        {
            handle_merged_piece(g);
            return 0;
        }
    }

    if(in->keys[KEY_ESCAPE])
    {
        return 1;
    }
    return 0;
}

static int
update_game_gameover(game *g, input *in)
{
    if(in->keys[KEY_ESCAPE])
    {
        return 1;
    }
    return 0;
}

int
update_game(game *g, input *in, bitmap frame)
{
    int should_quit = 0;
    switch(g->state)
    {
        case GAME_PLAYING:
        {
            should_quit = update_game_playing(g, in);
        } break;

        case GAME_LINEFILL:
        {
            should_quit = update_game_linefill(g, in);
        } break;

        case GAME_GAMEOVER:
        {
            should_quit = update_game_gameover(g, in);
        } break;
    }

    draw_game(g, frame);

    return should_quit;
}
