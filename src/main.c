#include "allegro_context.h"
#include "game.h"

int main()
{
    srand(time(NULL));

    AllegroContext *allegro = allegro_init();
    Input *input = input_init();

    // Teste de validação

    if (allegro == NULL || input == NULL) {
        allegro_free(allegro);
        input_free(input);

        return 1;
    }

    game_loop(allegro, input);

    input_free(input);
    allegro_free(allegro);

    return 0;
}
