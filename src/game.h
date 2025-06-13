#pragma once

#include "allegro_context.h"
#include "input.h"

typedef enum GameMode {
    MODE_EXIT=0,
    MODE_MAIN_MENU,
    MODE_PLAYFIELD
} GameMode;

void game_loop(AllegroContext *allegro, Input *input);

GameMode game_main_menu(AllegroContext *allegro, Input *input);
GameMode game_playfield(AllegroContext *allegro, Input *input);
