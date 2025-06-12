#include "game.h"

void game_loop(AllegroContext *allegro)
{
    GameMode mode = MODE_PLAYFIELD;
    Input *input = input_init();

    while (mode) {
        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || input->escape_pressed) {
            break;
        }

        switch (mode) {
        case MODE_MAIN_MENU:
            mode = game_main_menu(allegro, input);
            break;
        case MODE_PLAYFIELD:
            mode = game_playfield(allegro, input);
            break;
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            al_flip_display();
            allegro->redraw = 0;
        }
    }

    input_free(input);
}
