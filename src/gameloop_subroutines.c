
#include "gameloop_subroutines.h"

int check_collisions(t_piece *piece, int *board){

    for(int i = 0; i < 4; i++){             // pega o formato da peça na matriz 4x4
        for(int j = 0; j < 4; j++){
            if(piece->shape[i][j] != 0){
                int row = piece->board_y + i;   // converte para as coordenadas do tabuleiro
                int col = piece->board_x + j;   // ''       ''      ''      ''      ''

            if(row >= BOARD_ROWS || col >= BOARD_COLS || col < 0)   // garante que está nos limites do tabuleiro
                return 1;

            if(board[row * BOARD_COLS + col] != 0)  // testa se há colisão
                return 1;
            }
        }
    }
    return 0;   // retorna 0 se não há colisão
}

int add_piece_board(t_piece *piece, int *board){

    if(check_collisions(piece, board) == 0){    // testa se há colisão
        for(int i = 0; i < 4; i++){             // pega o formato da peça na matriz 4x4
            for(int j = 0; j < 4; j++){
                if(piece->shape[i][j] != 0){
                    int row = piece->board_y + i;   // converte para as coordenadas do tabuleiro
                    int col = piece->board_x + j;   // ''       ''      ''      ''      ''

                    if(row >= 0 && row < BOARD_ROWS && col >= 0 && col < BOARD_COLS)  // garante que está nos limites do tabuleiro
                        board[row * BOARD_COLS + col] = piece->shape[i][j]; // adiciona a matriz 4x4 no tabuleiro
                }
            }

        }
        return 0;
    }
    return 1;   // retorna 1 se falhou / há colisão
}

void remove_piece_board(t_piece *piece, int *board){

    for(int i = 0; i < 4; i++){             // pega o formato da peça na matriz 4x4
        for(int j = 0; j < 4; j++){
            if(piece->shape[i][j] != 0){
                int row = piece->board_y + i;   // converte para as coordenadas do tabuleiro
                int col = piece->board_x + j;   // ''       ''      ''      ''      ''

            if(board[row * BOARD_COLS + col] != 0 && piece->shape[i][j] != 0)   // deleta o formato da peça na posição do tabuleiro
                board[row * BOARD_COLS + col] = 0;
            }
        }
    }
}

int fall_piece(t_piece *piece, int *board){     // atualiza a peça e faz a lógica de queda

    remove_piece_board(piece, board);   // remove a peça atual
    piece->board_y += 1;                // desce 1 bloco
    int collision = check_collisions(piece, board); // testa se houve colisão

    if(collision == 0){
        add_piece_board(piece, board);  // adiciona a peça 1 unidade abaixo
        return 0;
    }

    else if(collision == 1){
        piece->board_y -= 1;
        add_piece_board(piece, board);  // retorna a peça na posição anterior em caso de colisão
        return 1;
    }
}

void correct_piece_onboard(t_piece *piece){ // retorna as peças pra borda do tabuleiro baseado em seus formatos

    for(int i = 0; i < 4; i++){             // pega o formato da peça na matriz 4x4
        for(int j = 0; j < 4; j++){
            if(piece->shape[i][j] != 0){
                if((piece->board_x + j) > 9){   // retorna a extremidade da peça para a borda 9 do tabuleiro
                    piece->board_x = 9 - j;
                    return;
                }
                else if((piece->board_x + j) < 0){  // retorna a extremidade da peça para a borda 0 do tabuleiro
                    piece->board_x = 0 - j;
                    return;
                }
            }
        }
    }
}

int clear_full_rows(int *board){    // remove fileiras cheias, essa função não funciona na primeira fileira por algum motivo

    int row_sum = 0;
    int clrd_row_height = -1;

    for(int i=0; i < BOARD_ROWS; i++){      // conta a quantidade de blocos em cada fileira
        for(int j=0; j < BOARD_COLS; j++){
            if(board[i * BOARD_COLS + j] != 0){
                row_sum++;
            }
        }
        if(row_sum == BOARD_COLS){              // se a fileira estiver cheia, zera a fileira
            for(int j=0; j < BOARD_COLS; j++){
                board[i * BOARD_COLS + j] = 0;
            }
            return clrd_row_height = i;
        }
        row_sum = 0;
    }
    return clrd_row_height;     // retorna a fileira zerada mais recente
}

void fall_upper_rows(int *board, int end_row){  // desce as fileiras até a espcificada no argumento, fileira mais no topo é a zero

    int can_fall = 1;

    while(can_fall == 1){
        can_fall = 0;
        for(int i= end_row - 1; i >= 0; i--){  // começa testando de baixo para cima
            for(int j=0; j < BOARD_COLS; j++){
                if(board[i * BOARD_COLS + j] != 0 && board[(i + 1) * BOARD_COLS + j] == 0 && (i + 1) < BOARD_ROWS){    // testa se é possível descer o bloco
                    board[(i + 1) * BOARD_COLS + j] = board[i * BOARD_COLS + j];               // desce o bloco
                    board[i * BOARD_COLS + j] = 0;
                    can_fall = 1;
                }
            }
        }
    }
}

void clear_and_fall_rows(t_piece *piece, int *board){ // subrotina loop que limpa e desce as fileiras

    int cleared_row;
    remove_piece_board(piece, board);   // remove a peça para impedir conflitos de posição

    while((cleared_row = clear_full_rows(board)) != -1){    // limpa cada fileira cheia e desce as acima
        fall_upper_rows(board, cleared_row);
    }
}



