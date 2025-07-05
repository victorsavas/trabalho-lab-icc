#include "game.h"
#include "global_state.h"

void button_draw(AllegroContext* allegro, t_button button, int selected)
{
    ALLEGRO_COLOR color;

    // Determina a cor do botão

    if (selected) {
        color = button.selected_color;
    } else {
        color = button.default_color;
    }

    // Corpo do botão

    al_draw_filled_rectangle(
        button.origin_x * allegro->scale + allegro->x_offset,
        button.origin_y * allegro->scale + allegro->y_offset,
        button.end_x * allegro->scale + allegro->x_offset,
        button.end_y * allegro->scale + allegro->y_offset,
        color
    );

    // Rótulo

    al_draw_text(
        allegro->font,
        WHITE,
        ((button.origin_x + button.end_x) / 2) * allegro->scale + allegro->x_offset,
        ((button.origin_y + button.end_y) / 2 - 25) * allegro->scale + allegro->y_offset,
        ALLEGRO_ALIGN_CENTER,
        button.label
    );
}

void game_loop(GlobalState *global, AllegroContext *allegro, Input *input)
{
    // Modo do jogo

    GameMode mode = MODE_MAIN_MENU;

    // Máquina de estados

    while (mode) {
        switch (mode) {
        case MODE_MAIN_MENU:
            mode = game_main_menu(global, allegro, input);
            break;
        case MODE_PLAYFIELD:
            mode = game_playfield(global, allegro, input);
            break;
        }
    }
}
