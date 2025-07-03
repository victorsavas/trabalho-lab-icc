#include "allegro_context.h"
#include "game.h"
#include "global_state.h"

int show_leaderboard(AllegroContext *allegro, Input *input);

GameMode game_main_menu(AllegroContext *allegro, Input* input)
{
    GameMode mode = MODE_MAIN_MENU;

    double current_time;
    double last_time = al_get_time();
    double delta_time;

    int selected = 0;

    t_button difficulty_button[3] = {
        {
            .origin_x = WIDTH/5 - WIDTH/64,
            .origin_y = HEIGHT/5,

            .end_x = WIDTH/5 - WIDTH/64 + WIDTH/5,
            .end_y = HEIGHT - HEIGHT/4,

            .label = "EASY",

            .default_color  = al_map_rgb(23, 81, 69),
            .selected_color = al_map_rgb(81, 179, 65)
        },

        {
            .origin_x = WIDTH/5 + WIDTH/5,
            .origin_y = HEIGHT/5,

            .end_x = 3 * WIDTH/5,
            .end_y = HEIGHT - HEIGHT/4,

            .label = "NORMAL",

            .default_color  = al_map_rgb(113, 40, 53),
            .selected_color = al_map_rgb(255, 127, 79)
        },

        {
            .origin_x = WIDTH/5 + WIDTH/64 + (2 * WIDTH/5),
            .origin_y = HEIGHT/5,

            .end_x = WIDTH/5 + WIDTH/64 + (3 * WIDTH/5),
            .end_y = HEIGHT - HEIGHT/4,

            .label = "HARD",

            .default_color  = al_map_rgb(98, 27, 82),
            .selected_color = al_map_rgb(238, 48, 70)
        }
    };

    t_button other_button = {
        .origin_x = WIDTH/5 + WIDTH/10,
        .origin_y = HEIGHT - HEIGHT/5,

        .end_x = 3 * WIDTH/5 + WIDTH/10,
        .end_y = HEIGHT - HEIGHT/5 + HEIGHT/10,

        .label = "LEADERBOARD",

        .default_color  = al_map_rgb(38, 17, 82),
        .selected_color = al_map_rgb(72, 118, 187)
    };

    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);

    while (mode == MODE_MAIN_MENU) {
        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || input->escape_pressed) {
            return MODE_EXIT;
        }

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            current_time = al_get_time();
            delta_time = current_time - last_time;

            if (input->up_pressed) {
                if (selected == 3) {
                    selected = 0;
                }
            }
            if (input->down_pressed) {
                selected = 3;
            }
            if (input->right_pressed) {
                if (selected != 3) {
                    selected = (selected + 1) % 3;
                }
            }
            if (input->left_pressed) {
                if (selected != 3) {
                    selected = (selected + 2) % 3;
                }
            }

            if (input->space_pressed || input->z_pressed || input->x_pressed) {
                if (selected != 3) {
                    // Seleciona a respectiva dificuldade

                    input_pressed_flush(input);
                    difficulty = selected;
                    return MODE_PLAYFIELD;
                } else {
                    show_leaderboard(allegro, input);
                }
            }

            input_pressed_flush(input);

            allegro->redraw = 1;

            last_time = current_time;
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            al_clear_to_color(BACKGROUND_COLOR);

            for (int i = 0; i < 3; i++) {
                button_draw(allegro, difficulty_button[i], selected == i);
            }

            button_draw(allegro, other_button, selected == 3);

            al_flip_display();
            allegro->redraw = 0;
        }
    }
}

int show_leaderboard(AllegroContext *allegro, Input *input)
{
    while (1) {
        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        if (input->escape_pressed) {
            return 0;
        }

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            input_pressed_flush(input);
            allegro->redraw = 1;
        }

        if (allegro->redraw) {
            allegro->redraw = 0;

            al_clear_to_color(BACKGROUND_COLOR);  // Limpa a tela com uma cor escura

            al_draw_filled_rounded_rectangle(
                WIDTH/8 - WIDTH/18,
                HEIGHT/8,
                WIDTH - WIDTH/8 + WIDTH/18,
                HEIGHT - HEIGHT/8 - HEIGHT/10,
                20,
                20,
                al_map_rgb(72, 118, 187)
            );

            al_draw_filled_rounded_rectangle(
                WIDTH/7 - WIDTH/18,
                HEIGHT/6,
                WIDTH - WIDTH/7 + WIDTH/18,
                HEIGHT - HEIGHT/6 - HEIGHT/10,
                20,
                20,
                BACKGROUND_COLOR
            );

            for(int j=0; j<DIFFICULTY_LEVELS; j++){
                for(int i=0; i<TOP_RANKING; i++){
                    al_draw_textf(
                        allegro->font,
                        WHITE,
                        WIDTH/4 + j * WIDTH/4,
                        HEIGHT/4 + HEIGHT/10 * i,
                        ALLEGRO_ALIGN_CENTER,
                        "%s %04d",
                        difficulty_entries[j].entries[i].name,
                        difficulty_entries[j].entries[i].score
                    );
                }
            }

            al_draw_textf(
                allegro->font,
                al_map_rgb(81, 179, 65),
                WIDTH/4 + 0 * WIDTH/4,
                HEIGHT/5,
                ALLEGRO_ALIGN_CENTER,
                "EASY"
            );

            al_draw_textf(
                allegro->font,
                al_map_rgb(255, 127, 79),
                WIDTH/4 + 1 * WIDTH/4,
                HEIGHT/5,
                ALLEGRO_ALIGN_CENTER,
                "NORMAL"
            );

            al_draw_textf(
                allegro->font,
                al_map_rgb(238, 48, 70),
                WIDTH/4 + 2 * WIDTH/4,
                HEIGHT/5,
                ALLEGRO_ALIGN_CENTER,
                "HARD"
            );

            al_draw_textf(
                allegro->font,
                WHITE,
                WIDTH/2,
                HEIGHT/20,
                ALLEGRO_ALIGN_CENTER,
                "LEADERBOARD"
            );

            al_draw_textf(
                allegro->font,
                WHITE,
                WIDTH/2,
                HEIGHT - HEIGHT/6,
                ALLEGRO_ALIGN_CENTER,
                "PRESS ESCAPE TO RETURN"
            );

            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }
}
