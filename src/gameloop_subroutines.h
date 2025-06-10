#pragma once

#define BOARD_ROWS 20
#define BOARD_COLS 10

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
void clear_and_fall_rows(t_piece *piece, int *board);
void fall_upper_rows(int *board, int end_row);
int clear_full_rows(int *board);
void randomize_piece(t_piece *piece);
void rotate_piece(t_piece *piece, int side);
