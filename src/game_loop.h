#pragma once
#include "allegro_context.h"

typedef struct{
    int origin_x;
    int origin_y;
    int end_x;
    int end_y;
    int selected;

}   t_button;

void game_loop(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev);

int menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev);
