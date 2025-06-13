#pragma once

#include "allegro_context.h"

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
} Input;

Input *input_init();
void input_free(Input *input);

void input_pressed_flush(Input *input);
void input_update(AllegroContext *allegro, Input *input);
