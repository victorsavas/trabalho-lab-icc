#include <stdlib.h>
#include "game.h"

#define BLOCK_LENGTH 30

#define PLAYFIELD_WIDTH  BLOCK_LENGTH * 10
#define PLAYFIELD_HEIGHT BLOCK_LENGTH * 20

#define PLAYFIELD_X (WINDOW_WIDTH  -  PLAYFIELD_WIDTH) / 2
#define PLAYFIELD_Y (WINDOW_HEIGHT - PLAYFIELD_HEIGHT) / 2

typedef enum TetrominoType {
    TETROMINO_O,
    TETROMINO_L,
    TETROMINO_J,
    TETROMINO_S,
    TETROMINO_Z,
    TETROMINO_T,
    TETROMINO_I
} TetrominoType;

typedef struct Tetromino {
    int x;
    int y;

    TetrominoType type;

    int shape[16];
} Tetromino;

typedef struct Playfield {
    int board[200];

    unsigned int lines_cleared;
    unsigned int score;

    double piece_fall_timer;

    double piece_horizontal_move_timer;
    double piece_fast_fall_timer;
} Playfield;

static Tetromino tetromino_rotate_counterclockwise(Tetromino tetromino);
static Tetromino tetromino_rotate_clockwise(Tetromino tetromino);

static void draw_playfield(Playfield playfield, Tetromino tetromino);

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

Tetromino tetromino_rotate_counterclockwise(Tetromino *tetromino)
{
    switch (tetromino->type) {
    case TETROMINO_O:
    {
        int shape[16] = {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
        int shape = {
            tetromino->shape[2], tetromino->shape[6], tetromino->shape[10], 0,
            tetromino->shape[1], tetromino->shape[5], tetromino->shape[9],  0,
            tetromino->shape[0], tetromino->shape[4], tetromino->shape[8],  0,
            0,                   0,                   0,                    0
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_I:
    {
        int shape = {
            tetromino->shape[3], tetromino->shape[7], tetromino->shape[11], tetromino->shape[15],
            tetromino->shape[2], tetromino->shape[6], tetromino->shape[10], tetromino->shape[14],
            tetromino->shape[1], tetromino->shape[5], tetromino->shape[9],  tetromino->shape[13],
            tetromino->shape[0], tetromino->shape[4], tetromino->shape[8],  tetromino->shape[12]
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
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

void draw_playfield(Playfield playfield, Tetromino tetromino)
{
    ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);

    for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 20; j++) {
                if (playfield.board[i + 10 * j]) {
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

void update_playfield(Playfield *playfield, Tetromino *tetromino, Input *input, double delta_time)
{
    if (playfield->piece_fall_timer <= 0.0) {
        tetromino->y++;
        playfield->piece_fall_timer = 1.0;
    }

    if (input->left_down && playfield->piece_horizontal_move_timer <= 0) {
        tetromino->x--;
        playfield->piece_horizontal_move_timer = 0.05;
    }

    if (input->right_down && playfield->piece_horizontal_move_timer <= 0) {
        tetromino->x++;
        playfield->piece_horizontal_move_timer = 0.05;
    }

    if (input->down_down && playfield->piece_fast_fall_timer <= 0) {
        tetromino->y++;
        playfield->piece_fast_fall_timer = 0.05;
        playfield->piece_fall_timer = 1.0;
    }

    if (input->rotate_clockwise_pressed) {
        tetromino_rotate_clockwise(tetromino);
    }

    if (input->rotate_counterclockwise_pressed) {
        tetromino_rotate_counterclockwise(tetromino);
    }


    if (playfield->piece_fall_timer > 0) {
        playfield->piece_fall_timer -= delta_time;
    }

    if (playfield->piece_horizontal_move_timer > 0) {
        playfield->piece_horizontal_move_timer -= delta_time;
    }

    if (playfield->piece_fast_fall_timer > 0) {
        playfield->piece_fast_fall_timer -= delta_time;
    }
}

GameMode game_playfield(AllegroContext *allegro, Input *input)
{
    double current_time;
    double last_time = al_get_time();

    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    Playfield playfield = {
        .board = {0},
        .lines_cleared = 0,
        .score = 0,

        .piece_fall_timer = 1.0,

        .piece_horizontal_move_timer = 0.0,
        .piece_fast_fall_timer = 0.0
    };

    Tetromino tetromino = tetromino_table[6];

    while (1) {
        double current_time = al_get_time();
        double delta_time = current_time - last_time;

        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || input->escape_pressed) {
            return MODE_EXIT;
        }

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            update_playfield(&playfield, &tetromino, input, delta_time);

            al_clear_to_color(black);
            draw_playfield(playfield, tetromino);

            allegro->redraw = 1;
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            al_flip_display();
            allegro->redraw = 0;
        }

        last_time = current_time;
    }

    return MODE_EXIT;
}
