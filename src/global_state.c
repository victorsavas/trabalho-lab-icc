#include <stdlib.h>
#include <stdio.h>
#include "global_state.h"

GlobalState *global_init()
{
    // Alocação dinâmica

    GlobalState *global;
    global = calloc(1, sizeof(*global));

    global->selected_difficulty = 0;

    // Carrega o placar

    read_leaderboard_file(global->leaderboard);

    return global;
}

void global_free(GlobalState *global)
{
    // Salva o placar e libera memória

    save_leaderboard_to_file(global->leaderboard);

    free(global);
}

int highscore_comparison(GlobalState *global, int points)
{
    t_leaderboard *leaderboard = global->leaderboard;
    int difficulty = global->selected_difficulty;

    if(points == 0){
        return 0;
    }

    if(points > leaderboard[difficulty].entries[0].score){
        return 1;
    }

    return 0;
}
