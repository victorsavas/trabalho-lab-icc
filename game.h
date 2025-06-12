#pragma once

#include "allegro_context.h"
#include "input.h"

typedef enum GameMode {
    MODE_EXIT=0,
    MODE_MAIN_MENU,
    MODE_PLAYFIELD
} GameMode;

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

void game_loop(AllegroContext *allegro);

GameMode game_main_menu(AllegroContext *allegro, Input *input);
GameMode game_playfield(AllegroContext *allegro, Input *input);
