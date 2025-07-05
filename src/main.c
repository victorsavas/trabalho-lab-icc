#include "allegro_context.h"
#include "global_state.h"
#include "game.h"

int main()
{
    // Geração de números aleatórios

    srand(time(NULL));

    // Inicialização dos módulos do Allegro

    AllegroContext *allegro = allegro_init();

    // Inicialização do módulo de entradas

    Input *input = input_init();

    // Inicialização do estado global do jogo (placar e dificuldade selecionada)

    GlobalState *global = global_init();

    // Teste de validação

    if (allegro == NULL || input == NULL || global == NULL) {
        // Finalização prematura do programa

        printf("Erro! Finalização prematura.\n");

        allegro_free(allegro);
        input_free(input);
        global_free(global);

        return 1;
    }

    // Loop do jogo

    game_loop(global, allegro, input);

    // Encerramento do programa

    global_free(global);
    input_free(input);
    allegro_free(allegro);

    return 0;
}
