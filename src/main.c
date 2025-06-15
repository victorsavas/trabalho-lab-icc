#include "allegro_context.h"               // contém todos os includes do allegro
#include "gameloop_subroutines.h"
#include "game_loop.h"

int main(){

    // Inicializa o Allegro e os módulos necessários
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_mouse();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();

    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);   // Cria uma janela com as dimensões especificadas

    // Cria uma fila de eventos para tratar interações do usuário e um temporizador para controle de FPS
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);  // 60 quadros por segundo
    ALLEGRO_EVENT ev;         // Estrutura para eventos

    ALLEGRO_FONT *font = al_load_ttf_font("../../fontes/PixelifySans-SemiBold.ttf", 50, 0);
    if (!font) {
        printf("Erro ao carregar fonte.\n");
        return -1;
    }

    ALLEGRO_FONT *font_small = al_load_ttf_font("../../fontes/PixelifySans-SemiBold.ttf", 35, 0);
    if (!font_small) {
        printf("Erro ao carregar fonte.\n");
        return -1;
    }

    // Registra as fontes de eventos que a fila vai monitorar
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    al_start_timer(timer); // inicia o temporizador

    main_menu(queue, timer, &ev, font, font_small);

    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}

