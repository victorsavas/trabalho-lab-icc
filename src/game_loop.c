#include "game.h"

void game_loop(AllegroContext *allegro)
{
    GameMode mode = MODE_PLAYFIELD;
    Input *input = input_init();

    while (mode) {
        switch (mode) {
        case MODE_MAIN_MENU:
            mode = game_main_menu(allegro, input);
            break;
        case MODE_PLAYFIELD:
            mode = game_playfield(allegro, input);
            break;
        }
    }

    input_free(input);
}
