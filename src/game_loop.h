#pragma once
#include "allegro_context.h"

#define DIFFICULTY_LEVELS 3
#define TOP_RANKING 5

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

    t_leaderboard_entry entries[TOP_RANKING];

}   t_topfive_leaderboard;

int main_menu(t_allegro_context allegro_contexts, t_topfive_leaderboard *difficulty_entries);
int leaderboard_menu(t_allegro_context allegro_contexts, t_topfive_leaderboard *difficulty_entries);
void game_loop(t_allegro_context allegro_contexts,t_topfive_leaderboard *difficulty_entries, float difficulty, int difficulty_level);
void loss_screen(t_allegro_context allegro_contexts,int points);
int pause_menu(t_allegro_context allegro_contexts);
void draw_keybinds(t_allegro_context allegro_contexts, int sprite_scaling, ALLEGRO_BITMAP *keybind_sprite);
void get_new_highscore(t_allegro_context allegro_contexts, int points, int difficulty, t_topfive_leaderboard *difficulty_entries);

int read_leaderboard_file(t_topfive_leaderboard difficulty_entries[]);
int save_leaderboard_to_file(t_topfive_leaderboard difficulty_entries[]);
void debug_leaderboard(t_topfive_leaderboard difficulty_entries[]);
void save_highscore(t_leaderboard_entry new_entry, t_topfive_leaderboard difficulty_entries[]);

int highscore_comparison(t_topfive_leaderboard difficulty_entries[], int points, int difficulty_level);

void go_fullscreen(t_allegro_context allegro_contexts, int is_fullscreen);
