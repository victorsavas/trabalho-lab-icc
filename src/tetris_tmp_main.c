#include <allegro5/allegro.h>              // Biblioteca principal do Allegro
#include <allegro5/allegro_font.h>         // Suporte para fontes bitmap
#include <allegro5/allegro_ttf.h>          // Suporte para fontes TrueType
#include <allegro5/allegro_primitives.h>   // Desenho de primitivas (formas geom�tricas)
#include <allegro5/allegro_audio.h>        // Sistema de �udio
#include <allegro5/allegro_acodec.h>       // Suporte a codecs de �udio
#include <allegro5/allegro_image.h>        // Carregamento de imagens
#include <allegro5/mouse.h>                // Suporte ao mouse
#include <allegro5/keyboard.h>             // Suporte ao teclado
#include "gameloop_subroutines.h"          // l�gica do loop de eventos

                                           // Constantes de configura��o da janela e dos sprites
#define WIDTH 700                       // Largura da janela
#define HEIGHT 500                        // Altura da janela
#define PIECE_SPRITE_SIZE 16               // Tamanho (largura e altura) de cada pe�a na spritesheet
#define PIECE_SPRITE_COLS 1                      // N�mero de colunas na spritesheet das pe�as
#define BOARD_SPRITE_ROWS 4                      // N�mero de fileiras na spritesheet (cores diferentes)
#define BOARD_SPRITE_WIDTH 170
#define BOARD_SPRITE_HEIGHT 330

int main() {
    // Inicializa o Allegro e os m�dulos necess�rios
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_mouse();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();


    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);   // Cria uma janela com as dimens�es especificadas

    // Cria uma fila de eventos para tratar intera��es do usu�rio e um temporizador para controle de FPS
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 10);  // 60 quadros por segundo

    // Registra as fontes de eventos que a fila vai monitorar
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Carrega uma imagem contendo m�ltimos sprites (spritesheet)
    ALLEGRO_BITMAP *piece_sprite = al_load_bitmap("../../sprites/tetris_sprite_16x16.png");
    ALLEGRO_BITMAP *board_sprite = al_load_bitmap("../../sprites/borda_tetris_170x330.png");
    al_convert_mask_to_alpha(piece_sprite, al_map_rgb(0, 0, 0));
    al_convert_mask_to_alpha(board_sprite, al_map_rgb(0, 0, 0));

    // Vari�veis de controle da aplica��o
    bool falling = true;      // Indica se a aplica��o deve continuar executando
    bool redraw = true;       // Indica se a tela precisa ser redesenhada
    ALLEGRO_EVENT ev;         // Estrutura para eventos

    int board[BOARD_ROWS * BOARD_COLS]; // definido no gameloop.h
    int cleared_row = 0; // usado na fun��o de descer as fileiras limpas

    for (int i = 0; i < BOARD_ROWS * BOARD_COLS; i++){ // preencher tabuleiro base
        board[i] = 0;
        // 0 vazio
        // 1 vermelho
        // 2 amarelo
        // 3 azul
        // 4 verde
    }

    t_piece current_piece = { // struct da pe�a atual
        .board_x = BOARD_COLS/2,
        .board_y = 0,
        .shape = {
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
        }       // formato da pe�a na matriz shape
    };

    al_start_timer(timer);    // Inicia o temporizador

    while (falling) {         // Loop principal do jogo

        al_wait_for_event(queue, &ev);   // Aguarda pr�ximo evento na fila

        remove_piece_board(&current_piece, board); // corrige o erro de sobreposi��o das pe�as

        // Tratamento dos eventos recebidos
        switch (ev.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                falling = false;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)          // termina o jogo
                    falling = false;

                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_LEFT){     // move a pe�a baseado no input do teclado
                    current_piece.board_x -= 1;
                    }
                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_RIGHT){    // move a pe�a baseado no input do teclado
                    current_piece.board_x += 1;
                    }
                break;

            case ALLEGRO_EVENT_TIMER:
                redraw = true; // Marca que a tela precisa ser redesenhada
                break;
        }

        correct_piece_onboard(&current_piece); // corrige escape de pe�as do mapa

        if(fall_piece(&current_piece, board) == 1){  // faz as pe�as cairem, definido em gameloop.c

        current_piece.board_y = 0;  // retorna a pe�a ao topo
    //  current_piece.board_x = BOARD_COLS/2; // retorna a pe�a pro centro, com problema atualmente

        clear_and_fall_rows(&current_piece, board);  // limpa as fileiras cheias e desce as superiores
        }

        if (redraw && al_is_event_queue_empty(queue)) { // Redesenho da tela
            redraw = false;

            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            int sprite_x = 0;        // Coordenada X no sprite da pe�a, sempre 0
            int sprite_y = 0;        // Coordenada Y no sprite da pe�a, 0, 1, 2, 3, 4 de acordo com a cor

            for(int i=0; i<20; i++){        // adiciona cada sprite dos blocos no display
                for(int j=0; j<10; j++){
                    sprite_y = board[i * BOARD_COLS + j] - 1; // define a cor do bloco de acordo com o tabuleiro
                    al_draw_bitmap_region(piece_sprite, sprite_x, 16 * sprite_y, PIECE_SPRITE_SIZE, PIECE_SPRITE_SIZE,
                                         (WIDTH - PIECE_SPRITE_SIZE - BOARD_SPRITE_WIDTH)/2 + (PIECE_SPRITE_SIZE * j), HEIGHT/5 + PIECE_SPRITE_SIZE + (PIECE_SPRITE_SIZE * i), 0);
                                            // desenha o bloco
                    }
                }
            al_draw_bitmap_region(board_sprite, 0, 0, 170, 330,
                                         (WIDTH - PIECE_SPRITE_SIZE - BOARD_SPRITE_WIDTH)/2 - 5, HEIGHT/5 + 10, 0);
                                            // desenha a borda do tabuleiro
                                            // o (-5) e (+10) s�o propor��es para alinhar o tabuleiro com as pe�as

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

    // Libera recursos alocados
    al_destroy_bitmap(piece_sprite);
    al_destroy_bitmap(board_sprite);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}
