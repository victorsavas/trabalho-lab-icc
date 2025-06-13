#include <stdlib.h>
#include "game.h"

#define BLOCK_LENGTH 30

#define PLAYFIELD_WIDTH  BLOCK_LENGTH * 10
#define PLAYFIELD_HEIGHT BLOCK_LENGTH * 20

#define PLAYFIELD_X (WINDOW_WIDTH  -  PLAYFIELD_WIDTH) / 2
#define PLAYFIELD_Y (WINDOW_HEIGHT - PLAYFIELD_HEIGHT) / 2

typedef enum TetrominoType {
    TETROMINO_I=0,
    TETROMINO_J,
    TETROMINO_L,
    TETROMINO_O,
    TETROMINO_S,
    TETROMINO_T,
    TETROMINO_Z
} TetrominoType;

// Tabelas de referência para o algorítmo SRS de rotação.
// Rotações positivas são em sentido horário
// Os índices correspondem à variável state do tetrominó

static const int wall_kick_tests_3x3[4][4][2] = {
    // 0 >> 1 (+) ou 1 >> 0 (-)
    {
        {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}
    },

    // 1 >> 2 (+) ou 2 >> 1 (-)
    {
        {1, 0}, {1, -1}, {0, 2}, {1, 2}
    },

    // 2 >> 3 (+) ou 3 >> 2 (-)
    {
        {1, 0}, {1, 1}, {0, -2}, {1, -2}
    },

    // 3 >> 0 (+) ou 0 >> 3 (-)
    {
        {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}
    }
};

static const int wall_kick_tests_i[4][4][2] = {
    // 0 >> 1 (+) ou 1 >> 0 (-)
    {
        {-2, 0}, {1, 0}, {-2, -1}, {1, 2}
    },

    // 1 >> 2 (+) ou 2 >> 1 (-)
    {
        {-1, 0}, {2, 0}, {-1, 2}, {2, -1}
    },

    // 2 >> 3 (+) ou 3 >> 2 (-)
    {
        {2, 0}, {-1, 0}, {2, 1}, {-1, -2}
    },

    // 3 >> 0 (+) ou 0 >> 3 (-)
    {
        {1, 0}, {-2, 0}, {1, -2}, {-2, 1}
    }
};

typedef struct Tetromino {
    int x;
    int y;

    TetrominoType type;
    int state;

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

static Tetromino tetromino_rotate_counterclockwise(Tetromino *tetromino);
static Tetromino tetromino_rotate_clockwise(Tetromino *tetromino);

static int tetromino_collision_check(Playfield *playfield, Tetromino tetromino, int x_offset, int y_offset);
static void tetromino_next(Playfield *playfield, Tetromino *tetromino);

static void tetromino_wall_kick(Playfield *playfield, Tetromino *tetromino, Tetromino updated_tetromino, int rotation);

static void draw_playfield(Playfield playfield, Tetromino tetromino);

static const Tetromino tetromino_table[7] = {

    {
        .type = TETROMINO_I,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            0,0,0,0,
            1,1,1,1,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_J,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            1,0,0,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_L,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            0,0,1,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_O,

        .x = 4,
        .y = -1,

        .state = 0,

        .shape = {
            1,1,0,0,
            1,1,0,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_S,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            0,1,1,0,
            1,1,0,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_T,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            0,1,0,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_Z,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            1,1,0,0,
            0,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    }
};

Tetromino tetromino_rotate_counterclockwise(Tetromino *tetromino)
{
    tetromino->state = (tetromino->state + 3) % 4;

    switch (tetromino->type) {
    case TETROMINO_O:
    {
        return;
    }
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
        int shape[] = {
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
        int shape[] = {
            tetromino->shape[3], tetromino->shape[7], tetromino->shape[11], tetromino->shape[15],
            tetromino->shape[2], tetromino->shape[6], tetromino->shape[10], tetromino->shape[14],
            tetromino->shape[1], tetromino->shape[5], tetromino->shape[9],  tetromino->shape[13],
            tetromino->shape[0], tetromino->shape[4], tetromino->shape[8],  tetromino->shape[12]
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    }
}

Tetromino tetromino_rotate_clockwise(Tetromino *tetromino)
{
    tetromino->state = (tetromino->state + 1) % 4;

    switch (tetromino->type) {
    case TETROMINO_O:
    {
        return;
    }
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
        int shape[] = {
            tetromino->shape[8],  tetromino->shape[4], tetromino->shape[0],  0,
            tetromino->shape[9],  tetromino->shape[5], tetromino->shape[1],  0,
            tetromino->shape[10], tetromino->shape[6], tetromino->shape[2],  0,
            0,                    0,                   0,                    0
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_I:
    {
        int shape[] = {
            tetromino->shape[12], tetromino->shape[8], tetromino->shape[4], tetromino->shape[0],
            tetromino->shape[13], tetromino->shape[9], tetromino->shape[5], tetromino->shape[1],
            tetromino->shape[14], tetromino->shape[10],  tetromino->shape[6], tetromino->shape[2],
            tetromino->shape[15], tetromino->shape[11],  tetromino->shape[7], tetromino->shape[3]
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    }
}

int tetromino_collision_check(Playfield *playfield, Tetromino tetromino, int x_offset, int y_offset)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino.shape[i + j * 4] == 0) {
                continue;
            }

            int x = tetromino.x + i + x_offset;
            int y = tetromino.y + j + y_offset;

            if (x < 0 || x >= 10) {
                return 1;
            }

            if (y < 0) {
                continue;
            }

            if (y >= 20) {
                return 1;
            }

            if (playfield->board[x + y * 10] != 0) {
                return 1;
            }
        }
    }

    return 0;
}

void tetromino_next(Playfield *playfield, Tetromino *tetromino)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int color = tetromino->shape[i + j * 4];

            if (color == 0) {
                continue;
            }

            int x = tetromino->x + i;
            int y = tetromino->y + j;

            playfield->board[x + y * 10] = color;
        }
    }

    *tetromino = tetromino_table[1];
}

void tetromino_wall_kick(Playfield *playfield, Tetromino *tetromino, Tetromino updated_tetromino, int rotation)
{
    if (tetromino->type == TETROMINO_O) {
        return;
    }

    if (!tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
        *tetromino = updated_tetromino;
        return;
    }

    int index;

    if (rotation == 1) {
        index = tetromino->state;
    } else {
        index = updated_tetromino.state;
    }

    const int (*tests)[2];

    if (tetromino->type == TETROMINO_I) {
        tests = wall_kick_tests_i[index];
    } else {
        tests = wall_kick_tests_3x3[index];
    }

    for (int i = 0; i < 4; i++) {
        int x_offset = tests[i][0] * rotation;
        int y_offset = tests[i][1] * rotation;

        if (!tetromino_collision_check(playfield, updated_tetromino, x_offset, y_offset)) {
            *tetromino = updated_tetromino;

            tetromino->x += x_offset;
            tetromino->y += y_offset;

            return;
        }
    }
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
    int x_move = 0;
    int y_move = 0;

    int rotation = 0;

    Tetromino updated_tetromino = *tetromino;

    // Cheque do movimento vertical

    if (playfield->piece_fall_timer <= 0.0) {
        updated_tetromino.y++;
        playfield->piece_fall_timer = 1.0;
        y_move = 1;
    }

    if ((input->down_down && playfield->piece_fast_fall_timer <= 0) || input->down_pressed) {
        updated_tetromino.y++;
        playfield->piece_fast_fall_timer = 0.05;
        playfield->piece_fall_timer = 1.0;
        y_move = 1;
    }

    if (y_move) {
        if (tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
            tetromino_next(playfield, tetromino);
            return;
        }
    }

    // Cheque do movimento horizontal

    if ((input->left_down && playfield->piece_horizontal_move_timer <= 0) || input->left_pressed) {
        updated_tetromino.x--;
        playfield->piece_horizontal_move_timer = 0.05;

        x_move--;
    }

    if ((input->right_down && playfield->piece_horizontal_move_timer <= 0) || input->right_pressed) {
        updated_tetromino.x++;
        playfield->piece_horizontal_move_timer = 0.05;

        x_move++;
    }

    if (x_move != 0) {
        if (tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
            updated_tetromino.x -= x_move;
        }
    }

    *tetromino = updated_tetromino;

    // Cheque da rotação

    if (input->z_pressed) {
        tetromino_rotate_clockwise(&updated_tetromino);

        rotation++;
    }

    if (input->x_pressed) {
        tetromino_rotate_counterclockwise(&updated_tetromino);

        rotation--;
    }

    if (rotation) {
        tetromino_wall_kick(playfield, tetromino, updated_tetromino, rotation);
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

    Tetromino tetromino = tetromino_table[2];

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
            input_pressed_flush(input);

            allegro->redraw = 1;
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            al_clear_to_color(black);
            draw_playfield(playfield, tetromino);

            al_flip_display();
            allegro->redraw = 0;
        }

        last_time = current_time;
    }

    return MODE_EXIT;
}
