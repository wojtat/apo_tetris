/*******************************************************************
  Defines an api for the game struct and game related logic

  game.c

  Author: Vojtech Tilhon, tilhovoj@fel.cvut.cz

 *******************************************************************/

#include "tetrominoes.h"
#include "game.h"
#include "mzapo_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    FIELD_TILE_SIZE = 16,
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
    g->active.xoff = (FIELD_WIDTH - tetrominoes[piece_index].side_length) / 2;
    g->active.yoff = 0;
    g->active.rotation = 0;
    g->active.frames_since_drop = 0;
    g->active.tetromino_desc_index = piece_index;
    if(piece_collides(g))
    {
        g->state = GAME_GAMEOVER;
        g->gameover.m.selected = 0;
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
        g->linefill.led_colors[0] = 0x00ff0000;
        g->linefill.led_colors[1] = 0x000000ff;
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
                    tetromino_colors[value-1]);
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
                    tetromino_colors[value-1]);
            }
        }
    }
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
    draw_playing_field(g, frame);
    draw_piece(g, frame);
    draw_statistics(g, frame);
}

static int
update_game_linefill(game *g, input *in)
{
    --g->linefill.frames_left;
    if(g->linefill.frames_left <= 0)
    {
        apo_led_line_set_word(0);
        apo_led_set_color(1, 0);
        apo_led_set_color(2, 0);
        g->state = GAME_PLAYING;
        remove_filled_lines(g);
        spawn_new_piece(g);
        return 0;
    }

    if(g->linefill.frames_left % 2 == 0)
    {
        apo_led_set_color(1, g->linefill.led_colors[0]);
        apo_led_set_color(2, g->linefill.led_colors[1]);
    }
    else
    {
        apo_led_set_color(1, g->linefill.led_colors[1]);
        apo_led_set_color(2, g->linefill.led_colors[0]);
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
        g->state = GAME_PAUSE;
        g->pause.m.selected = 0;
    }
    return 0;
}

static int
update_game_gameover(game *g, input *in, bitmap frame)
{
    menu_start_update(&g->gameover.m, in, frame, 2);
    menu_do_title(&g->gameover.m, "GAME OVER\n");
    if(menu_do_item(&g->gameover.m, "Start Menu\n") == INTERACTION_ENTER)
    {
        g->state = GAME_START;
        memset(g->field, 0, FIELD_WIDTH*FIELD_HEIGHT);
        g->start_level = g->level = g->total_lines = g->score = 0;
        g->start.m.selected = 0;
    }
    if(menu_do_item(&g->gameover.m, "Exit\n") == INTERACTION_ENTER)
    {
        return 1;
    }
    return 0;
}

static int
update_game_start(game *g, input *in, bitmap frame)
{
    menu_start_update(&g->start.m, in, frame, 3);
    menu_do_title(&g->start.m, "START MENU\n");

    if(menu_do_item(&g->start.m, "New Game\n") == INTERACTION_ENTER)
    {
        g->state = GAME_PLAYING;
        memset(g->field, 0, FIELD_WIDTH*FIELD_HEIGHT);
        g->level = g->start_level;
        g->total_lines = g->score = 0;
        spawn_new_piece(g);
    }
    
    char start_level_string[32];
    sprintf(start_level_string, "Start Level %d\n", g->start_level);

    interaction_type type = menu_do_item(&g->start.m, start_level_string);
    if(type == INTERACTION_LEFT)
    {
        --g->start_level;
    }
    else if(type == INTERACTION_RIGHT)
    {
        ++g->start_level;
    }

    if(menu_do_item(&g->start.m, "Exit\n") == INTERACTION_ENTER)
    {
        return 1;
    }
    return 0;
}

static int
update_game_pause(game *g, input *in, bitmap frame)
{
    menu_start_update(&g->pause.m, in, frame, 3);
    menu_do_title(&g->pause.m, "PAUSE MENU\n");

    if(menu_do_item(&g->pause.m, "Resume Game\n") == INTERACTION_ENTER)
    {
        g->state = GAME_PLAYING;
    }
    if(menu_do_item(&g->pause.m, "Start Menu\n") == INTERACTION_ENTER)
    {
        g->state = GAME_START;
        memset(g->field, 0, FIELD_WIDTH*FIELD_HEIGHT);
        g->start_level = g->level = g->total_lines = g->score = 0;
        g->start.m.selected = 0;
    }
    if(menu_do_item(&g->pause.m, "Exit\n") == INTERACTION_ENTER)
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
        case GAME_START:
        {
            should_quit = update_game_start(g, in, frame);
        } break;
        case GAME_PLAYING:
        {
            should_quit = update_game_playing(g, in);
            draw_game(g, frame);
        } break;
        case GAME_LINEFILL:
        {
            should_quit = update_game_linefill(g, in);
            draw_game(g, frame);
        } break;
        case GAME_PAUSE:
        {
            should_quit = update_game_pause(g, in, frame);
        } break;
        case GAME_GAMEOVER:
        {
            should_quit = update_game_gameover(g, in, frame);
        } break;
    }

    return should_quit;
}
