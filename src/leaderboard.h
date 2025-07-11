#pragma once

// Número de dificuldades (fácil, normal e difícil)

#define DIFFICULTY_LEVELS 3

// Número de entradas no placar

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

// Subrotinas relativas ao placar

void read_leaderboard_file(t_leaderboard *leaderboard);      // Ler arquivo do placar para a memória
int save_leaderboard_to_file(t_leaderboard *leaderboard);   // Salvar o placar em arquivo

void save_highscore(t_leaderboard *leaderboard, t_leaderboard_entry new_entry);     // Adicionar novo recorde ao placar
void debug_leaderboard(t_leaderboard *leaderboard);                                 // Mostrar no console o placar
