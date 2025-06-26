#pragma once

#define BOARD_ROWS 20
#define BOARD_COLS 10
#define FPS 60                             // taxa de quadros
#define WIDTH 1500                         // Largura da janela
#define HEIGHT 1000                        // Altura da janela
#define PIECE_SPRITE_SIZE 16               // Tamanho (largura e altura) de cada peça na spritesheet
#define PIECE_SPRITE_COLS 1                // Número de colunas na spritesheet das peças
#define BOARD_SPRITE_ROWS 4                // Número de fileiras na spritesheet (cores diferentes)
#define BOARD_SPRITE_WIDTH 170
#define BOARD_SPRITE_HEIGHT 330

typedef struct{
    int board_x;
    int board_y;
    int shape[4][4];

}   t_piece;

int fall_piece(t_piece *piece, int *board);
int check_collisions(t_piece *piece, int *board);
int add_piece_board(t_piece *piece, int *board);
void remove_piece_board(t_piece *piece, int *board);
void correct_piece_onboard(t_piece *piece);
int clear_and_fall_rows(t_piece *piece, int *board);
void fall_upper_rows(int *board, int end_row);
int clear_full_rows(int *board);
void randomize_piece(t_piece *piece);
void rotate_piece(t_piece *piece, int side);
int check_wallkick_collision(t_piece *piece, int *board);
