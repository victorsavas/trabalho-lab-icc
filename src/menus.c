#include "allegro_context.h"               // contém todos os includes do allegro
#include "gameloop_subroutines.h"
#include "game_loop.h"

int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev, ALLEGRO_FONT *font, ALLEGRO_FONT *font_small){

    int running = 1;
    int redraw = 0;
    int selected = 0;

    ALLEGRO_COLOR cor = al_map_rgb(40,100,40);
    ALLEGRO_COLOR cor_select = al_map_rgb(40,100,40);

    t_button difficulty_button[3];
    difficulty_button[0] = (t_button){WIDTH/5 - WIDTH/64, HEIGHT/5, WIDTH/5 - WIDTH/64 + WIDTH/5, HEIGHT - HEIGHT/4, 0};
    difficulty_button[1] = (t_button){WIDTH/5 + WIDTH/5, HEIGHT/5, 3 * WIDTH/5, HEIGHT - HEIGHT/4, 0};
    difficulty_button[2] = (t_button){WIDTH/5 + WIDTH/64 + (2 * WIDTH/5), HEIGHT/5, WIDTH/5 + WIDTH/64 + (3 * WIDTH/5), HEIGHT - HEIGHT/4, 0};

    difficulty_button[0].default_color = al_map_rgb(40,130,40);
    difficulty_button[0].selected_color = al_map_rgb(20,65,20);

    difficulty_button[1].default_color = al_map_rgb(190,160,20);
    difficulty_button[1].selected_color = al_map_rgb(95,80,10);

    difficulty_button[2].default_color = al_map_rgb(150,30,30);
    difficulty_button[2].selected_color = al_map_rgb(75,15,15);

    t_button other_button[2];
    other_button[0] = (t_button){WIDTH/5 + WIDTH/10, HEIGHT - HEIGHT/5, 3 * WIDTH/5 + WIDTH/10, HEIGHT - HEIGHT/5 + HEIGHT/10, 0};
    other_button[1] = (t_button){WIDTH/5 + WIDTH/5, HEIGHT/4, 3 * WIDTH/5, HEIGHT - HEIGHT/4, 0};

    other_button[0].default_color = al_map_rgb(30,30,150);
    other_button[0].selected_color = al_map_rgb(15,15,75);

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
                if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT){
                    if(other_button[selected].selected == 0){
                    difficulty_button[selected].selected = 0;
                    if(--selected < 0) selected = 2;
                    difficulty_button[selected].selected = 1;
                    }
                }
                if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT){
                    if(other_button[selected].selected == 0){
                    difficulty_button[selected].selected = 0;
                    if(++selected > 2) selected = 0;
                    difficulty_button[selected].selected = 1;
                    }
                }
                if (ev->keyboard.keycode == ALLEGRO_KEY_DOWN){
                    difficulty_button[selected].selected = 0;
                    selected = 0;
                    other_button[selected].selected = 1;
                }
                if (ev->keyboard.keycode == ALLEGRO_KEY_UP){
                    if(other_button[selected].selected == 1){
                    other_button[selected].selected = 0;
                    selected = 1;
                    difficulty_button[selected].selected = 1;
                    }
                }
                if (ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_ENTER
                    || ev->keyboard.keycode == ALLEGRO_KEY_Z || ev->keyboard.keycode == ALLEGRO_KEY_X){

                    if(difficulty_button[0].selected == 1)
                    game_loop(queue, timer, ev, font, 0.5); // executa o jogo;

                    if(difficulty_button[1].selected == 1)
                    game_loop(queue, timer, ev, font, 0.2); // executa o jogo;

                    if(difficulty_button[2].selected == 1)
                    game_loop(queue, timer, ev, font, 0.1); // executa o jogo;

                    if(other_button[selected].selected == 1)
                    leaderboard_menu(queue, timer, ev, font, font_small);
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
            al_draw_filled_rounded_rectangle(difficulty_button[0].origin_x, difficulty_button[0].origin_y, difficulty_button[0].end_x, difficulty_button[0].end_y, 2, 2, difficulty_button[0].default_color);
            else if(difficulty_button[0].selected == 1)
            al_draw_filled_rounded_rectangle(difficulty_button[0].origin_x, difficulty_button[0].origin_y, difficulty_button[0].end_x, difficulty_button[0].end_y, 2, 2, difficulty_button[0].selected_color);

            al_draw_text(font, al_map_rgb(255,255,255), difficulty_button[0].origin_x + WIDTH/10,
                        ((difficulty_button[0].end_y + difficulty_button[0].origin_y) / 2) - difficulty_button[0].origin_y / 10, ALLEGRO_ALIGN_CENTER,"EASY");

            if(difficulty_button[1].selected == 0)
            al_draw_filled_rounded_rectangle(difficulty_button[1].origin_x, difficulty_button[1].origin_y, difficulty_button[1].end_x, difficulty_button[1].end_y, 2, 2, difficulty_button[1].default_color);
            else if(difficulty_button[1].selected == 1)
            al_draw_filled_rounded_rectangle(difficulty_button[1].origin_x, difficulty_button[1].origin_y, difficulty_button[1].end_x, difficulty_button[1].end_y, 2, 2, difficulty_button[1].selected_color);

            al_draw_text(font, al_map_rgb(255,255,255), difficulty_button[1].origin_x + WIDTH/10,
                        ((difficulty_button[1].end_y + difficulty_button[1].origin_y) / 2) - difficulty_button[1].origin_y / 10, ALLEGRO_ALIGN_CENTER,"MEDIUM");

            if(difficulty_button[2].selected == 0)
            al_draw_filled_rounded_rectangle(difficulty_button[2].origin_x, difficulty_button[2].origin_y, difficulty_button[2].end_x, difficulty_button[2].end_y, 2, 2, difficulty_button[2].default_color);
            else if(difficulty_button[2].selected == 1)
            al_draw_filled_rounded_rectangle(difficulty_button[2].origin_x, difficulty_button[2].origin_y, difficulty_button[2].end_x, difficulty_button[2].end_y, 2, 2, difficulty_button[2].selected_color);

            al_draw_text(font, al_map_rgb(255,255,255), difficulty_button[2].origin_x + WIDTH/10,
                        ((difficulty_button[2].end_y + difficulty_button[2].origin_y) / 2) - difficulty_button[2].origin_y / 10, ALLEGRO_ALIGN_CENTER,"HARD");

            if(other_button[0].selected == 0)
            al_draw_filled_rounded_rectangle(other_button[0].origin_x, other_button[0].origin_y, other_button[0].end_x, other_button[0].end_y, 2, 2, other_button[0].default_color);
            else if(other_button[0].selected == 1)
            al_draw_filled_rounded_rectangle(other_button[0].origin_x, other_button[0].origin_y, other_button[0].end_x, other_button[0].end_y, 2, 2, other_button[0].selected_color);

            al_draw_text(font, al_map_rgb(255,255,255), (other_button[0].end_x - other_button[0].origin_x) /2 + other_button[0].origin_x,
                        ((other_button[0].end_y + other_button[0].origin_y) / 2) - other_button[0].origin_y / 30, ALLEGRO_ALIGN_CENTER,"LEADERBOARD");

            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }
}

int leaderboard_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev,  ALLEGRO_FONT *font, ALLEGRO_FONT *font_small){

    int running = 1;
    int redraw = 0;

    t_leaderboard_entry leaderboard_entries[5];

    leaderboard_entries[0] = (t_leaderboard_entry) {0, 200,{'A','B','C','\0'}};
    leaderboard_entries[1] = (t_leaderboard_entry) {0, 200000,{'A','D','F','B','C','\0'}};
    leaderboard_entries[2] = (t_leaderboard_entry) {0, 200234,{'B','J','I','\0'}};
    leaderboard_entries[3] = (t_leaderboard_entry) {0, 1,{'V','2','L','\0'}};
    leaderboard_entries[4] = (t_leaderboard_entry) {0, 1,{'V','2','L','\0'}};

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

            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            al_draw_filled_rounded_rectangle(WIDTH/8, HEIGHT/8, WIDTH - WIDTH/8, HEIGHT - HEIGHT/8, 20, 20, al_map_rgb(50,50,150));

            al_draw_filled_rounded_rectangle(WIDTH/7, HEIGHT/6, WIDTH - WIDTH/7, HEIGHT - HEIGHT/6, 20, 20, al_map_rgb(10,10,10));

            for(int i=0; i<5; i++){
            al_draw_textf(font_small, al_map_rgb(255,255,255), WIDTH/4 + WIDTH/25, HEIGHT/5 + 50 * i,
                          ALLEGRO_ALIGN_CENTER,"%04d | %s", leaderboard_entries[i].score, leaderboard_entries[i].name);
            }
            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }
}

void loss_screen(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev, ALLEGRO_FONT *font, ALLEGRO_FONT *font_small){

    // a ser feito
}

