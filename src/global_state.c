#include <stdio.h>
#include "global_state.h"

GlobalState *global_init()
{
    GlobalState *global;
    global = calloc(1, sizeof(*global));

    int error_code = read_leaderboard_file(global->leaderboard);

    if (error_code) {
        free(global);
        return NULL;
    }

    return global;
}

void global_free(GlobalState *global)
{
    save_leaderboard_to_file(global->leaderboard);

    free(global);
}

int read_leaderboard_file(t_leaderboard *leaderboard)
{
    FILE *file;

    file = fopen("leaderboard.bin", "rb+");

    if(file == NULL){
        file = fopen("leaderboard.bin", "wb");

        // Caso não exista arquivo do placar e não seja possível criar um novo, o jogo é encerrado prematuramente

        if(file == NULL){
            return 1;
        }

        fclose(file);
        return 0;
    }

    // Leitura do arquivo

    for(int i=0; i<DIFFICULTY_LEVELS; i++){
            if(fread(leaderboard[i].entries, sizeof(t_leaderboard_entry), TOP_RANKING, file) != TOP_RANKING){
                fclose(file);
                return 0;
            }
    }

    // Fechamento do arquivo
    fclose(file);

    return 0;
}

int save_leaderboard_to_file(t_leaderboard *leaderboard)
{
    FILE *file;

    file = fopen("leaderboard.bin", "wb");

    if(file == NULL){
        return -1;
    }

    for(int i=0; i<DIFFICULTY_LEVELS; i++){
    fwrite(leaderboard[i].entries, sizeof(t_leaderboard_entry), TOP_RANKING, file);
    }

    fclose(file);
    return 0;
}

void save_highscore(t_leaderboard *leaderboard, t_leaderboard_entry new_entry)
{
    int position = -1;

    for(int i = 0; i<TOP_RANKING; i++){
        if(new_entry.score > leaderboard[new_entry.difficulty].entries[i].score){
            position = i;
            break;
        }
    }

    if(position == -1){
        return;
    }

    memmove(
        &leaderboard[new_entry.difficulty].entries[position + 1],
        &leaderboard[new_entry.difficulty].entries[position],
        (TOP_RANKING - position - 1) * sizeof(t_leaderboard_entry)
    );

    leaderboard[new_entry.difficulty].entries[position] = new_entry;

    save_leaderboard_to_file(leaderboard);
}

void debug_leaderboard(t_leaderboard *leaderboard)
{
    for(int i = 0; i<DIFFICULTY_LEVELS; i++){

        printf("\n\n PROX DIFICULDADE \n\n");

        for(int j=0; j<TOP_RANKING; j++){
            printf("%d %d %s\n", leaderboard[i].entries[j].difficulty, leaderboard[i].entries[j].score, leaderboard[i].entries[j].name);
        }

    }
    printf("\n\n FIM \n\n");
    return;

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
