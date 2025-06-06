#include "game_loop.h"

typedef enum mode_e {
    EXIT,
    MENU,
    GAME
} GameMode;

typedef enum tetromino_e {
    TETROMINO_O,
    TETROMINO_L,
    TETROMINO_J,
    TETROMINO_S,
    TETROMINO_Z,
    TETROMINO_T,
    TETROMINO_I
} TetrominoType;

typedef struct tetromino_t {
    TetrominoType type;

    int x;
    int y;

    int shape[4][4];
} Tetromino;

const Tetromino tetromino_table[7] =
{
    {
        .type = TETROMINO_O,

        .x = 5,
        .y = 0,

        .shape = {
            0,1,1,0,
            0,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_L,

        .x = 4,
        .y = 0,

        .shape = {
            0,0,1,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_J,

        .x = 4,
        .y = 0,

        .shape = {
            1,0,0,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_S,

        .x = 4,
        .y = 0,

        .shape = {
            0,1,1,0,
            1,1,0,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_Z,

        .x = 4,
        .y = 0,

        .shape = {
            1,1,0,0,
            0,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_T,

        .x = 4,
        .y = 0,

        .shape = {
            0,1,0,0,
            1,1,1,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_T,

        .x = 4,
        .y = 0,

        .shape = {
            0,0,0,0,
            1,1,1,1,
            0,0,0,0,
            0,0,0,0
        }
    }
};

void game_loop(AllegroContext allegro)
{
    int redraw = 1;
    GameMode mode = GAME;

    int playfield[10][20] = {0};
    Tetromino tetromino = tetromino_table[0];

    ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);

    while (mode != EXIT) {
        al_wait_for_event(allegro.queue, &allegro.event);

        if (allegro.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        switch (mode) {
        case MENU:
            break;
        case GAME:
            al_draw_filled_rectangle(0,0,32,32,red);
            break;
        }

        al_flip_display();
    }
}
