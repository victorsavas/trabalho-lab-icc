#include <stdlib.h>
#include <stdio.h>
#include "allegro_context.h"               // contém todos os includes do allegro
#include "gameloop_logic.h"
#include "game_loop.h"

int WIDTH, HEIGHT;

int main(){

    // Inicializa o Allegro e os módulos necessários
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_init_acodec_addon();

    t_allegro_context allegro_contexts;

    ALLEGRO_MONITOR_INFO screen_info;

    char manual_wh[15];

    if(al_get_monitor_info(0, &screen_info) == 0){
        printf("SEND SCREEN WIDTH AND HEIGHT\n");
        fgets(manual_wh,15, stdin);
        if(sscanf(manual_wh, "%d %d", &WIDTH, &HEIGHT) != 2){
            printf("INVALID INPUT\n");
            return -1;
        }
    }
    else{

        WIDTH = screen_info.x2 - screen_info.x1;
        HEIGHT = screen_info.y2 - screen_info.y1;
    }

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    al_set_new_display_flags(ALLEGRO_MAXIMIZED);
    allegro_contexts.display = al_create_display(WIDTH, HEIGHT);   // Cria uma janela com as dimensões especificadas

    // Cria uma fila de eventos para tratar interações do usuário e um temporizador para controle de FPS

    allegro_contexts.queue = al_create_event_queue();
    allegro_contexts.timer = al_create_timer(1.0 / FPS);  // 60 quadros por segundo

    int font_scale, small_font_scale;

    if(HEIGHT < WIDTH){               // escala os sprites baseado no menor para manter proporção
        font_scale = HEIGHT / 20;
        small_font_scale = HEIGHT / 28;
    }
    else{
        font_scale = WIDTH / 30;
        small_font_scale = WIDTH / 43;
    }


    allegro_contexts.font = al_load_ttf_font("../../fontes/Jersey15-Regular.ttf", font_scale, 0);
    if (!allegro_contexts.font) {
        printf("Erro ao carregar fonte.\n");
        return -1;
    }

    allegro_contexts.small_font = al_load_ttf_font("../../fontes/Jersey15-Regular.ttf", small_font_scale, 0);
    if (!allegro_contexts.small_font) {
        printf("Erro ao carregar fonte.\n");
        return -1;
    }

    // Registra as fontes de eventos que a fila vai monitorar
    al_register_event_source(allegro_contexts.queue, al_get_display_event_source(allegro_contexts.display));
    al_register_event_source(allegro_contexts.queue, al_get_timer_event_source(allegro_contexts.timer));
    al_register_event_source(allegro_contexts.queue, al_get_keyboard_event_source());

    al_start_timer(allegro_contexts.timer); // inicia o temporizador


    t_topfive_leaderboard *difficulty_entries;

    difficulty_entries = calloc(DIFFICULTY_LEVELS, sizeof(t_topfive_leaderboard));

    if(read_leaderboard_file(difficulty_entries) == -1){
        printf("LEADERBOARD FILE ERROR");
        return -1;
    }

    main_menu(allegro_contexts, difficulty_entries);


    if((save_leaderboard_to_file(difficulty_entries)) == -1){
        printf("LEADERBOARD FILE ERROR");
        return -1;
    }

    al_destroy_timer(allegro_contexts.timer);
    al_destroy_event_queue(allegro_contexts.queue);
    al_destroy_display(allegro_contexts.display);
}
