#include <allegro5/allegro.h>              // Biblioteca principal do Allegro
#include <allegro5/allegro_font.h>         // Suporte para fontes bitmap
#include <allegro5/allegro_ttf.h>          // Suporte para fontes TrueType
#include <allegro5/allegro_primitives.h>   // Desenho de primitivas (formas geométricas)
#include <allegro5/allegro_audio.h>        // Sistema de áudio
#include <allegro5/allegro_acodec.h>       // Suporte a codecs de áudio
#include <allegro5/allegro_image.h>        // Carregamento de imagens
#include <allegro5/mouse.h>                // Suporte ao mouse
#include <allegro5/keyboard.h>             // Suporte ao teclado
#include <time.h>                          // necessário para aleatorizar as peças
#include "gameloop_subroutines.h"          // lógica do loop de eventos

                                           // Constantes de configuração da janela e dos sprites
#define FPS 60                             // taxa de quadros
#define WIDTH 1500                         // Largura da janela
#define HEIGHT 1000                        // Altura da janela
#define PIECE_SPRITE_SIZE 16               // Tamanho (largura e altura) de cada peça na spritesheet
#define PIECE_SPRITE_COLS 1                // Número de colunas na spritesheet das peças
#define BOARD_SPRITE_ROWS 4                // Número de fileiras na spritesheet (cores diferentes)
#define BOARD_SPRITE_WIDTH 170
#define BOARD_SPRITE_HEIGHT 330

int main() {
    // Inicializa o Allegro e os módulos necessários
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_mouse();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();

    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);   // Cria uma janela com as dimensões especificadas

    // Cria uma fila de eventos para tratar interações do usuário e um temporizador para controle de FPS
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);  // 60 quadros por segundo

    // Registra as fontes de eventos que a fila vai monitorar
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Carrega uma imagem contendo múltimos sprites (spritesheet)
    ALLEGRO_BITMAP *piece_sprite = al_load_bitmap("../../sprites/tetris_sprite_16x16.png");
    ALLEGRO_BITMAP *board_sprite = al_load_bitmap("../../sprites/borda_tetris_170x330.png");
    al_convert_mask_to_alpha(piece_sprite, al_map_rgb(0, 0, 0));
    al_convert_mask_to_alpha(board_sprite, al_map_rgb(0, 0, 0));

    // Variáveis de controle da aplicação
    int falling = 1;      // Indica se a aplicação deve continuar executando
    bool redraw = true;       // Indica se a tela precisa ser redesenhada
    ALLEGRO_EVENT ev;         // Estrutura para eventos

    int board[BOARD_ROWS * BOARD_COLS]; // definido no gameloop.h
    int cleared_row = 0; // usado na função de descer as fileiras limpas
    int fall_timer = 0; // usado para queda de blocos
    int sprite_scaling = (HEIGHT * 40) / 1000; // usado para alterar o tamanho dos sprites
    float fall_speed = 0.50;    // maior diminui a velocidade, menor aumenta

    for (int i = 0; i < BOARD_ROWS * BOARD_COLS; i++){ // preencher tabuleiro base com vazio
        board[i] = 0;
        // 0 vazio
        // 1 vermelho
        // 2 amarelo
        // 3 azul
        // 4 verde
    }

    srand(time(NULL)); // necessário para aleatorizar as peças

    t_piece current_piece;  // cria a peça livre
    randomize_piece(&current_piece) ;// aleatoriza a primeira peça

    al_start_timer(timer);    // Inicia o temporizador

    while (falling == 1) {         // Loop principal do jogo

        al_wait_for_event(queue, &ev);   // Aguarda próximo evento na fila

       // remove_piece_board(&current_piece, board); // corrige o erro de sobreposição das peças

        // Tratamento dos eventos recebidos
        switch (ev.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                falling = false;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)          // termina o jogo
                    falling = 0;

                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_LEFT){     // move a peça baseado no input do teclado
                    remove_piece_board(&current_piece, board);          // remove a peça antiga
                    current_piece.board_x -= 1;                         // move para a esquerda
                    if(check_collisions(&current_piece, board) == 1) current_piece.board_x += 1; // retorna caso nova posição colide
                    add_piece_board(&current_piece, board);             // adiciona a peça de volta
                    fall_timer = 0;                                     // segura a peça após um movimento
                    }
                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_RIGHT){    // move a peça baseado no input do teclado
                    remove_piece_board(&current_piece, board);          // remove a peça antiga
                    current_piece.board_x += 1;                         // move para a direita
                    if(check_collisions(&current_piece, board) == 1) current_piece.board_x -= 1; // retorna caso nova posição colide
                    add_piece_board(&current_piece, board);             // adiciona a peça de volta
                    fall_timer = 0;                                     // segura a queda após um movimento
                    }
                 else if (ev.keyboard.keycode ==  ALLEGRO_KEY_X){       // rotaciona a peça
                    remove_piece_board(&current_piece, board);          // remove peça para evitar conflitos
                    rotate_piece(&current_piece,1);                     // rootaciona a peça
                    if(check_wallkick_collision(&current_piece, board) == 0)    // caso a peça não vá colidir no wallkick
                    correct_piece_onboard(&current_piece);              // dá wallkick na peça
                    if(check_collisions(&current_piece, board) == 1){   // caso a peça vá colidir
                    rotate_piece(&current_piece, 0);                    // volta em caso de colisão
                    }
                    add_piece_board(&current_piece, board);             // adiciona a peça
                    fall_timer = 0;                                     // pausa a queda
                 }
                 else if (ev.keyboard.keycode ==  ALLEGRO_KEY_Z){       // rotaciona a peça
                    remove_piece_board(&current_piece, board);          // remove peça para evitar conflitos
                    rotate_piece(&current_piece,0);                     // rotaciona a peça
                    if(check_wallkick_collision(&current_piece, board) == 0)    // caso a peça não vá colidir no wallkick
                    correct_piece_onboard(&current_piece);              // dá wallkick na peça
                    if(check_collisions(&current_piece, board) == 1){   // caso a peça colida na rotação
                    rotate_piece(&current_piece, 1);                    // volta em caso de colisão
                    }
                    add_piece_board(&current_piece, board);             // adiciona a peça
                    fall_timer = 0;                                     // pausa a queda
                 }
                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_DOWN){     // faz a peça descer bem rápido
                    fall_speed = 1/FPS;                                 // aumenta velocidade de queda enquanto segurado
                }

                break;

            case ALLEGRO_EVENT_KEY_UP:

                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
                    fall_speed = 0.5;


            case ALLEGRO_EVENT_TIMER: // controla os eventos por frame do jogo

                redraw = true; // Marca que a tela precisa ser redesenhada
                fall_timer++;
                if(fall_timer > 1000) fall_timer = 0; // reinicia o timer para evitar problemas

                if(fall_timer > (FPS * 3/15)){                       // timer de queda para as peças
                    fall_timer = 0;
                    if(fall_piece(&current_piece, board) == 1){      // faz as peças cairem, testa se chegaram no fundo
                        fall_speed = 0.5;
                        correct_piece_onboard(&current_piece);       // corrige escape de peças do mapa
                        current_piece.board_y = 0;                   // retorna a peça ao topo
                        current_piece.board_x = BOARD_COLS/2;        // retorna a peça pro centro
                        if(check_collisions(&current_piece, board) == 1){
                            for (int i = 0; i < BOARD_ROWS * BOARD_COLS; i++){
                            board[i] = 0;}
                            randomize_piece(&current_piece); // reinicia o jogo em caso de derrota
                            }
                            //falling = 0; // termina o jogo caso a peça inicie em colisão
                        randomize_piece(&current_piece);             // aleatoriza a peça após a queda
                        clear_and_fall_rows(&current_piece, board);  // limpa as fileiras cheias e desce as superiores
                        add_piece_board(&current_piece, board);      // adiciona a peça no topo para manter integridade
                    }
                }
                break;
        }
                correct_piece_onboard(&current_piece); // corrige escape de peças do mapa


        if (redraw && al_is_event_queue_empty(queue)) { // Redesenho da tela
            redraw = false;

            al_clear_to_color(al_map_rgb(10, 10, 10));  // Limpa a tela com uma cor escura

            int sprite_x = 0;        // Coordenada X no sprite da peça, sempre 0
            int sprite_y = 0;        // Coordenada Y no sprite da peça, 0, 1, 2, 3, 4 de acordo com a cor

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
                                            // o (-5) e (+10) são proporções para alinhar o tabuleiro com as peças

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
