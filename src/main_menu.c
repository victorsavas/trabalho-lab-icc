#include "allegro_context.h"
#include "game.h"
#include "global_state.h"

// Subrotina subordinada de game_main_menu. Mostra o placar na tela.

static GameMode show_leaderboard(t_leaderboard *leaderboard, AllegroContext *allegro, Input *input);

// Implementação das subrotinas

GameMode game_main_menu(GlobalState *global, AllegroContext *allegro, Input* input)
{
    // Variáveis para o cálculo do tempo delta_time entre cada frame

    double current_time;
    double last_time = al_get_time();
    double delta_time;

    // Índice do botão selecionado

    // 0: BOTÃO EASY
    // 1: BOTÃO NORMAL
    // 2: BOTÃO HARD
    // 3: BOTÃO LEADERBOARD

    int selected = 0;

    // Botões de dificuldade

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

    // Botão do placar

    t_button leaderboard_button = {
        .origin_x = WIDTH/5 + WIDTH/10,
        .origin_y = HEIGHT - HEIGHT/5,

        .end_x = 3 * WIDTH/5 + WIDTH/10,
        .end_y = HEIGHT - HEIGHT/5 + HEIGHT/10,

        .label = "LEADERBOARD",

        .default_color  = al_map_rgb(38, 17, 82),
        .selected_color = al_map_rgb(72, 118, 187)
    };

    // Loop do menu principal

    while (1) {
        // Recebe o próximo evento da fila e recebe as entradas do teclado

        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        // Fecha o jogo

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || input->escape_pressed) {
            return MODE_EXIT;
        }

        // Lógica de cada frame

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            // Cálculo do delta_time

            current_time = al_get_time();
            delta_time = current_time - last_time;

            // Fullscreen

            if (input->f4_pressed) {
                toggle_fullscreen(allegro);
            }

            // Move a seleção do botão

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

            // Pressiona o botão selecionado

            if (input->space_pressed || input->z_pressed || input->x_pressed) {
                if (selected != 3) {
                    // Seleciona a dificuldade e altera o modo para playfield

                    input_pressed_flush(input);
                    global->selected_difficulty = selected;
                    return MODE_PLAYFIELD;
                } else {
                    // Exibe o placar na tela

                    GameMode mode = show_leaderboard(global->leaderboard, allegro, input);

                    if (mode == MODE_EXIT) {
                        return MODE_EXIT;
                    }
                }
            }

            // Indica que deve-se redesenhar a tela, atualiza as entradas de teclado e registra o tempo do frame

            input_pressed_flush(input);

            allegro->redraw = 1;

            last_time = current_time;
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            // Limpa o fundo

            al_clear_to_color(BACKGROUND_COLOR);

            // Desenha os botões

            for (int i = 0; i < 3; i++) {
                button_draw(allegro, difficulty_button[i], selected == i);
            }

            button_draw(allegro, leaderboard_button, selected == 3);

            // Redesenha a tela

            al_flip_display();
            allegro->redraw = 0;
        }
    }
}

GameMode show_leaderboard(t_leaderboard *leaderboard, AllegroContext *allegro, Input *input)
{
    // Loop do placar

    while (1) {
        // Recebe o próximo evento da fila e recebe as entradas do teclado

        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        // Retorna ao menu principal caso ESCAPE seja pressionado

        if (input->escape_pressed) {
            input_pressed_flush(input);
            return MODE_MAIN_MENU;
        }

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            return MODE_EXIT;
        }

        // Lógica de cada frame

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {

            // Fullscreen

            if (input->f4_pressed) {
                toggle_fullscreen(allegro);
            }

            // Atualiza as entradas e indica que deve-se redesenhar atela

            input_pressed_flush(input);
            allegro->redraw = 1;
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            allegro->redraw = 0;

            // Limpa o fundo

            al_clear_to_color(BACKGROUND_COLOR);

            // Bordas do placar

            al_draw_filled_rounded_rectangle(
                (WIDTH/8 - WIDTH/18) * allegro->scale + allegro->x_offset,
                HEIGHT/8 * allegro->scale + allegro->y_offset,
                (WIDTH - WIDTH/8 + WIDTH/18) * allegro->scale + allegro->x_offset,
                (HEIGHT - HEIGHT/8 - HEIGHT/10) * allegro->scale + allegro->y_offset,
                20,
                20,
                al_map_rgb(72, 118, 187)
            );

            al_draw_filled_rounded_rectangle(
                (WIDTH/7 - WIDTH/18) * allegro->scale + allegro->x_offset,
                (HEIGHT/6) * allegro->scale + allegro->y_offset,
                (WIDTH - WIDTH/7 + WIDTH/18) * allegro->scale + allegro->x_offset,
                (HEIGHT - HEIGHT/6 - HEIGHT/10) * allegro->scale + allegro->y_offset,
                20,
                20,
                BACKGROUND_COLOR
            );

            // Desenha as pontuações salvas

            for(int j=0; j<DIFFICULTY_LEVELS; j++){
                for(int i=0; i<TOP_RANKING; i++){
                    char *name = leaderboard[j].entries[i].name;
                    int score = leaderboard[j].entries[i].score;

                    if (score != 0) {
                        al_draw_textf(
                            allegro->font_small,
                            WHITE,
                            (WIDTH/4 + j * WIDTH/4) * allegro->scale + allegro->x_offset,
                            (HEIGHT/4 + HEIGHT/10 * i) * allegro->scale + allegro->y_offset,
                            ALLEGRO_ALIGN_CENTER,
                            "%s %06d", name, score
                        );
                    } else {
                        al_draw_textf(
                            allegro->font_small,
                            WHITE,
                            (WIDTH/4 + j * WIDTH/4) * allegro->scale + allegro->x_offset,
                            (HEIGHT/4 + HEIGHT/10 * i) * allegro->scale + allegro->y_offset,
                            ALLEGRO_ALIGN_CENTER,
                            "-"
                        );
                    }
                }
            }

            // Nomes das dificuldades

            al_draw_textf(
                allegro->font,
                al_map_rgb(81, 179, 65),
                (WIDTH/4 + 0 * WIDTH/4) * allegro->scale + allegro->x_offset,
                (HEIGHT/5) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "EASY"
            );

            al_draw_textf(
                allegro->font,
                al_map_rgb(255, 127, 79),
                (WIDTH/4 + 1 * WIDTH/4) * allegro->scale + allegro->x_offset,
                (HEIGHT/5) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "NORMAL"
            );

            al_draw_textf(
                allegro->font,
                al_map_rgb(238, 48, 70),
                (WIDTH/4 + 2 * WIDTH/4) * allegro->scale + allegro->x_offset,
                (HEIGHT/5) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "HARD"
            );

            // Título do placar

            al_draw_textf(
                allegro->font,
                WHITE,
                (WIDTH/2) * allegro->scale + allegro->x_offset,
                (HEIGHT/20) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "LEADERBOARD"
            );

            // Mensagem informando como retornar ao menu principal

            al_draw_textf(
                allegro->font,
                WHITE,
                (WIDTH/2) * allegro->scale + allegro->x_offset,
                (HEIGHT - HEIGHT/6) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "PRESS ESCAPE TO RETURN"
            );

            // Atualiza a tela com o que foi desenhado

            al_flip_display();
        }
    }

    return MODE_EXIT;
}
