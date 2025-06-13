#include "allegro_context.h"
#include "game.h"

int main()
{
    AllegroContext *allegro = allegro_init();
    Input *input = input_init();

    game_loop(allegro, input);

    input_free(input);
    allegro_free(allegro);

    return 0;
}
