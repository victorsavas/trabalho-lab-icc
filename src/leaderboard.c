#include <stdio.h>
#include "global_state.h"
#include "game.h"

int read_leaderboard_file()
{
    // chances de dar problema com struct padding dependendo do sistema operacional

    FILE *leaderboard;

    leaderboard = fopen("leaderboard.bin", "rb+");

    if(leaderboard == NULL){
        leaderboard = fopen("leaderboard.bin", "wb");

            if(leaderboard == NULL){
                return 1;
            }
        fclose(leaderboard);
        return 0;
    }

    for(int i=0; i<DIFFICULTY_LEVELS; i++){
            if(fread(difficulty_entries[i].entries, sizeof(t_leaderboard_entry), TOP_RANKING, leaderboard) != TOP_RANKING){
                fclose(leaderboard);
                return 0;
            }
    }

    fclose(leaderboard);
    return 0;
}

int save_leaderboard_to_file()
{
    FILE *leaderboard;

    leaderboard = fopen("leaderboard.bin", "wb");

    if(leaderboard == NULL){
        return -1;
    }

    for(int i=0; i<DIFFICULTY_LEVELS; i++){
    fwrite(difficulty_entries[i].entries, sizeof(t_leaderboard_entry), TOP_RANKING, leaderboard);
    }

    fclose(leaderboard);
    return 0;
}

void save_highscore(t_leaderboard_entry new_entry)
{
    int position = -1;

    for(int i = 0; i<TOP_RANKING; i++){
        if(new_entry.score > difficulty_entries[new_entry.difficulty].entries[i].score){
            position = i;
            break;
        }
    }

    if(position == -1){
        return;
    }

    memmove(
        &difficulty_entries[new_entry.difficulty].entries[position + 1],
        &difficulty_entries[new_entry.difficulty].entries[position],
        (TOP_RANKING - position - 1) * sizeof(t_leaderboard_entry)
    );

    difficulty_entries[new_entry.difficulty].entries[position] = new_entry;
}

void debug_leaderboard()
{
    for(int i = 0; i<DIFFICULTY_LEVELS; i++){

        printf("\n\n PROX DIFICULDADE \n\n");

        for(int j=0; j<TOP_RANKING; j++){
            printf("%d %d %s\n", difficulty_entries[i].entries[j].difficulty, difficulty_entries[i].entries[j].score, difficulty_entries[i].entries[j].name);
        }

    }
    printf("\n\n FIM \n\n");
    return;

}

int highscore_comparison(int points)
{
    if(points == 0){
        return 0;
    }

    if(points > difficulty_entries[difficulty].entries[0].score){
        return 1;
    }

    return 0;
}

void get_new_highscore(AllegroContext *allegro, int points)
{
    int i = 0;

    t_leaderboard_entry new_entry = {0};

    new_entry.difficulty = difficulty;
    new_entry.score = points;

    while (1) {
        al_wait_for_event(allegro->queue, &allegro->event);

        switch (allegro->event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                return;

            case ALLEGRO_EVENT_KEY_CHAR:
                if(i <= 10){

                if(allegro->event.keyboard.unichar > 47 && allegro->event.keyboard.unichar < 123)
                    new_entry.name[i++] = allegro->event.keyboard.unichar;
                }

                if(allegro->event.keyboard.unichar == 127 || allegro->event.keyboard.unichar == 8 && i > 0)
                new_entry.name[--i] = 0;

                if(allegro->event.keyboard.unichar == 13 && i > 0){

                    save_highscore(new_entry);
                    return;
                }

                break;

            case ALLEGRO_EVENT_TIMER:
                allegro->redraw = 1;
                break;
        }

        if (allegro->redraw == 1 && al_is_event_queue_empty(allegro->queue)) {
            allegro->redraw = 0;

            al_draw_filled_rounded_rectangle(
                WIDTH/3 + WIDTH/28,
                HEIGHT/3 + HEIGHT/15,
                WIDTH - (WIDTH/3 + WIDTH/28),
                HEIGHT - (HEIGHT/3 + HEIGHT/15),
                0,
                0,
                al_map_rgb(81, 179, 65)
            );

            al_draw_filled_rounded_rectangle(
                WIDTH/3 + WIDTH/23,
                HEIGHT/3 + HEIGHT/12,
                WIDTH - (WIDTH/3 + WIDTH/23),
                HEIGHT - (HEIGHT/3 + HEIGHT/12),
                0,
                0,
                al_map_rgb(23, 81, 69)
            );

            al_draw_textf(
                allegro->font,
                WHITE,
                WIDTH/2,
                HEIGHT/3 + HEIGHT/10,
                ALLEGRO_ALIGN_CENTER,
                "NEW HIGHSCORE! %d",
                points
            );

            al_draw_textf(
                allegro->font,
                WHITE,
                WIDTH/2,
                HEIGHT/2,
                ALLEGRO_ALIGN_CENTER,
                "NAME: %s",
                new_entry.name
            );

            al_flip_display();
        }
    }
}
