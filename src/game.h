#pragma once

#include "allegro_context.h"
#include "input.h"

typedef enum GameMode {
    MODE_EXIT=0,
    MODE_MAIN_MENU,
    MODE_PLAYFIELD
} GameMode;

typedef struct t_button {
    int origin_x;
    int origin_y;

    int end_x;
    int end_y;

    char *label;

    ALLEGRO_COLOR default_color;
    ALLEGRO_COLOR selected_color;
} t_button;

#define BACKGROUND_COLOR al_map_rgb(12, 8, 42)
#define WHITE            al_map_rgb(255, 255, 255)

void button_draw(AllegroContext* allegro, t_button button, int selected);

void game_loop(AllegroContext *allegro, Input *input);

GameMode game_main_menu(AllegroContext *allegro, Input *input);
GameMode game_playfield(AllegroContext *allegro, Input *input);
