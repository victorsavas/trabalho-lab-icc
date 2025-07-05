#pragma once

#include "leaderboard.h"

typedef struct GlobalState {
    t_leaderboard leaderboard[DIFFICULTY_LEVELS];   // Placar
    int selected_difficulty;                        // Dificuldade selecionada
} GlobalState;

GlobalState *global_init();             // Inicialização do estado global
void global_free(GlobalState *global);  // Finalização do estado global

int highscore_comparison(GlobalState *global, int points);      // Compara os pontos obtidos com o recorde do placar
