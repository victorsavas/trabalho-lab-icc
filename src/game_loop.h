#pragma once
#include "allegro_context.h"

typedef struct{
    int origin_x;
    int origin_y;
    int end_x;
    int end_y;
    int selected;
    ALLEGRO_COLOR default_color;
    ALLEGRO_COLOR selected_color;
}   t_button;

typedef struct{
    int difficulty;
    int score;
    char name[20];

}   t_leaderboard_entry;

typedef struct{

    t_leaderboard_entry entries[5];

}   t_topfive_leaderboard;

int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev, ALLEGRO_FONT *font, ALLEGRO_FONT *font_small);
void game_loop(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev, ALLEGRO_FONT *font, float difficulty);
void loss_screen(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev, ALLEGRO_FONT *font,int points);
int pause_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev, ALLEGRO_FONT *font);
void get_new_highscore(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer, ALLEGRO_EVENT *ev, ALLEGRO_FONT *font, int points);

