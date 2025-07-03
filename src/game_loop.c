#include "game.h"
#include "global_state.h"

int difficulty = 0;
t_topfive_leaderboard difficulty_entries[DIFFICULTY_LEVELS] = {0};

void button_draw(AllegroContext* allegro, t_button button, int selected)
{
    ALLEGRO_COLOR color;

    if (selected) {
        color = button.selected_color;
    } else {
        color = button.default_color;
    }

    al_draw_filled_rectangle(
        button.origin_x,
        button.origin_y,
        button.end_x,
        button.end_y,
        color
    );

    al_draw_text(
        allegro->font,
        WHITE,
        (button.origin_x + button.end_x) / 2,
        (button.origin_y + button.end_y) / 2 - 25,
        ALLEGRO_ALIGN_CENTER,
        button.label
    );
}

void game_loop(AllegroContext *allegro, Input *input)
{
    GameMode mode = MODE_MAIN_MENU;

    int error_code = read_leaderboard_file();

    if (error_code) {
        return;
    }

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

    save_leaderboard_to_file();
}
