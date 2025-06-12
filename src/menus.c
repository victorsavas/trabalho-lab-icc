#include "allegro_context.h"               // contém todos os includes do allegro
#include "gameloop_subroutines.h"
#include "game_loop.h"

int menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev){

    int running = 1;
    int redraw = 0;
    ALLEGRO_COLOR cor = al_map_rgb(40,100,50);

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
                break;

            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo
                redraw = 1; // Marca que a tela precisa ser redesenhada
                break;
        }

        if (redraw == 1 && al_is_event_queue_empty(queue)) { // Redesenho da tela
            redraw = 0;

            al_draw_filled_rounded_rectangle(WIDTH/5, HEIGHT/4, WIDTH/5 + WIDTH/6, HEIGHT - HEIGHT/4, 2, 2, cor);

            al_draw_filled_rounded_rectangle(WIDTH/2, HEIGHT/4, WIDTH/2 + WIDTH/6, HEIGHT - HEIGHT/4, 2, 2, cor);

            al_draw_filled_rounded_rectangle(WIDTH - WIDTH/5, HEIGHT/4, WIDTH - WIDTH/5 + WIDTH/6, HEIGHT - HEIGHT/4, 2, 2, cor);

            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }
}

