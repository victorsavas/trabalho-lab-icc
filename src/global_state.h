#pragma once

#include "allegro_context.h"

#define DIFFICULTY_LEVELS 3
#define TOP_RANKING 5

typedef struct t_leaderboard_entry {
    int difficulty;
    int score;
    char name[20];
} t_leaderboard_entry;

typedef struct t_topfive_leaderboard {
    t_leaderboard_entry entries[TOP_RANKING];
} t_topfive_leaderboard;

extern int difficulty;
extern t_topfive_leaderboard difficulty_entries[DIFFICULTY_LEVELS];

int read_leaderboard_file();
int save_leaderboard_to_file();
void save_highscore(t_leaderboard_entry new_entry);
void debug_leaderboard();

int highscore_comparison(int points);
void get_new_highscore(AllegroContext *allegro, int points);
