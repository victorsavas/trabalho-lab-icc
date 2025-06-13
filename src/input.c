#include "input.h"

Input *input_init()
{
    Input *input;

    input = calloc(1, sizeof(*input));

    return input;
}

void input_free(Input *input)
{
    free(input);
}

void input_pressed_flush(Input *input)
{
    // Zerando as variáveis um frame após a tecla ser pressionado

    input->left_pressed  = 0;
    input->right_pressed = 0;
    input->up_pressed    = 0;
    input->down_pressed  = 0;

    input->z_pressed = 0;
    input->x_pressed = 0;

    input->space_pressed = 0;
    input->c_pressed     = 0;

    input->escape_pressed = 0;
}

void input_update(AllegroContext *allegro, Input *input)
{
    // Checando teclas pressionadas

    if (allegro->event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (allegro->event.keyboard.keycode) {
        case ALLEGRO_KEY_LEFT:
            input->left_pressed = 1;
            input->left_down = 1;
            break;

        case ALLEGRO_KEY_RIGHT:
            input->right_pressed = 1;
            input->right_down = 1;
            break;

        case ALLEGRO_KEY_UP:
            input->up_pressed = 1;
            input->up_down = 1;
            break;

        case ALLEGRO_KEY_DOWN:
            input->down_pressed = 1;
            input->down_down = 1;
            break;

        case ALLEGRO_KEY_Z:
            input->z_pressed = 1;
            break;

        case ALLEGRO_KEY_X:
            input->x_pressed = 1;
            break;

        case ALLEGRO_KEY_C:
            input->c_pressed = 1;
            break;

        case ALLEGRO_KEY_SPACE:
            input->space_pressed = 1;
            break;

        case ALLEGRO_KEY_ESCAPE:
            input->escape_pressed = 1;
            break;
        }
    }

    // Checando teclas soltas

    if (allegro->event.type == ALLEGRO_EVENT_KEY_UP) {
        switch (allegro->event.keyboard.keycode) {
        case ALLEGRO_KEY_LEFT:
            input->left_down = 0;
            break;

        case ALLEGRO_KEY_RIGHT:
            input->right_down = 0;
            break;

        case ALLEGRO_KEY_UP:
            input->up_down = 0;
            break;

        case ALLEGRO_KEY_DOWN:
            input->down_down = 0;
            break;
        }
    }
}
