#pragma once

#include "allegro_context.h"

#define DIFFICULTY_LEVELS 3
#define TOP_RANKING 5

// Estrutura contendo dados de uma entrada do placar

typedef struct t_leaderboard_entry {
    int difficulty;                     // Dificuldade relativa à pontuação
    int score;                          // Pontuação
    char name[20];                      // Nome salvo
} t_leaderboard_entry;

// Estrutura do placar

typedef struct t_leaderboard {
    t_leaderboard_entry entries[TOP_RANKING];   // Entradas do placar
} t_leaderboard;

typedef struct GlobalState {
    t_leaderboard leaderboard[DIFFICULTY_LEVELS];   // Placar
    int selected_difficulty;                        // Dificuldade selecionada
} GlobalState;

GlobalState *global_init();             // Inicialização do estado global
void global_free(GlobalState *global);  // Finalização do estado global

int read_leaderboard_file(t_leaderboard *leaderboard);      // Ler arquivo do placar para a memória
int save_leaderboard_to_file(t_leaderboard *leaderboard);   // Salvar o placar em arquivo

void save_highscore(t_leaderboard *leaderboard, t_leaderboard_entry new_entry);     // Adicionar novo recorde ao placar
void debug_leaderboard(t_leaderboard *leaderboard);                                 // Mostrar no console o placar

int highscore_comparison(GlobalState *global, int points);      // Compara os pontos obtidos com o recorde do placar
