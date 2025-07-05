#include <stdlib.h>
#include "allegro_context.h"

AllegroContext *allegro_init()
{
    // Inicializa o Allegro e os m�dulos necess�rios
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();

    // Cria uma janela com as dimens�es especificadas
    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);

    // Cria uma fila de eventos para tratar intera��es do usu�rio e um temporizador para controle de FPS
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0); // 60 quadros por segundo

    // Registra as fontes de eventos que a fila vai monitorar
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Inicia o temporizador
    al_start_timer(timer);

    // Fonte
    ALLEGRO_FONT *font = al_load_ttf_font("../../fontes/Jersey15-Regular.ttf", 50, 0);
    ALLEGRO_FONT *font_small = al_load_ttf_font("../../fontes/Jersey15-Regular.ttf", 35, 0);

    // Bitmaps
    ALLEGRO_BITMAP *bitmap_blocks = al_load_bitmap("../../sprites/tetris_sprite_16x16.png");
    ALLEGRO_BITMAP *bitmap_playfield = al_load_bitmap("../../sprites/borda_tetris_170x330.png");
    ALLEGRO_BITMAP *bitmap_keybinds = al_load_bitmap("../../sprites/keybind_sprite_48x384.png");

    // Aloca��o din�mica
    AllegroContext *allegro;
    allegro = malloc(sizeof(*allegro));

    // Testes de valida��o
    if (allegro          == NULL
     || display          == NULL
     || queue            == NULL
     || timer            == NULL
     || font             == NULL
     || font_small       == NULL
     || bitmap_blocks    == NULL
     || bitmap_playfield == NULL
     || bitmap_keybinds  == NULL) {
        free(allegro);

        al_destroy_timer(allegro->timer);
        al_destroy_event_queue(allegro->queue);
        al_destroy_display(allegro->display);

        al_destroy_font(font);
        al_destroy_font(font_small);

        al_destroy_bitmap(bitmap_blocks);
        al_destroy_bitmap(bitmap_playfield);
        al_destroy_bitmap(bitmap_keybinds);

        return NULL;
    }

    *allegro = (AllegroContext){
        .display = display,
        .queue   = queue,
        .timer   = timer,
        .event   = {0},

        .font       = font,
        .font_small = font_small,

        .bitmap_blocks    = bitmap_blocks,
        .bitmap_playfield = bitmap_playfield,
        .bitmap_keybinds  = bitmap_keybinds,

        .fullscreen = 0,
        .redraw     = 0
    };

    return allegro;
}

void allegro_free(AllegroContext *allegro)
{
    if (allegro == NULL) {
        return;
    }

    // Libera recursos alocados
    al_destroy_timer(allegro->timer);
    al_destroy_event_queue(allegro->queue);
    al_destroy_display(allegro->display);

    al_destroy_font(allegro->font);
    al_destroy_font(allegro->font_small);

    al_destroy_bitmap(allegro->bitmap_blocks);
    al_destroy_bitmap(allegro->bitmap_playfield);
    al_destroy_bitmap(allegro->bitmap_keybinds);

    free(allegro);
}


void toggle_fullscreen(AllegroContext *allegro)
{
    if(allegro->fullscreen){
        al_set_new_display_flags(ALLEGRO_WINDOWED);

        allegro->fullscreen = 0;
    } else {
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

        allegro->fullscreen = 1;
    }

    al_destroy_event_queue(allegro->queue);
    al_destroy_display(allegro->display);

    allegro->display = al_create_display(WIDTH, HEIGHT);
    allegro->queue = al_create_event_queue();

    al_register_event_source(allegro->queue, al_get_display_event_source(allegro->display));
    al_register_event_source(allegro->queue, al_get_timer_event_source(allegro->timer));
    al_register_event_source(allegro->queue, al_get_keyboard_event_source());
}
