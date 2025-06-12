#include <stdlib.h>
#include "game.h"

#define BLOCK_LENGTH 30

#define PLAYFIELD_WIDTH  BLOCK_LENGTH * 10
#define PLAYFIELD_HEIGHT BLOCK_LENGTH * 20

#define PLAYFIELD_X (WINDOW_WIDTH  -  PLAYFIELD_WIDTH) / 2
#define PLAYFIELD_Y (WINDOW_HEIGHT - PLAYFIELD_HEIGHT) / 2


static Tetromino tetromino_rotate_counterclockwise(Tetromino tetromino);
static Tetromino tetromino_rotate_clockwise(Tetromino tetromino);

static void draw_playfield(int playfield[20], Tetromino tetromino);

static const Tetromino tetromino_table[7] =
{
    {
        .type = TETROMINO_O,

        .x = 4,
        .y = -1,

        .shape = {
            1,1,0,0,
            1,1,0,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_L,

        .x = 3,
        .y = -1,

        .shape = {
            0,0,1,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_J,

        .x = 3,
        .y = -1,

        .shape = {
            1,0,0,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_S,

        .x = 3,
        .y = -1,

        .shape = {
            0,1,1,0,
            1,1,0,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_Z,

        .x = 3,
        .y = -1,

        .shape = {
            1,1,0,0,
            0,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_T,

        .x = 3,
        .y = -1,

        .shape = {
            0,1,0,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_I,

        .x = 3,
        .y = -1,

        .shape = {
            0,0,0,0,
            1,1,1,1,
            0,0,0,0,
            0,0,0,0
        }
    }
};

Tetromino tetromino_rotate_counterclockwise(Tetromino tetromino)
{
    Tetromino rotated_tetromino;

    rotated_tetromino.x = tetromino.x;
    rotated_tetromino.y = tetromino.y;

    rotated_tetromino.type = tetromino.type;

    switch (tetromino.type) {
    case TETROMINO_O:
    {
        int shape[16] = {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };

        memcpy(rotated_tetromino.shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
        int shape = {
            tetromino.shape[2], tetromino.shape[6], tetromino.shape[10], 0,
            tetromino.shape[1], tetromino.shape[5], tetromino.shape[9],  0,
            tetromino.shape[0], tetromino.shape[4], tetromino.shape[8],  0,
            0,                  0,                  0,                   0
        };

        memcpy(rotated_tetromino.shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_I:
    {
        int shape = {
            tetromino.shape[3], tetromino.shape[7], tetromino.shape[11], tetromino.shape[15],
            tetromino.shape[2], tetromino.shape[6], tetromino.shape[10], tetromino.shape[14],
            tetromino.shape[1], tetromino.shape[5], tetromino.shape[9],  tetromino.shape[13],
            tetromino.shape[0], tetromino.shape[4], tetromino.shape[8],  tetromino.shape[12]
        };

        memcpy(rotated_tetromino.shape, shape, sizeof(shape));
        break;
    }
    }

    return rotated_tetromino;
}

Tetromino tetromino_rotate_clockwise(Tetromino tetromino)
{
        Tetromino rotated_tetromino;

    rotated_tetromino.x = tetromino.x;
    rotated_tetromino.y = tetromino.y;

    rotated_tetromino.type = tetromino.type;

    switch (tetromino.type) {
    case TETROMINO_O:
    {
        int shape[16] = {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };

        memcpy(rotated_tetromino.shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
        int shape = {
            tetromino.shape[10], tetromino.shape[6], tetromino.shape[2],  0,
            tetromino.shape[9],  tetromino.shape[5], tetromino.shape[1],  0,
            tetromino.shape[8],  tetromino.shape[4], tetromino.shape[0],  0,
            0,                   0,                  0,                   0
        };

        memcpy(rotated_tetromino.shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_I:
    {
        int shape = {
            tetromino.shape[15], tetromino.shape[11], tetromino.shape[7], tetromino.shape[3],
            tetromino.shape[14], tetromino.shape[10], tetromino.shape[6], tetromino.shape[2],
            tetromino.shape[13], tetromino.shape[9],  tetromino.shape[5], tetromino.shape[1],
            tetromino.shape[12], tetromino.shape[8],  tetromino.shape[4], tetromino.shape[0]
        };

        memcpy(rotated_tetromino.shape, shape, sizeof(shape));
        break;
    }
    }

    return rotated_tetromino;
}

void draw_playfield(int playfield[20], Tetromino tetromino)
{
    ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);

    for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 20; j++) {
                if (playfield[i + 10 * j]) {
                    al_draw_filled_rectangle(
                    PLAYFIELD_X + i * BLOCK_LENGTH,
                    PLAYFIELD_Y + j * BLOCK_LENGTH,
                    PLAYFIELD_X + (i + 1) * BLOCK_LENGTH,
                    PLAYFIELD_Y + (j + 1) * BLOCK_LENGTH,
                    red
                    );
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (tetromino.shape[i + 4 * j]) {
                    al_draw_filled_rectangle(
                    PLAYFIELD_X + (i + tetromino.x) * BLOCK_LENGTH,
                    PLAYFIELD_Y + (j + tetromino.y) * BLOCK_LENGTH,
                    PLAYFIELD_X + (i + tetromino.x + 1) * BLOCK_LENGTH,
                    PLAYFIELD_Y + (j + tetromino.y + 1) * BLOCK_LENGTH,
                    red
                    );
                }
            }
        }

        al_draw_rectangle(
        PLAYFIELD_X,
        PLAYFIELD_Y,
        PLAYFIELD_X + PLAYFIELD_WIDTH,
        PLAYFIELD_Y + PLAYFIELD_HEIGHT,
        red,
        1
        );
}

GameMode game_playfield(AllegroContext *allegro, Input *input)
{
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    int playfield[200] = {0};

    Tetromino tetromino = tetromino_table[6];

    if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
        al_clear_to_color(black);

        draw_playfield(playfield, tetromino);

        allegro->redraw = 1;
    }

    return MODE_PLAYFIELD;
}
