#include "allegro_context.h"
#include "game.h"

int main()
{
    AllegroContext *allegro = allegro_init();

    game_loop(allegro);

    allegro_free(allegro);

    return 0;
}
