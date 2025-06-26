#include "allegro_context.h"

AllegroContext allegro_init()
{
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

    // Cria uma janela com as dimensões especificadas
    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);

    // Cria uma fila de eventos para tratar interações do usuário e um temporizador para controle de FPS
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60); // 60 quadros por segundo

    // Registra as fontes de eventos que a fila vai monitorar
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Inicia o temporizador
    al_start_timer(timer);

    AllegroContext allegro = {
        .display = display,
        .queue = queue,
        .timer = timer
    };

    return allegro;
}

void allegro_free(AllegroContext allegro)
{
    // Libera recursos alocados
    al_destroy_timer(allegro.timer);
    al_destroy_event_queue(allegro.queue);
    al_destroy_display(allegro.display);
}
