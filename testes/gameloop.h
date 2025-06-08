#define BOARD_ROWS 20
#define BOARD_COLS 10

typedef struct{

    int board_x;
    int board_y;
    int shape[4][4];

}   t_piece;

int check_collisions(t_piece *piece, int *board);
int add_piece_board(t_piece *piece, int *board);
void remove_piece_board(t_piece *piece, int *board);
int fall_piece(t_piece *piece, int *board);
int gameloop();

