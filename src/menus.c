
#include "allegro_context.h"               // contém todos os includes do allegro
#include "gameloop_logic.h"
#include "game_loop.h"

int main_menu(t_allegro_context allegro_contexts, t_topfive_leaderboard *difficulty_entries){

    ALLEGRO_EVENT ev = allegro_contexts.ev;

    int running = 1;
    int redraw = 0;
    int selected = 0;

    ALLEGRO_COLOR cor = al_map_rgb(40,100,40);
    ALLEGRO_COLOR cor_select = al_map_rgb(40,100,40);

    t_button difficulty_button[DIFFICULTY_LEVELS];
    difficulty_button[0] = (t_button){WIDTH/5 - WIDTH/64, HEIGHT/5, WIDTH/5 - WIDTH/64 + WIDTH/5, HEIGHT - HEIGHT/4, 0};
    difficulty_button[1] = (t_button){WIDTH/5 + WIDTH/5, HEIGHT/5, 3 * WIDTH/5, HEIGHT - HEIGHT/4, 0};
    difficulty_button[2] = (t_button){WIDTH/5 + WIDTH/64 + (2 * WIDTH/5), HEIGHT/5, WIDTH/5 + WIDTH/64 + (3 * WIDTH/5), HEIGHT - HEIGHT/4, 0};

    difficulty_button[0].default_color = al_map_rgb(40,130,40);
    difficulty_button[0].selected_color = al_map_rgb(20,65,20);

    difficulty_button[1].default_color = al_map_rgb(190,160,20);
    difficulty_button[1].selected_color = al_map_rgb(95,80,10);

    difficulty_button[2].default_color = al_map_rgb(150,30,30);
    difficulty_button[2].selected_color = al_map_rgb(75,15,15);

    t_button other_button[1];
    other_button[0] = (t_button){WIDTH/5 + WIDTH/10, HEIGHT - HEIGHT/5, 3 * WIDTH/5 + WIDTH/10, HEIGHT - HEIGHT/5 + HEIGHT/10, 0};

    other_button[0].default_color = al_map_rgb(30,30,150);
    other_button[0].selected_color = al_map_rgb(15,15,75);

    while(running == 1){

    redraw = 1;

    al_wait_for_event(allegro_contexts.queue, &ev);   // Aguarda próximo evento na fila

        switch (ev.type) {

            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                running = 0;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)  // termina o jogo
                    running = 0;
                if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT){
                    if(other_button[0].selected == 0){
                    difficulty_button[selected].selected = 0;
                    if(--selected < 0) selected = 2;
                    difficulty_button[selected].selected = 1;
                    }
                }
                if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT){
                    if(other_button[0].selected == 0){
                    difficulty_button[selected].selected = 0;
                    if(++selected > 2) selected = 0;
                    difficulty_button[selected].selected = 1;
                    }
                }
                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN){
                    difficulty_button[selected].selected = 0;
                    selected = 0;
                    other_button[selected].selected = 1;
                }
                if (ev.keyboard.keycode == ALLEGRO_KEY_UP){
                    if(other_button[selected].selected == 1){
                    other_button[selected].selected = 0;
                    selected = 1;
                    difficulty_button[selected].selected = 1;
                    }
                }
                if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE || ev.keyboard.keycode == ALLEGRO_KEY_ENTER
                    || ev.keyboard.keycode == ALLEGRO_KEY_Z || ev.keyboard.keycode == ALLEGRO_KEY_X){

                    if(difficulty_button[0].selected == 1)
                    game_loop(allegro_contexts, difficulty_entries, 0.5, 0); // executa o jogo;

                    if(difficulty_button[1].selected == 1)
                    game_loop(allegro_contexts, difficulty_entries, 0.2, 1); // executa o jogo;

                    if(difficulty_button[2].selected == 1)
                    game_loop(allegro_contexts, difficulty_entries, 0.1, 2); // executa o jogo;

                    if(other_button[selected].selected == 1)
                    leaderboard_menu(allegro_contexts, difficulty_entries);
                    }

                break;

            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo
                redraw = 1; // Marca que a tela precisa ser redesenhada
                break;
        }

        if (redraw == 1 && al_is_event_queue_empty(allegro_contexts.queue)) { // Redesenho da tela
            redraw = 0;

            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            for(int i=0; i<3; i++){
                if(difficulty_button[i].selected == 0)
                    al_draw_filled_rounded_rectangle(difficulty_button[i].origin_x, difficulty_button[i].origin_y, difficulty_button[i].end_x, difficulty_button[i].end_y, 2, 2, difficulty_button[i].default_color);
                else if(difficulty_button[i].selected == 1)
                    al_draw_filled_rounded_rectangle(difficulty_button[i].origin_x, difficulty_button[i].origin_y, difficulty_button[i].end_x, difficulty_button[i].end_y, 2, 2, difficulty_button[i].selected_color);
            }


            al_draw_text(allegro_contexts.font, al_map_rgb(255,255,255), difficulty_button[0].origin_x + WIDTH/10,
                        ((difficulty_button[0].end_y + difficulty_button[0].origin_y) / 2) - difficulty_button[0].origin_y / 10, ALLEGRO_ALIGN_CENTER,"EASY");

            al_draw_text(allegro_contexts.font, al_map_rgb(255,255,255), difficulty_button[1].origin_x + WIDTH/10,
                        ((difficulty_button[1].end_y + difficulty_button[1].origin_y) / 2) - difficulty_button[1].origin_y / 10, ALLEGRO_ALIGN_CENTER,"MEDIUM");

            al_draw_text(allegro_contexts.font, al_map_rgb(255,255,255), difficulty_button[2].origin_x + WIDTH/10,
                        ((difficulty_button[2].end_y + difficulty_button[2].origin_y) / 2) - difficulty_button[2].origin_y / 10, ALLEGRO_ALIGN_CENTER,"HARD");

            if(other_button[0].selected == 0)
            al_draw_filled_rounded_rectangle(other_button[0].origin_x, other_button[0].origin_y, other_button[0].end_x, other_button[0].end_y, 2, 2, other_button[0].default_color);
            else if(other_button[0].selected == 1)
            al_draw_filled_rounded_rectangle(other_button[0].origin_x, other_button[0].origin_y, other_button[0].end_x, other_button[0].end_y, 2, 2, other_button[0].selected_color);

            al_draw_text(allegro_contexts.font, al_map_rgb(255,255,255), (other_button[0].end_x - other_button[0].origin_x) /2 + other_button[0].origin_x,
                        ((other_button[0].end_y + other_button[0].origin_y) / 2) - other_button[0].origin_y / 30, ALLEGRO_ALIGN_CENTER,"LEADERBOARD");

            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }
}

int leaderboard_menu(t_allegro_context allegro_contexts, t_topfive_leaderboard *difficulty_entries){

    ALLEGRO_EVENT ev = allegro_contexts.ev;

    int running = 1;
    int redraw = 0;

    while(running == 1){

        redraw = 1;

        al_wait_for_event(allegro_contexts.queue, &ev);   // Aguarda próximo evento na fila

        switch (ev.type) {

            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                running = 0;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)  // termina o jogo
                    running = 0;

                break;

            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo
                redraw = 1; // Marca que a tela precisa ser redesenhada
                break;
        }

        if (redraw == 1 && al_is_event_queue_empty(allegro_contexts.queue)) { // Redesenho da tela
            redraw = 0;

            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            al_draw_filled_rounded_rectangle(WIDTH/8 - WIDTH/18, HEIGHT/8, WIDTH - WIDTH/8 + WIDTH/18, HEIGHT - HEIGHT/8 - HEIGHT/10, 20, 20, al_map_rgb(50,50,150));

            al_draw_filled_rounded_rectangle(WIDTH/7 - WIDTH/18, HEIGHT/6, WIDTH - WIDTH/7 + WIDTH/18, HEIGHT - HEIGHT/6 - HEIGHT/10, 20, 20, al_map_rgb(10,10,10));

            for(int j=0; j<DIFFICULTY_LEVELS; j++){
                for(int i=0; i<TOP_RANKING; i++){
                al_draw_textf(allegro_contexts.font, al_map_rgb(255,255,255), WIDTH/4 + j * WIDTH/4, HEIGHT/4 + HEIGHT/10 * i,
                              ALLEGRO_ALIGN_CENTER,"%s %04d", difficulty_entries[j].entries[i].name, difficulty_entries[j].entries[i].score);
                }
            }
                al_draw_textf(allegro_contexts.font, al_map_rgb(45,160,45), WIDTH/4 + 0 * WIDTH/4, HEIGHT/5 ,
                              ALLEGRO_ALIGN_CENTER,"EASY");

                al_draw_textf(allegro_contexts.font, al_map_rgb(200,170,25), WIDTH/4 + 1 * WIDTH/4, HEIGHT/5 ,
                              ALLEGRO_ALIGN_CENTER,"MEDIUM");

                al_draw_textf(allegro_contexts.font, al_map_rgb(180,30,30), WIDTH/4 + 2 * WIDTH/4, HEIGHT/5 ,
                              ALLEGRO_ALIGN_CENTER,"HARD");

                al_draw_textf(allegro_contexts.font, al_map_rgb(255, 255, 255), WIDTH/2, HEIGHT/20,
                              ALLEGRO_ALIGN_CENTER,"LEADERBOARD");

                al_draw_textf(allegro_contexts.font, al_map_rgb(255, 255, 255), WIDTH/2, HEIGHT - HEIGHT/6 ,
                              ALLEGRO_ALIGN_CENTER,"PRESS ESCAPE TO RETURN");


            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }
}

void loss_screen(t_allegro_context allegro_contexts, int points){

    ALLEGRO_EVENT ev = allegro_contexts.ev;


    int running = 1;
    int redraw = 0;

    while(running == 1){

        redraw = 1;

        al_wait_for_event(allegro_contexts.queue, &ev);   // Aguarda próximo evento na fila

        switch (ev.type) {

            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                running = 0;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE || ev.keyboard.keycode == ALLEGRO_KEY_X ||
                    ev.keyboard.keycode == ALLEGRO_KEY_Z || ev.keyboard.keycode == ALLEGRO_KEY_SPACE || ev.keyboard.keycode == ALLEGRO_KEY_ENTER)  // termina o jogo
                    running = 0;

                break;

            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo
                redraw = 1; // Marca que a tela precisa ser redesenhada
                break;
        }

        if (redraw == 1 && al_is_event_queue_empty(allegro_contexts.queue)) { // Redesenho da tela
            redraw = 0;

//            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            al_draw_filled_rounded_rectangle(WIDTH/3 + WIDTH/18, HEIGHT/3 + HEIGHT/15, WIDTH - (WIDTH/3 + WIDTH/18), HEIGHT - (HEIGHT/3 + HEIGHT/15), 0, 0, al_map_rgb(150,10,10));

            al_draw_filled_rounded_rectangle(WIDTH/3 + WIDTH/15, HEIGHT/3 + HEIGHT/12, WIDTH - (WIDTH/3 + WIDTH/15), HEIGHT - (HEIGHT/3 + HEIGHT/12), 0, 0, al_map_rgb(90,6,6));

            al_draw_text(allegro_contexts.font, al_map_rgb(255,255,255), WIDTH/2,
                        HEIGHT/3 + HEIGHT/10 , ALLEGRO_ALIGN_CENTER,"GAME OVER");

            al_draw_textf(allegro_contexts.font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2,
                          ALLEGRO_ALIGN_CENTER,"SCORE: %04d", points);
            }
            al_flip_display(); // Atualiza a tela com o que foi desenhado
    }
}

int pause_menu(t_allegro_context allegro_contexts){

    ALLEGRO_EVENT ev = allegro_contexts.ev;

    int running = 1;
    int redraw = 0;
    int selected = 1;

    t_button pause_button[2];
    pause_button[0] = (t_button){WIDTH/3 + WIDTH/18, HEIGHT/3 + HEIGHT/18, WIDTH - (WIDTH/3 + WIDTH/18), HEIGHT/3 + HEIGHT/18 + HEIGHT/15, 0};
    pause_button[1] = (t_button){WIDTH/3 + WIDTH/18, HEIGHT/3 + HEIGHT/18 + HEIGHT/15 + HEIGHT/30, WIDTH - (WIDTH/3 + WIDTH/18), HEIGHT/3 + HEIGHT/18 + 2 * HEIGHT/15 + HEIGHT/30, 0};

    pause_button[0].default_color = al_map_rgb(30,30,150);
    pause_button[0].selected_color = al_map_rgb(15,15,75);

    pause_button[1].default_color = al_map_rgb(30,30,150);
    pause_button[1].selected_color = al_map_rgb(15,15,75);


    while(running == 1){

        redraw = 1;

        al_wait_for_event(allegro_contexts.queue, &ev);   // Aguarda próximo evento na fila

        switch (ev.type) {

            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                running = 0;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                    return 0;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN){
                    pause_button[selected].selected = 0;
                    if(selected < 1) selected++;
                    else selected = 1;
                    pause_button[selected].selected = 1;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_UP){
                    pause_button[selected].selected = 0;
                    if(selected > 0) selected--;
                    else selected = 0;
                    pause_button[selected].selected = 1;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_X ||
                    ev.keyboard.keycode == ALLEGRO_KEY_Z || ev.keyboard.keycode == ALLEGRO_KEY_SPACE || ev.keyboard.keycode == ALLEGRO_KEY_ENTER){
                    if(pause_button[0].selected == 1){
                    running = 0;
                    return 0; // continua o jogo;
                    }
                    if(pause_button[1].selected == 1){
                    running = 0;
                    return 1; // volta para o menu;
                    }
                }

                break;

            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo
                redraw = 1; // Marca que a tela precisa ser redesenhada
                break;
        }

        if (redraw == 1 && al_is_event_queue_empty(allegro_contexts.queue)) { // Redesenho da tela
            redraw = 0;

            for(int i=0; i<2; i++){
                if(pause_button[i].selected == 0)
                al_draw_filled_rounded_rectangle(pause_button[i].origin_x, pause_button[i].origin_y, pause_button[i].end_x, pause_button[i].end_y, 0, 0, pause_button[i].default_color);
                else if(pause_button[i].selected == 1)
                al_draw_filled_rounded_rectangle(pause_button[i].origin_x, pause_button[i].origin_y, pause_button[i].end_x, pause_button[i].end_y, 0, 0, pause_button[i].selected_color);
            }

            al_draw_text(allegro_contexts.font, al_map_rgb(255,255,255), WIDTH/2,
                        ((pause_button[0].end_y - pause_button[0].origin_y) / 2) + pause_button[0].origin_y - HEIGHT/33, ALLEGRO_ALIGN_CENTER,"CONTINUE");

            al_draw_text(allegro_contexts.font, al_map_rgb(255,255,255), WIDTH/2,
                        ((pause_button[1].end_y - pause_button[1].origin_y) / 2) + pause_button[1].origin_y - HEIGHT/33, ALLEGRO_ALIGN_CENTER,"EXIT");

            }
            al_flip_display(); // Atualiza a tela com o que foi desenhado
    }
}

void get_new_highscore(t_allegro_context allegro_contexts, int points, int difficulty, t_topfive_leaderboard *difficulty_entries){

    ALLEGRO_EVENT ev = allegro_contexts.ev;

    int running = 1;
    int redraw = 0;
    int i = 0;

    t_leaderboard_entry new_entry = {0};

    new_entry.difficulty = difficulty;

    new_entry.score = points;

    while(running == 1){

        redraw = 1;

        al_wait_for_event(allegro_contexts.queue, &ev);   // Aguarda próximo evento na fila

        switch (ev.type) {

            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                running = 0;
                break;

            case ALLEGRO_EVENT_KEY_CHAR:
                if(i <= 10){

                if(ev.keyboard.unichar > 47 && ev.keyboard.unichar < 123)
                new_entry.name[i++] = ev.keyboard.unichar;
                }

                if(ev.keyboard.unichar == 127 || ev.keyboard.unichar == 8 && i > 0)
                new_entry.name[--i] = 0;

                if(ev.keyboard.unichar == 13 && i > 0){

                    save_highscore(new_entry, difficulty_entries);
                    return;
                }

                break;

            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo
                redraw = 1; // Marca que a tela precisa ser redesenhada
                break;
        }

        if (redraw == 1 && al_is_event_queue_empty(allegro_contexts.queue)) { // Redesenho da tela
            redraw = 0;

//            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            al_draw_filled_rounded_rectangle(WIDTH/3 + WIDTH/28, HEIGHT/3 + HEIGHT/15, WIDTH - (WIDTH/3 + WIDTH/28), HEIGHT - (HEIGHT/3 + HEIGHT/15), 0, 0, al_map_rgb(0,150,0));

            al_draw_filled_rounded_rectangle(WIDTH/3 + WIDTH/23, HEIGHT/3 + HEIGHT/12, WIDTH - (WIDTH/3 + WIDTH/23), HEIGHT - (HEIGHT/3 + HEIGHT/12), 0, 0, al_map_rgb(0,75,0));

            al_draw_textf(allegro_contexts.font, al_map_rgb(255,255,255), WIDTH/2,
                        HEIGHT/3 + HEIGHT/10 , ALLEGRO_ALIGN_CENTER,"NEW HIGHSCORE! %d", points);

            al_draw_textf(allegro_contexts.font, al_map_rgb(255,255,255), WIDTH/2,
                        HEIGHT/2, ALLEGRO_ALIGN_CENTER,"NAME: %s", new_entry.name);
            }
            al_flip_display(); // Atualiza a tela com o que foi desenhado
    }

}


int read_leaderboard_file(t_topfive_leaderboard difficulty_entries[]){

    // chances de dar problema com struct padding dependendo do sistema operacional

    FILE *leaderboard;

    leaderboard = fopen("leaderboard.bin", "rb+");

    if(leaderboard == NULL){
        leaderboard = fopen("leaderboard.bin", "wb");

            if(leaderboard == NULL){
                return -1;
            }
        fclose(leaderboard);
        return 1;
    }

    for(int i=0; i<DIFFICULTY_LEVELS; i++){
            if(fread(difficulty_entries[i].entries, sizeof(t_leaderboard_entry), TOP_RANKING, leaderboard) != TOP_RANKING){
                fclose(leaderboard);
                return 0;
            }
    }

    fclose(leaderboard);
    return 0;
}

int save_leaderboard_to_file(t_topfive_leaderboard difficulty_entries[]){

    FILE *leaderboard;

    leaderboard = fopen("leaderboard.bin", "wb");

    if(leaderboard == NULL){
        return -1;
    }

    for(int i=0; i<DIFFICULTY_LEVELS; i++){
    fwrite(difficulty_entries[i].entries, sizeof(t_leaderboard_entry), TOP_RANKING, leaderboard);
    }

    fclose(leaderboard);
    return 0;
}

void save_highscore(t_leaderboard_entry new_entry, t_topfive_leaderboard difficulty_entries[]){

    int position = -1;

    for(int i = 0; i<TOP_RANKING; i++){
        if(new_entry.score > difficulty_entries[new_entry.difficulty].entries[i].score){
            position = i;
            break;
        }
    }

    if(position == -1){
    return;
    }

    memmove(&difficulty_entries[new_entry.difficulty].entries[position + 1], &difficulty_entries[new_entry.difficulty].entries[position],
            (TOP_RANKING - position - 1) * sizeof(t_leaderboard_entry));

    difficulty_entries[new_entry.difficulty].entries[position] = new_entry;
}

void debug_leaderboard(t_topfive_leaderboard difficulty_entries[]){

    for(int i = 0; i<DIFFICULTY_LEVELS; i++){

        printf("\n\n PROX DIFICULDADE \n\n");

        for(int j=0; j<TOP_RANKING; j++){
            printf("%d %d %s\n", difficulty_entries[i].entries[j].difficulty, difficulty_entries[i].entries[j].score, difficulty_entries[i].entries[j].name);
        }

    }
    printf("\n\n FIM \n\n");
    return;

}

int highscore_comparison(t_topfive_leaderboard difficulty_entries[], int points, int difficulty_level){

    if(points == 0){
        return 0;
    }

    if(points > difficulty_entries[difficulty_level].entries[0].score){
        return 1;
    }

    return 0;
}

void draw_keybinds(t_allegro_context allegro_contexts, int sprite_scaling, ALLEGRO_BITMAP *keybind_sprite){

    int sprite_x = 0;        // Coordenada X no sprite, sempre 0
    int sprite_y = 0;        // Coordenada Y no sprite, 0, 1, 2, 3, 4, 5 de acordo com o botão

    char keybind_names[6][15] = {
    "RIGHT ARROW",
    "LEFT ARROW",
    "DOWN ARROW",
    "SPACEBAR",
    " Z X ",
    "ESCAPE"
    };

    for(int i=0; i<2; i++){
        for(int j=3; j>0; j--){
            al_draw_scaled_bitmap(keybind_sprite, sprite_x, sprite_y * 48, 48, 48,
                                         WIDTH + WIDTH/30 - (j * 4.5f * sprite_scaling), HEIGHT/10 + (i * (sprite_scaling + HEIGHT/10)),
                                          2 * sprite_scaling, 2 * sprite_scaling, 0);
                                            // desenha o botão baseado na escala definida

            al_draw_textf(allegro_contexts.small_font, al_map_rgb(255,255,255), WIDTH + WIDTH/30 - ((j * 4.5f * sprite_scaling) - (sprite_scaling)),
                          HEIGHT/10 + (i * (sprite_scaling + HEIGHT/10)) + (HEIGHT / 12), ALLEGRO_ALIGN_CENTER,"%s", keybind_names[sprite_y]);

            sprite_y++; // define o botão de acordo com o sprite
        }
    }

}


