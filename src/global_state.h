#pragma once

#include "allegro_context.h"

#define DIFFICULTY_LEVELS 3
#define TOP_RANKING 5

// Estrutura contendo dados de uma entrada do placar

typedef struct t_leaderboard_entry {
    int difficulty;
    int score;
    char name[20];
} t_leaderboard_entry;

// Estrutura do placar

typedef struct t_topfive_leaderboard {
    t_leaderboard_entry entries[TOP_RANKING];
} t_leaderboard;

typedef struct GlobalState {
    t_leaderboard leaderboard[DIFFICULTY_LEVELS];
    int selected_difficulty;
} GlobalState;

GlobalState *global_init();
void global_free(GlobalState *global);

int read_leaderboard_file(t_leaderboard *leaderboard);
int save_leaderboard_to_file(t_leaderboard *leaderboard);

void save_highscore(t_leaderboard *leaderboard, t_leaderboard_entry new_entry);
void debug_leaderboard(t_leaderboard *leaderboard);

int highscore_comparison(GlobalState *global, int points);
