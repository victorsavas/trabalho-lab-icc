#include "allegro_context.h"               // contém todos os includes do allegro
#include "gameloop_subroutines.h"
#include "game_loop.h"

int menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev){

    ALLEGRO_FONT *font = al_load_ttf_font("../../fontes/PixelifySans-SemiBold.ttf", 50, 0);
    if (!font) {
        printf("Erro ao carregar fonte.\n");
        return -1;
    }

    int running = 1;
    int redraw = 0;
    int selected = 0;

    ALLEGRO_COLOR cor = al_map_rgb(40,100,50);
    ALLEGRO_COLOR cor_select = al_map_rgb(20,50,25);

    t_button difficulty_button[3];

    difficulty_button[0] = (t_button){WIDTH/5 - WIDTH/64, HEIGHT/4, WIDTH/5 - WIDTH/64 + WIDTH/5, HEIGHT - HEIGHT/4, 0};
    difficulty_button[1] = (t_button){WIDTH/5 + WIDTH/5, HEIGHT/4, 3 * WIDTH/5, HEIGHT - HEIGHT/4, 0};
    difficulty_button[2] = (t_button){WIDTH/5 + WIDTH/64 + (2 * WIDTH/5), HEIGHT/4, WIDTH/5 + WIDTH/64 + (3 * WIDTH/5), HEIGHT - HEIGHT/4, 0};

    while(running == 1){

    redraw = 1;

    al_wait_for_event(queue, ev);   // Aguarda próximo evento na fila

        switch (ev->type) {

            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                running = 0;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)  // termina o jogo
                    running = 0;
                if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT){ // termina o jogo

                    difficulty_button[selected].selected = 0;
                    if(--selected < 0) selected = 2;
                    difficulty_button[selected].selected = 1;
                }
                if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT){ // termina o jogo
                    difficulty_button[selected].selected = 0;
                    if(++selected > 2) selected = 0;
                    difficulty_button[selected].selected = 1;
                }
                if (ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_ENTER){ // termina o jogo

                    if(difficulty_button[0].selected == 1)
                    game_loop(queue, timer, ev, font, 0.5); // executa o jogo;

                    if(difficulty_button[1].selected == 1)
                    game_loop(queue, timer, ev, font, 0.2); // executa o jogo;

                    if(difficulty_button[2].selected == 1)
                    game_loop(queue, timer, ev, font, 0.1); // executa o jogo;
                    }
                break;

            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo
                redraw = 1; // Marca que a tela precisa ser redesenhada
                break;
        }

        if (redraw == 1 && al_is_event_queue_empty(queue)) { // Redesenho da tela
            redraw = 0;

            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            if(difficulty_button[0].selected == 0)
            al_draw_filled_rounded_rectangle(difficulty_button[0].origin_x, difficulty_button[0].origin_y, difficulty_button[0].end_x, difficulty_button[0].end_y, 2, 2, cor);
            else if(difficulty_button[0].selected == 1)
            al_draw_filled_rounded_rectangle(difficulty_button[0].origin_x, difficulty_button[0].origin_y, difficulty_button[0].end_x, difficulty_button[0].end_y, 2, 2, cor_select);

            al_draw_text(font, al_map_rgb(255,255,255), difficulty_button[0].origin_x + WIDTH/10, difficulty_button[0].origin_y * 2, ALLEGRO_ALIGN_CENTER,"MODO FACIL");

            if(difficulty_button[1].selected == 0)
            al_draw_filled_rounded_rectangle(difficulty_button[1].origin_x, difficulty_button[1].origin_y, difficulty_button[1].end_x, difficulty_button[1].end_y, 2, 2, cor);
            else if(difficulty_button[1].selected == 1)
            al_draw_filled_rounded_rectangle(difficulty_button[1].origin_x, difficulty_button[1].origin_y, difficulty_button[1].end_x, difficulty_button[1].end_y, 2, 2, cor_select);

            al_draw_text(font, al_map_rgb(255,255,255), difficulty_button[1].origin_x + WIDTH/10, difficulty_button[1].origin_y * 2, ALLEGRO_ALIGN_CENTER,"MODO MEDIO");

            if(difficulty_button[2].selected == 0)
            al_draw_filled_rounded_rectangle(difficulty_button[2].origin_x, difficulty_button[2].origin_y, difficulty_button[2].end_x, difficulty_button[2].end_y, 2, 2, cor);
            else if(difficulty_button[2].selected == 1)
            al_draw_filled_rounded_rectangle(difficulty_button[2].origin_x, difficulty_button[2].origin_y, difficulty_button[2].end_x, difficulty_button[2].end_y, 2, 2, cor_select);

            al_draw_text(font, al_map_rgb(255,255,255), difficulty_button[2].origin_x + WIDTH/10, difficulty_button[2].origin_y * 2, ALLEGRO_ALIGN_CENTER,"MODO HARD");

            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }
}

