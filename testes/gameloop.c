
#include "gameloop.h"

int check_collisions(t_piece *piece, int *board){

    for(int i = 0; i < 4; i++){             // pega o formato da pe�a na matriz 4x4
        for(int j = 0; j < 4; j++){
            if(piece->shape[i][j] != 0){
                int row = piece->board_y + i;   // converte para as coordenadas do tabuleiro
                int col = piece->board_x + j;   // ''       ''      ''      ''      ''

            if(row >= BOARD_ROWS || col >= BOARD_COLS || col < 0)   // garante que est� nos limites do tabuleiro
                return 1;

            if(board[row * BOARD_COLS + col] != 0)  // testa se h� colis�o
                return 1;
            }
        }
    }
    return 0;   // retorna 0 se n�o h� colis�o
}

int add_piece_board(t_piece *piece, int *board){

    if(check_collisions(piece, board) == 0){    // testa se h� colis�o
        for(int i = 0; i < 4; i++){             // pega o formato da pe�a na matriz 4x4
            for(int j = 0; j < 4; j++){
                if(piece->shape[i][j] != 0){
                    int row = piece->board_y + i;   // converte para as coordenadas do tabuleiro
                    int col = piece->board_x + j;   // ''       ''      ''      ''      ''

                    if(row >= 0 && row < BOARD_ROWS && col >= 0 && col < BOARD_COLS)  // garante que est� nos limites do tabuleiro
                        board[row * BOARD_COLS + col] = piece->shape[i][j]; // adiciona a matriz 4x4 no tabuleiro
                }
            }

        }
        return 0;
    }
    return 1;   // retorna 1 se falhou / h� colis�o
}

void remove_piece_board(t_piece *piece, int *board){

    for(int i = 0; i < 4; i++){             // pega o formato da pe�a na matriz 4x4
        for(int j = 0; j < 4; j++){
            if(piece->shape[i][j] != 0){
                int row = piece->board_y + i;   // converte para as coordenadas do tabuleiro
                int col = piece->board_x + j;   // ''       ''      ''      ''      ''

            if(board[row * BOARD_COLS + col] != 0 && piece->shape[i][j] != 0)   // deleta o formato da pe�a na posi��o do tabuleiro
                board[row * BOARD_COLS + col] = 0;
            }
        }
    }
}

int fall_piece(t_piece *piece, int *board){     // atualiza a pe�a e faz a l�gica de queda

    remove_piece_board(piece, board);   // remove a pe�a atual
    piece->board_y += 1;                // desce 1 bloco
    int collision = check_collisions(piece, board); // testa se houve colis�o

    if(collision == 0){
        add_piece_board(piece, board);  // adiciona a pe�a 1 unidade abaixo
        return 0;
    }

    else if(collision == 1){
        piece->board_y -= 1;
        add_piece_board(piece, board);  // retorna a pe�a na posi��o anterior em caso de colis�o
        return 1;
    }
}

void correct_piece_onboard(t_piece *piece){ // retorna as pe�as pra borda do tabuleiro baseado em seus formatos

    for(int i = 0; i < 4; i++){             // pega o formato da pe�a na matriz 4x4
        for(int j = 0; j < 4; j++){
            if(piece->shape[i][j] != 0){
                if((piece->board_x + j) > 9){
                    piece->board_x = 9 - j;
                    return;
                }
                else if((piece->board_x + j) < 0){
                    piece->board_x = 0 - j;
                    return;
                }
            }
        }
    }
}

int clear_full_rows(int *board){

    int row_sum = 0;
    int flag = 0;

    for(int i=0; i < BOARD_ROWS; i++){
        for(int j=0; j < BOARD_COLS; j++){
            if(board[i * BOARD_COLS + j] != 0){
                row_sum++;
            }
        }
        if(row_sum == BOARD_COLS){
            for(int j=0; j < BOARD_COLS; j++){
                board[i * BOARD_COLS + j] = 0;
            }
            flag = i;
        }
        row_sum = 0;
    }
    return flag;
}

void fall_upper_rows(int *board, int row){

    for(int i=0; i < row; i++){
        for(int j=0; j < BOARD_COLS; j++){
            if(board[i * BOARD_COLS + j] != 0 && board[i + 1 * BOARD_COLS + j] == 0){
                board[i + 1 * BOARD_COLS + j] = board[i * BOARD_COLS + j];
                board[i * BOARD_COLS + j] = 0;
            }
        }
    }
}

