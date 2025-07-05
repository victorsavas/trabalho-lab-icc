#pragma once

#include "allegro_context.h"
#include "global_state.h"
#include "input.h"

// Enumeração dos possíveis estados ou "modos" do jogo

typedef enum GameMode {
    MODE_EXIT=0,        // Sinal para encerra o programa
    MODE_MAIN_MENU,     // Modo de exibição do menu principal
    MODE_PLAYFIELD      // Modo de exibição do tabuleiro
} GameMode;

// Estrutura de dados de botões selecionáveis de formato retangular

typedef struct t_button {
    int origin_x;                   // Posição x da borda esquerda do botão
    int origin_y;                   // Posição y da borda superior do botão

    int end_x;                      // Posição x da borda direita do botão
    int end_y;                      // Posição y da borda inferior do botão

    char *label;                    // Rótulo do botão

    ALLEGRO_COLOR default_color;    // Cor padrão do botão
    ALLEGRO_COLOR selected_color;   // Cor do botão ao ser selecionado
} t_button;

// Cores comuns empregadas no jogo

#define BACKGROUND_COLOR al_map_rgb(12, 8, 42)
#define WHITE            al_map_rgb(255, 255, 255)

// Subrotina que desenha um botão na tela

void button_draw(AllegroContext* allegro, t_button button, int selected);

// Subrotina principal que manuseia os estados ou "modos" do jogo

void game_loop(GlobalState *global, AllegroContext *allegro, Input *input);

// Subrotinas dos dois "modos" do jogo: menu principal e playfield. Ao retornar, indicam qual deve ser a próxima subrotina chamada.

GameMode game_main_menu(GlobalState *global, AllegroContext *allegro, Input *input);
GameMode game_playfield(GlobalState *global, AllegroContext *allegro, Input *input);
