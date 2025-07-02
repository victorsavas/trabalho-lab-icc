#include <time.h>                          // necess�rio para aleatorizar as pe�as
#include "allegro_context.h"               // cont�m todos os includes do allegro
#include "game_loop.h"
#include "gameloop_logic.h"

void game_loop(t_allegro_context allegro_contexts, t_topfive_leaderboard *difficulty_entries, float difficulty, int difficulty_level){

    ALLEGRO_EVENT ev = allegro_contexts.ev;

    // Carrega uma imagem contendo m�ltimos sprites (spritesheet)
    ALLEGRO_BITMAP *piece_sprite = al_load_bitmap("../../sprites/tetris_sprite_16x16.png");
    ALLEGRO_BITMAP *board_sprite = al_load_bitmap("../../sprites/borda_tetris_170x330.png");
    ALLEGRO_BITMAP *keybind_sprite = al_load_bitmap("../../sprites/keybind_sprite_48x288.png");
    al_convert_mask_to_alpha(piece_sprite, al_map_rgb(0, 0, 0));
    al_convert_mask_to_alpha(board_sprite, al_map_rgb(0, 0, 0));
    al_convert_mask_to_alpha(keybind_sprite, al_map_rgb(0, 0, 0));

    // Vari�veis de controle da aplica��o
    int falling = 1;      // Indica se a aplica��o deve continuar executando
    bool redraw = true;       // Indica se a tela precisa ser redesenhada

    int board[BOARD_ROWS * BOARD_COLS]; // definido no gameloop.h
    int cleared_row = 0; // usado na fun��o de descer as fileiras limpas
    int fall_timer = 0; // usado para queda de blocos
    int sprite_scaling;// usado para alterar o tamanho dos sprites
    int points = 0; // pontua��o do usu�rio
    int dont_show_keybinds_again = 0;
    float fall_speed = difficulty;    // maior diminui a velocidade, menor aumenta

    if(HEIGHT < WIDTH){               // escala os sprites baseado no menor para manter propor��o
        sprite_scaling = HEIGHT / 25;
    }
    else{
        sprite_scaling = WIDTH / 37.5F;
    }


    for (int i = 0; i < BOARD_ROWS * BOARD_COLS; i++){ // preencher tabuleiro base com vazio
        board[i] = 0;
        // 0 vazio
        // 1 vermelho
        // 2 amarelo
        // 3 azul
        // 4 verde
    }

    srand(time(NULL)); // necess�rio para aleatorizar as pe�as

    t_piece current_piece;  // cria a pe�a livre
    randomize_piece(&current_piece) ;// aleatoriza a primeira pe�a


    while (falling == 1) {         // Loop principal do jogo

        al_wait_for_event(allegro_contexts.queue, &ev);   // Aguarda pr�ximo evento na fila

       // remove_piece_board(&current_piece, board); // corrige o erro de sobreposi��o das pe�as

        // Tratamento dos eventos recebidos
        switch (ev.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                falling = false;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE){          // termina o jogo
                    if(points > 0 || dont_show_keybinds_again == 1)
                    draw_keybinds(allegro_contexts, sprite_scaling, keybind_sprite);
                    if(pause_menu(allegro_contexts) == 1) falling = 0;
                }
                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_LEFT){    // move a pe�a baseado no input do teclado
                    remove_piece_board(&current_piece, board);          // remove a pe�a antiga
                    current_piece.board_x -= 1;                         // move para a esquerda
                    if(check_collisions(&current_piece, board) == 1) current_piece.board_x += 1; // retorna caso nova posi��o colide
                    add_piece_board(&current_piece, board);             // adiciona a pe�a de volta
                    fall_timer = 0;                                     // segura a pe�a ap�s um movimento
                    }
                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_RIGHT){    // move a pe�a baseado no input do teclado
                    remove_piece_board(&current_piece, board);          // remove a pe�a antiga
                    current_piece.board_x += 1;                         // move para a direita
                    if(check_collisions(&current_piece, board) == 1) current_piece.board_x -= 1; // retorna caso nova posi��o colide
                    add_piece_board(&current_piece, board);             // adiciona a pe�a de volta
                    fall_timer = 0;                                     // segura a queda ap�s um movimento
                    }
                 else if (ev.keyboard.keycode ==  ALLEGRO_KEY_X){       // rotaciona a pe�a
                    remove_piece_board(&current_piece, board);          // remove pe�a para evitar conflitos
                    rotate_piece(&current_piece,1);                     // rootaciona a pe�a
                    if(check_wallkick_collision(&current_piece, board) == 0)    // caso a pe�a n�o v� colidir no wallkick
                    correct_piece_onboard(&current_piece);              // d� wallkick na pe�a
                    if(check_collisions(&current_piece, board) == 1){   // caso a pe�a v� colidir
                    rotate_piece(&current_piece, 0);                    // volta em caso de colis�o
                    }
                    add_piece_board(&current_piece, board);             // adiciona a pe�a
                    fall_timer = 0;                                     // pausa a queda
                 }
                 else if (ev.keyboard.keycode ==  ALLEGRO_KEY_Z){      // rotaciona a pe�a
                    remove_piece_board(&current_piece, board);          // remove pe�a para evitar conflitos
                    rotate_piece(&current_piece,0);                     // rotaciona a pe�a
                    if(check_wallkick_collision(&current_piece, board) == 0)    // caso a pe�a n�o v� colidir no wallkick
                    correct_piece_onboard(&current_piece);              // d� wallkick na pe�a
                    if(check_collisions(&current_piece, board) == 1){   // caso a pe�a colida na rota��o
                    rotate_piece(&current_piece, 1);                    // volta em caso de colis�o
                    }
                    add_piece_board(&current_piece, board);             // adiciona a pe�a
                    fall_timer = 0;                                     // pausa a queda
                 }
                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_DOWN){     // faz a pe�a descer bem r�pido
                    fall_speed = 1/FPS;                                 // aumenta velocidade de queda enquanto segurado
                }
                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_SPACE){     // faz a pe�a descer bem r�pido
                    while(fall_piece(&current_piece, board) == 0);                                // aumenta velocidade de queda enquanto segurado
                }
                break;

            case ALLEGRO_EVENT_KEY_UP:

                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
                    fall_speed = difficulty;

            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo

                redraw = true; // Marca que a tela precisa ser redesenhada
                fall_timer++;
                if(fall_timer > 1000) fall_timer = 0; // reinicia o timer para evitar problemas

                if(fall_timer > (FPS * fall_speed)){                       // timer de queda para as pe�as
                    fall_timer = 0;
                    if(fall_piece(&current_piece, board) == 1){      // faz as pe�as cairem, testa se chegaram no fundo

                        fall_speed = difficulty;
                        correct_piece_onboard(&current_piece);       // corrige escape de pe�as do mapa
                        current_piece.board_y = 0;                   // retorna a pe�a ao topo
                        current_piece.board_x = BOARD_COLS/2;        // retorna a pe�a pro centro

                        if(check_collisions(&current_piece, board) == 1){ // situa��o de derrota

                            loss_screen(allegro_contexts,points);
                            if (highscore_comparison(difficulty_entries, points, difficulty_level) == 1){
                                    get_new_highscore(allegro_contexts, points, difficulty_level, difficulty_entries);
                                    // debug_leaderboard(difficulty_entries);
                                }

                            for (int i = 0; i < BOARD_ROWS * BOARD_COLS; i++){
                            board[i] = 0;
                            }
                            randomize_piece(&current_piece);
                            points = 0;                      // reinicia o jogo em caso de derrota
                            dont_show_keybinds_again = 1;
                        }

                        randomize_piece(&current_piece);             // aleatoriza a pe�a ap�s a queda
                        points += clear_and_fall_rows(&current_piece, board);  // limpa as fileiras cheias e desce as superiores
                        add_piece_board(&current_piece, board);      // adiciona a pe�a no topo para manter integridade
                    }
                }
                break;
        }
                correct_piece_onboard(&current_piece); // corrige escape de pe�as do mapa


        if (redraw && al_is_event_queue_empty(allegro_contexts.queue)) { // Redesenho da tela
            redraw = false;

            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            int sprite_x = 0;        // Coordenada X no sprite da pe�a, sempre 0
            int sprite_y = 0;        // Coordenada Y no sprite da pe�a, 0, 1, 2, 3, 4 de acordo com a cor

            for(int i=0; i<20; i++){        // adiciona cada sprite dos blocos no display
                for(int j=0; j<10; j++){
                    sprite_y = board[i * BOARD_COLS + j] - 1; // define a cor do bloco de acordo com o tabuleiro
                    al_draw_scaled_bitmap(piece_sprite,
                                          sprite_x, 16 * sprite_y,
                                          PIECE_SPRITE_SIZE, PIECE_SPRITE_SIZE,
                                         (WIDTH - ((BOARD_SPRITE_WIDTH - 10) * sprite_scaling)/16)/2 + (sprite_scaling * j),
                                          (HEIGHT - ((BOARD_SPRITE_HEIGHT - 10) * sprite_scaling)/16)/2 + (sprite_scaling * i),
                                          sprite_scaling, sprite_scaling, 0);
                                            // desenha o bloco baseado na escala definida
                    }
                }
            al_draw_scaled_bitmap(board_sprite, 0, 0, 170, 330,
                                  (WIDTH - (BOARD_SPRITE_WIDTH * sprite_scaling)/16)/2,
                                  (HEIGHT - (BOARD_SPRITE_HEIGHT * sprite_scaling)/16)/2,
                                  (170 * sprite_scaling) / 16, (330 * sprite_scaling) / 16, 0);
                                            // desenha a borda do tabuleiro
                                            // o (-5) e (+10) s�o propor��es para alinhar o tabuleiro com as pe�as

            al_draw_textf(allegro_contexts.font, al_map_rgb(255,255,255), WIDTH - WIDTH/5, HEIGHT - HEIGHT/8, ALLEGRO_ALIGN_CENTER,"%04d points", points);

            if(points == 0 && dont_show_keybinds_again == 0){
                draw_keybinds(allegro_contexts, sprite_scaling, keybind_sprite);
            }

            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }

    // debug do tabuleiro no console
    for(int i=0; i<BOARD_ROWS; i++){
        for(int j=0; j<BOARD_COLS; j++){
            printf("%d", board[i * BOARD_COLS + j]);
        }
        printf("\n");
    }

}
