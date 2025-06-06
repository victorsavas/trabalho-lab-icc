#include "allegro_context.h"
#include "game_loop.h"

int main()
{
    AllegroContext allegro = allegro_init();

    game_loop(allegro);

    allegro_free(allegro);

    return 0;
}
