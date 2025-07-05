#pragma once

#include "allegro_context.h"

// struct contendo o módulo das entradas

typedef struct Input {
    int left_down;
    int right_down;
    int up_down;
    int down_down;

    int left_pressed;
    int right_pressed;
    int up_pressed;
    int down_pressed;

    int z_pressed;
    int x_pressed;

    int space_pressed;
    int c_pressed;

    int escape_pressed;

    int f4_pressed;
} Input;

Input *input_init();            // Inicialização do módulo de entrada
void input_free(Input *input);  // Finalização do módulo de entrada

void input_pressed_flush(Input *input);                     // Função chamada a cada frame, para resetar os valores das teclas *pressionadas*
void input_update(AllegroContext *allegro, Input *input);   // Função que recebe as entradas do teclado
