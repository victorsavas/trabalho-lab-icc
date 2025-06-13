#include "game.h"

void game_loop(AllegroContext *allegro, Input *input)
{
    GameMode mode = MODE_PLAYFIELD;

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
}
