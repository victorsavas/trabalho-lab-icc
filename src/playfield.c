#include <stdlib.h>
#include "game.h"
#include "global_state.h"

#define BLOCK_LENGTH 30

#define PLAYFIELD_WIDTH  BLOCK_LENGTH * 10
#define PLAYFIELD_HEIGHT BLOCK_LENGTH * 20

#define PLAYFIELD_X (WIDTH  - PLAYFIELD_WIDTH) / 2
#define PLAYFIELD_Y (HEIGHT - PLAYFIELD_HEIGHT) / 2

#define NEXT_TIME 1.0
#define HOR_TIME  0.05
#define FAST_FALL 0.05

// Enumeração para cada tipo de tetrominó. Os valores correspondem aos índices na tabela de tetrominós.

typedef enum TetrominoType {
    TETROMINO_I=0,
    TETROMINO_J=1,
    TETROMINO_L=2,
    TETROMINO_O=3,
    TETROMINO_S=4,
    TETROMINO_T=5,
    TETROMINO_Z=6
} TetrominoType;

// Estrutura de dados contendo informação sobre um tetrominó (peças do jogo de formatos variados compostas por quatro blocos).

typedef struct Tetromino {
    int x;                  // Posição relativa horizontal do tetrominó ao tabuleiro.
    int y;                  // Posição relativa vertical do tetrominó ao tabuleiro.

    TetrominoType type;     // Tipo do tetrominó (I, J, L O, S, T ou Z)
    int state;              // Estado de rotação do tetrominó. Admite apenas 4 valores: 0 para sua posição original, 1 após uma rotação horária de 90º a partir da posição original, 2 para uma rotação em 180º e 3 para uma rotação de 90º no sentido anti-horário.

    int shape[16];          // Matriz contendoa forma do tetrominó. Índices zero são vazios e índices não-zero indicam blocos constituíntes do tetrominó. Cada valor não-zero possui uma cor respectiva atribuída.
} Tetromino;

// Estrutura de dados contendo todos os dados relevantes do tabuleiro.

typedef struct Playfield {
    int board[200];                 // Tabuleiro

    Tetromino tetromino;            // Tetrominó ativo

    unsigned int lines_cleared;     // Número de linhas limpas
    unsigned int points;            // Número de pontos

    double fall_timer;              // Cronômetro para queda do tetrominó ativo
    double next_timer;              // Cronômetro para adesão do tetrominó ao tabuleiro uma vez que atinge os blocos abaixo

    double horizontal_move_timer;   // Cronômetro que regula a "sensibilidade" do movimento horizontal dos tetrominós
    double fast_fall_timer;         // Cronômetro que regula a velocidade da queda rápida ao se pressionar para baixo

    TetrominoType queue[5];         // Fila dos índices próximos cinco tetrominós

    TetrominoType hand;             // Índice do tetrominó na mão
    int toggle_hand;                // Flag indicando que já se alternou o tetrominó na mão.
} Playfield;

// Tabela contendo cada tetrominó, em sua posição relativa inicial no tabuleiro.

static const Tetromino tetromino_table[7] = {

    {
        .type = TETROMINO_I,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            0,0,0,0,
            1,1,1,1,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_J,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            2,0,0,0,
            2,2,2,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_L,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            0,0,3,0,
            3,3,3,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_O,

        .x = 4,
        .y = -1,

        .state = 0,

        .shape = {
            4,4,0,0,
            4,4,0,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_S,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            0,5,5,0,
            5,5,0,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_T,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            0,6,0,0,
            6,6,6,0,
            0,0,0,0,
            0,0,0,0
        }
    },

    {
        .type = TETROMINO_Z,

        .x = 3,
        .y = -1,

        .state = 0,

        .shape = {
            7,7,0,0,
            0,7,7,0,
            0,0,0,0,
            0,0,0,0
        }
    }
};

// Tabela das velocidades das quedas das peças em função da dificuldade.

static const double fall_time_difficulty_table[3] = {1.0, 0.2, 0.08};

// Tabela de pontos somados em função do número de linhas limpas

static const int scoring_table[5] = { 0, 40, 100, 300, 1200 };

// Tabelas de referência para o algorítmo SRS (Super Rotation System) de rotação.
// Rotações positivas são em sentido horário.
// Os índices correspondem à variável state do tetrominó.

// O tetrominó O não gira.
// Os tetrominós J, L, S, T e Z giram em torno de seu bloco central e usam a tabela wall_kick_tests_3x3 para a realização do wall kick.
// O tetrominó I gira em torno do centro de sua matriz e usa a tabela wall_kick_tests_i para a realização do wall kick.

static const int wall_kick_tests_3x3[4][4][2] = {
    // 0 >> 1 (+) ou 1 >> 0 (-)
    {
        {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}
    },

    // 1 >> 2 (+) ou 2 >> 1 (-)
    {
        {1, 0}, {1, 1}, {0, -2}, {1, -2}
    },

    // 2 >> 3 (+) ou 3 >> 2 (-)
    {
        {1, 0}, {1, -1}, {0, 2}, {1, 2}
    },

    // 3 >> 0 (+) ou 0 >> 3 (-)
    {
        {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}
    }
};

static const int wall_kick_tests_i[4][4][2] = {
    // 0 >> 1 (+) ou 1 >> 0 (-)
    {
        {-2, 0}, {1, 0}, {-2, 1}, {1, -2}
    },

    // 1 >> 2 (+) ou 2 >> 1 (-)
    {
        {-1, 0}, {2, 0}, {-1, -2}, {2, 1}
    },

    // 2 >> 3 (+) ou 3 >> 2 (-)
    {
        {2, 0}, {-1, 0}, {2, -1}, {-1, 2}
    },

    // 3 >> 0 (+) ou 0 >> 3 (-)
    {
        {1, 0}, {-2, 0}, {1, 2}, {-2, -1}
    }
};

// Subrotinas do tabuleiro

static void playfield_reset(Playfield *playfield, int difficulty);                                      // Reinicia o tabuleiro.
static int playfield_update(Playfield *playfield, Input *input, double delta_time, int difficulty);     // Atualiza o estado do tabuleiro. Retorna 0 normalmente e retorna 1 caso o jogador perca o jogo.
static void playfield_draw(AllegroContext *allegro, Playfield *playfield);                              // Desenha o tabuleiro na tela.

// Subrotinas de menus

static GameMode pause_menu(AllegroContext *allegro, Input *input);                  // Menu de pause. Retorna MODE_MAIN_MENU ou MODE_PLAYFIELD a depender da escolha do jogador.
static GameMode loss_screen(AllegroContext *allegro, Input *input, int points);     // Exibe uma tela de game over ao perder o jogo. Caso o jogador atinja um novo récorde, chama a função get_new_highscore() e salva sua pontuação.

// Desenho do tutorial na tela

static void draw_keybinds(AllegroContext *allegro, int sprite_scaling);

// Subrotinas subordinadas de playfield_update

static void tetromino_rotate_counterclockwise(Tetromino *tetromino);    // Rotaciona um tetrominó em sentido anti-horário
static void tetromino_rotate_clockwise(Tetromino *tetromino);           // Rotaciona um tetrominó em sentido anti-horário

static int tetromino_collision_check(Playfield *playfield, Tetromino tetromino, int x_offset, int y_offset);    // Verifica se um tetrominó em uma posição relativa colide com o tabuleiro.
static int tetromino_next(Playfield *playfield, Tetromino *updated_tetromino);                                  // Função que insere o tetrominó ativo no tabuleiro e puxa da fila o próximo, limpando linhas preenchidas.

static void tetromino_wall_kick(Playfield *playfield, Tetromino updated_tetromino, int rotation);   // Função que corrige a posição de um tetrominó rotacionado. O parâmetro rotation é 1 para rotações horárias e -1 para rotações anti-horárias.
static void playfield_dequeue(Playfield *playfield);                                                // Função que puxa o próximo tetrominó da fila e insere um novo tetrominó aleatorio em seu fim.

static int playfield_clear_contiguous_lines(Playfield *playfield);                                      // Limpa linhas contíguas do tabuleiro. Retorna o número de linhas limpas.

// Obtenção de uma nova pontuação máxima

static GameMode get_new_highscore(GlobalState *global, AllegroContext *allegro, int points);

// Implementação das subrotinas

GameMode game_playfield(GlobalState *global, AllegroContext *allegro, Input *input)
{
    // Flag que indica que deve-se exibir o tutorial na tela

    int show_tutorial = 1;

    // Variáveis para o cálculo do tempo entre cada frame (delta_time)

    double current_time;
    double last_time = al_get_time();
    double delta_time;

    // Inicialização do tabuleiro

    Playfield playfield;
    playfield_reset(&playfield, global->selected_difficulty);

    // Loop do tabuleiro

    while (1) {
        // Recebe o próximo evento e as entradas do teclado

        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        // Fecha o jogo ao clicar no X

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return MODE_EXIT;
        }

        // Abre o menu de pause

        if (input->escape_pressed) {
            input_pressed_flush(input);
            GameMode mode = pause_menu(allegro, input);

            if (mode != MODE_PLAYFIELD) {
                return mode;
            }
        }

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            // Cálculo do delta_time

            current_time = al_get_time();
            delta_time = current_time - last_time;

            // Fullscreen

            if (input->f4_pressed) {
                toggle_fullscreen(allegro);
            }

            // Execução da lógica do frame
            // Verifica se o jogador perdeu


            int loss = playfield_update(&playfield, input, delta_time, global->selected_difficulty);
            input_pressed_flush(input);

            // Remove o tutorial da tela ao se limpar uma linha

            if (playfield.lines_cleared != 0) {
                show_tutorial = 0;
            }

            // Reinicia o tabuleiro ao perder

            if (loss) {
                GameMode mode = loss_screen(allegro, input, playfield.points);

                if (mode == MODE_EXIT) {
                    return MODE_EXIT;
                }

                // Salva um novo recorde caso ocorra

                if (highscore_comparison(global, playfield.points)){
                    GameMode mode = get_new_highscore(global, allegro, playfield.points);
                    debug_leaderboard(global->leaderboard);

                    if (mode == MODE_EXIT) {
                        return MODE_EXIT;
                    }
                }

                playfield_reset(&playfield, global->selected_difficulty);
            }

            // da tela e registro do tempo do frame

            allegro->redraw = 1;
            last_time = current_time;
        }

        // Redesenho da tela

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            allegro->redraw = 0;

            // Limpa o fundo

            al_clear_to_color(BACKGROUND_COLOR);

            // Desenha o tabuleiro

            playfield_draw(allegro, &playfield);

            // Desenha o tutorial

            if (show_tutorial) {
                draw_keybinds(allegro, HEIGHT / 32.0);
            }

            // Atualiza a tela

            al_flip_display();
        }
    }

    // Essa linha nunca é atingida

    return MODE_EXIT;
}

void tetromino_rotate_counterclockwise(Tetromino *tetromino)
{
    // Atualiza o estado de rotação do tetrominó

    tetromino->state = (tetromino->state + 3) % 4;

    switch (tetromino->type) {
    case TETROMINO_O:
        // Não há rotação
        return;
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
        // Rotação ao redor do bloco central

        int shape[] = {
            tetromino->shape[2], tetromino->shape[6], tetromino->shape[10], 0,
            tetromino->shape[1], tetromino->shape[5], tetromino->shape[9],  0,
            tetromino->shape[0], tetromino->shape[4], tetromino->shape[8],  0,
            0,                   0,                   0,                    0
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_I:
    {
        // Rotação ao redor do centro da matriz

        int shape[] = {
            tetromino->shape[3], tetromino->shape[7], tetromino->shape[11], tetromino->shape[15],
            tetromino->shape[2], tetromino->shape[6], tetromino->shape[10], tetromino->shape[14],
            tetromino->shape[1], tetromino->shape[5], tetromino->shape[9],  tetromino->shape[13],
            tetromino->shape[0], tetromino->shape[4], tetromino->shape[8],  tetromino->shape[12]
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    }
}

void tetromino_rotate_clockwise(Tetromino *tetromino)
{
    // Atualiza o estado da rotação do tetrominó

    tetromino->state = (tetromino->state + 1) % 4;

    switch (tetromino->type) {
    case TETROMINO_O:
        // Não há rotação
        return;
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
        // Rotação em torno do bloco central

        int shape[] = {
            tetromino->shape[8],  tetromino->shape[4], tetromino->shape[0],  0,
            tetromino->shape[9],  tetromino->shape[5], tetromino->shape[1],  0,
            tetromino->shape[10], tetromino->shape[6], tetromino->shape[2],  0,
            0,                    0,                   0,                    0
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    case TETROMINO_I:
    {
        // Rotação em torno do centro da matriz

        int shape[] = {
            tetromino->shape[12], tetromino->shape[8], tetromino->shape[4], tetromino->shape[0],
            tetromino->shape[13], tetromino->shape[9], tetromino->shape[5], tetromino->shape[1],
            tetromino->shape[14], tetromino->shape[10],  tetromino->shape[6], tetromino->shape[2],
            tetromino->shape[15], tetromino->shape[11],  tetromino->shape[7], tetromino->shape[3]
        };

        memcpy(tetromino->shape, shape, sizeof(shape));
        break;
    }
    }
}

int tetromino_collision_check(Playfield *playfield, Tetromino tetromino, int x_offset, int y_offset)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // Se a região não estiver preenchida por um bloco, ignore

            if (tetromino.shape[i + j * 4] == 0) {
                continue;
            }

            int x = tetromino.x + i + x_offset;
            int y = tetromino.y + j + y_offset;

            // Se o bloco estiver fora do tabuleiro, há colisão, exceto se estiver em seu topo

            if (x < 0 || x >= 10) {
                return 1;
            }

            if (y < 0) {
                continue;
            }

            if (y >= 20) {
                return 1;
            }

            // Se houver sobreposição de blocos, há colisão

            if (playfield->board[x + y * 10] != 0) {
                return 1;
            }
        }
    }

    // Se não houver sobreposição entre blocos, não há colisão

    return 0;
}

void playfield_dequeue(Playfield *playfield)
{
    // Insere o próximo tetrominó no tabuleiro

    playfield->tetromino = tetromino_table[playfield->queue[0]];

    // Avança a fila

    for (int i = 0; i < 4; i++) {
        playfield->queue[i] = playfield->queue[i + 1];
    }

    // Insere um novo tetrominó no fim da fila

    playfield->queue[4] = rand() % 7;
}

int playfield_clear_contiguous_lines(Playfield *playfield)
{
    int lines_cleared = 0;

    // Verificando se há linhas preenchidas

    int y_0;

    for (int y = 19; y >= 0; y--) {
        int x;

        for (x = 0; x < 10; x++) {
            if (playfield->board[x + y * 10] == 0) {
                if (lines_cleared != 0) {
                    y_0 = y;
                    y = 0;
                }
                break;
            }
        }

        // Linha preenchida

        if (x == 10) {
            lines_cleared++;
        }
    }

    // Limpando as linhas

    for (int y = y_0; y >= 0; y--) {
        for (int x = 0; x < 10; x++) {
            playfield->board[x + (y + lines_cleared) * 10] = playfield->board[x + y * 10];
        }
    }

    for (int y = 0; y < lines_cleared; y++) {
        for (int x = 0; x < 10; x++) {
            playfield->board[x + y * 10] = 0;
        }
    }

    return lines_cleared;
}

int tetromino_next(Playfield *playfield, Tetromino *updated_tetromino)
{
    Tetromino *tetromino = &playfield->tetromino;

    // Adere o tetrominó ao tabuleiro

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int color = tetromino->shape[i + j * 4];

            if (color == 0) {
                continue;
            }

            int x = tetromino->x + i;
            int y = tetromino->y + j;

            // Caso o tetrominó colocado esteja parcialmente fora da tela, o jogador perde

            if (x < 0 || x >= 10 || y < 0 || y >= 20) {
                return 1;
            }

            playfield->board[x + y * 10] = color;
        }
    }

    // Avança a fila e recebe o novo tetrominó

    playfield_dequeue(playfield);

    // Verificando se há linhas preenchidas

    int lines_cleared = playfield_clear_contiguous_lines(playfield);

    // A subrotina é chamada uma segunda vez para verificar se há mais linhas não contíguas à que foi limpa anteriormente

    lines_cleared += playfield_clear_contiguous_lines(playfield);

    // Somando os pontos

    playfield->lines_cleared += lines_cleared;
    playfield->points += scoring_table[lines_cleared];
    playfield->toggle_hand = 1;

    // Caso o novo tetrominó colida com o tabuleiro, o jogador perde

    *updated_tetromino = *tetromino;

    if (tetromino_collision_check(playfield, playfield->tetromino, 0, 0)) {
        return 1;
    }

    return 0;
}

void tetromino_wall_kick(Playfield *playfield, Tetromino updated_tetromino, int rotation)
{
    // Implementação do algorítimo Super Rotation System de rotações

    Tetromino *tetromino = &playfield->tetromino;

    // Tetrominós O não giram

    if (tetromino->type == TETROMINO_O) {
        return;
    }

    // Caso não haja colisão, não é efetuado wallkicks

    if (!tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
        *tetromino = updated_tetromino;
        return;
    }

    // Verificação dos casos de wallkick

    int index;

    if (rotation == 1) {
        index = tetromino->state;
    } else {
        index = updated_tetromino.state;
    }

    const int (*tests)[2];

    if (tetromino->type == TETROMINO_I) {
        tests = wall_kick_tests_i[index];
    } else {
        tests = wall_kick_tests_3x3[index];
    }

    for (int i = 0; i < 4; i++) {
        // Para rotações anti-horárias (rotation=-1), o wallkick é inverso ao das rotações horárias (rotation=1).

        int x_offset = tests[i][0] * rotation;
        int y_offset = tests[i][1] * rotation;

        // Caso haja uma rotação possível, ela é executada

        if (!tetromino_collision_check(playfield, updated_tetromino, x_offset, y_offset)) {
            *tetromino = updated_tetromino;

            tetromino->x += x_offset;
            tetromino->y += y_offset;

            return;
        }
    }

    // Caso não haja wallkick de resolução, não ocorre rotação.
}

void playfield_draw(AllegroContext *allegro, Playfield *playfield)
{
    // Borda do mapa

    al_draw_scaled_bitmap(
        allegro->bitmap_playfield,
        0,
        0,
        170,
        330,
        (PLAYFIELD_X - BLOCK_LENGTH * 5 / 16) * allegro->scale + allegro->x_offset,
        (PLAYFIELD_Y - BLOCK_LENGTH * 5 / 16) * allegro->scale + allegro->y_offset,
        (PLAYFIELD_WIDTH + BLOCK_LENGTH * 5 / 8) * allegro->scale,
        (PLAYFIELD_HEIGHT + BLOCK_LENGTH * 5 / 8) * allegro->scale,
        0
    );

    // Blocos do mapa

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            int color = playfield->board[i + 10 * j];

            if (color != 0) {
                al_draw_scaled_bitmap(
                    allegro->bitmap_blocks,
                    0,
                    16 * (color - 1),
                    16,
                    16,
                    (PLAYFIELD_X + i * BLOCK_LENGTH) * allegro->scale + allegro->x_offset,
                    (PLAYFIELD_Y + j * BLOCK_LENGTH) * allegro->scale + allegro->y_offset,
                    BLOCK_LENGTH * allegro->scale,
                    BLOCK_LENGTH * allegro->scale,
                    0
                );
            }
        }
    }

    // Sombra do tetrominó ativo

    Tetromino ghost_tetromino = playfield->tetromino;

    for (int i = 0; i < 20; i++) {
        if (tetromino_collision_check(playfield, ghost_tetromino, 0, 1)) {
            break;
        }
        ghost_tetromino.y++;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int color = ghost_tetromino.shape[i + 4 * j];

            if (color != 0) {
                al_draw_filled_rectangle(
                    (PLAYFIELD_X + (i + ghost_tetromino.x) * BLOCK_LENGTH) * allegro->scale + allegro->x_offset,
                    (PLAYFIELD_Y + (j + ghost_tetromino.y) * BLOCK_LENGTH) * allegro->scale + allegro->y_offset,
                    (PLAYFIELD_X + (i + ghost_tetromino.x + 1) * BLOCK_LENGTH) * allegro->scale + allegro->x_offset,
                    (PLAYFIELD_Y + (j + ghost_tetromino.y + 1) * BLOCK_LENGTH) * allegro->scale + allegro->y_offset,
                    al_map_rgb(39, 37, 115)
                );
            }
        }
    }

    // Tetrominó ativo

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int color = playfield->tetromino.shape[i + 4 * j];

            if (color != 0) {
                al_draw_scaled_bitmap(
                    allegro->bitmap_blocks,
                    0,
                    16 * (color - 1),
                    16,
                    16,
                    (PLAYFIELD_X + (i + playfield->tetromino.x) * BLOCK_LENGTH) * allegro->scale + allegro->x_offset,
                    (PLAYFIELD_Y + (j + playfield->tetromino.y) * BLOCK_LENGTH) * allegro->scale + allegro->y_offset,
                    BLOCK_LENGTH * allegro->scale,
                    BLOCK_LENGTH * allegro->scale,
                    0
                );
            }
        }
    }

    // Linhas e pontos

    al_draw_textf(
        allegro->font,
        al_map_rgb(255,255,255),
        (WIDTH - WIDTH/5) * allegro->scale + allegro->x_offset,
        (HEIGHT - HEIGHT/8 - 35) * allegro->scale + allegro->y_offset,
        ALLEGRO_ALIGN_CENTER,
        "LINES:%d", playfield->lines_cleared
    );

    al_draw_textf(
        allegro->font,
        al_map_rgb(255,255,255),
        (WIDTH - WIDTH/5) * allegro->scale + allegro->x_offset,
        (HEIGHT - HEIGHT/8) * allegro->scale + allegro->y_offset,
        ALLEGRO_ALIGN_CENTER,
        "POINTS:%06d", playfield->points
    );

    // Fila de tetrominós

    for (int k = 0; k < 5; k++) {
        Tetromino t = tetromino_table[playfield->queue[k]];

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int color = t.shape[i + 4 * j];

                if (color != 0) {
                    al_draw_scaled_bitmap(
                        allegro->bitmap_blocks,
                        0,
                        16 * (color - 1),
                        16,
                        16,
                        (PLAYFIELD_X + (i + 12) * BLOCK_LENGTH) * allegro->scale + allegro->x_offset,
                        (PLAYFIELD_Y + (j + k * 3 + 2) * BLOCK_LENGTH) * allegro->scale + allegro->y_offset,
                        BLOCK_LENGTH * allegro->scale,
                        BLOCK_LENGTH * allegro->scale,
                        0
                    );
                }
            }
        }
    }

    // Tetrominó na "mão"

    if (playfield->hand != -1) {
        Tetromino hand_tetromino = tetromino_table[playfield->hand];

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int color = hand_tetromino.shape[i + 4 * j];

                if (color != 0) {
                    al_draw_scaled_bitmap(
                        allegro->bitmap_blocks,
                        0,
                        16 * (color - 1),
                        16,
                        16,
                        (PLAYFIELD_X + (i - 5) * BLOCK_LENGTH) * allegro->scale + allegro->x_offset,
                        (PLAYFIELD_Y + (j + 1) * BLOCK_LENGTH) * allegro->scale + allegro->y_offset,
                        BLOCK_LENGTH * allegro->scale,
                        BLOCK_LENGTH * allegro->scale,
                        0
                    );
                }
            }
        }
    }
}

int playfield_update(Playfield *playfield, Input *input, double delta_time, int difficulty)
{
    // O cronômetro da subrotina é acionado em função da dificuldade

    Tetromino *tetromino = &playfield->tetromino;

    // Inverte o tetrominó na mão

    if (input->c_pressed && playfield->toggle_hand) {
        TetrominoType hand = playfield->hand;

        if (hand == -1) {
            playfield->hand = tetromino->type;
            playfield_dequeue(playfield);
        } else {
            playfield->hand = tetromino->type;
            *tetromino = tetromino_table[hand];
        }

        playfield->toggle_hand = 0;
    }

    int x_move = 0;
    int rotation = 0;

    Tetromino updated_tetromino = *tetromino;

    // Queda instantânea

    if (input->space_pressed) {
        for (int i = 0; i < 20; i++) {
            if (tetromino_collision_check(playfield, updated_tetromino, 0, 1)) {
                *tetromino = updated_tetromino;
                break;
            }
            updated_tetromino.y++;
        }

        tetromino_next(playfield, &updated_tetromino);
        updated_tetromino = *tetromino;
    }

    // Queda rápida

    if ((input->down_down && playfield->fast_fall_timer <= 0) || input->down_pressed) {
        updated_tetromino.y++;

        playfield->fast_fall_timer = FAST_FALL;
        playfield->fall_timer = fall_time_difficulty_table[difficulty];

        if (tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
            updated_tetromino.y--;
        } else {
            playfield->next_timer = NEXT_TIME;
        }
    }

    // Queda lenta

    if (playfield->fall_timer <= 0.0) {
        updated_tetromino.y++;
        playfield->fall_timer = fall_time_difficulty_table[difficulty];

        if (tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
            updated_tetromino.y--;
        } else {
            playfield->next_timer = NEXT_TIME;
        }
    }

    // Cheque de aderência

    if (tetromino_collision_check(playfield, updated_tetromino, 0, 1)) {
        playfield->next_timer -= delta_time;

        if (playfield->next_timer <= 0) {
            playfield->next_timer = NEXT_TIME;
            int loss = tetromino_next(playfield, &updated_tetromino);

            if (loss) {
                return 1;
            }
        }
    }

    // Cheque do movimento horizontal

    if ((input->left_down && playfield->horizontal_move_timer <= 0) || input->left_pressed) {
        updated_tetromino.x--;
        playfield->horizontal_move_timer = HOR_TIME;

        x_move--;
    }

    if ((input->right_down && playfield->horizontal_move_timer <= 0) || input->right_pressed) {
        updated_tetromino.x++;
        playfield->horizontal_move_timer = HOR_TIME;

        x_move++;
    }

    if (x_move != 0) {
        if (tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
            updated_tetromino.x -= x_move;
        }
    }

    *tetromino = updated_tetromino;

    // Cheque da rotação

    if (input->z_pressed) {
        tetromino_rotate_clockwise(&updated_tetromino);

        rotation++;
    }

    if (input->x_pressed) {
        tetromino_rotate_counterclockwise(&updated_tetromino);

        rotation--;
    }

    // Wall kick

    if (rotation) {
        tetromino_wall_kick(playfield, updated_tetromino, rotation);
    }

    // Atualização dos cronometros

    if (playfield->fall_timer >= 0) {
        playfield->fall_timer -= delta_time;
    }

    if (playfield->horizontal_move_timer) {
        playfield->horizontal_move_timer -= delta_time;
    }

    if (playfield->fast_fall_timer) {
        playfield->fast_fall_timer -= delta_time;
    }

    return 0;
}

void playfield_reset(Playfield *playfield, int difficulty)
{
    // Limpa o tabuleiro, inicializa os cronômetros e a fila de tetrominós

    *playfield = (Playfield){
        .board = {0},

        .tetromino = tetromino_table[rand() % 7],

        .lines_cleared = 0,
        .points = 0,

        .fall_timer = fall_time_difficulty_table[difficulty],
        .next_timer = 1.0,

        .horizontal_move_timer = 0.0,
        .fast_fall_timer = 0.0,

        .hand = -1,
        .toggle_hand = 1
    };

    // Inicializa a fila

    for (int i = 0; i < 5; i++) {
        playfield->queue[i] = rand() % 7;
    }
}

GameMode pause_menu(AllegroContext *allegro, Input *input)
{
    // Índice do botão selecionado

    int selected = 0;

    // Botões de continuar e sair

    const t_button pause_button[2] = {
        {
            .origin_x = WIDTH/3 + WIDTH/18,
            .origin_y = HEIGHT/3 + HEIGHT/18,

            .end_x = WIDTH - (WIDTH/3 + WIDTH/18),
            .end_y = HEIGHT/3 + HEIGHT/18 + HEIGHT/15,

            .label = "CONTINUE",

            .default_color  = al_map_rgb(38, 17, 82),
            .selected_color = al_map_rgb(72, 118, 187)
        },

        {
            .origin_x = WIDTH/3 + WIDTH/18,
            .origin_y = HEIGHT/3 + HEIGHT/18 + HEIGHT/15 + HEIGHT/30,

            .end_x = WIDTH - (WIDTH/3 + WIDTH/18),
            .end_y = HEIGHT/3 + HEIGHT/18 + 2 * HEIGHT/15 + HEIGHT/30,

            .label = "EXIT",

            .default_color  = al_map_rgb(38, 17, 82),
            .selected_color = al_map_rgb(72, 118, 187)
        }
    };

    while (1) {
        // Recebe o evento e as entradas

        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        // Encerra o jogo

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return MODE_EXIT;
        }

        // Retorna ao jogo

        if (input->escape_pressed || input->x_pressed) {
            input_pressed_flush(input);
            return MODE_PLAYFIELD;
        }

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {

            // Fullscreen

            if (input->f4_pressed) {
                toggle_fullscreen(allegro);
            }
            // Move a seleção

            if (input->down_pressed) {
                selected = 1;
            }
            if (input->up_pressed) {
                selected = 0;
            }

            // Seleciona a opção escolhida

            if (input->space_pressed || input->z_pressed) {
                input_pressed_flush(input);

                switch (selected) {
                case 0:
                    return MODE_PLAYFIELD;
                case 1:
                    return MODE_MAIN_MENU;
                }
            }

            // Atualiza as entradas e indica que deve-se redesenhar a tela

            input_pressed_flush(input);
            allegro->redraw = 1;
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            allegro->redraw = 0;

            // Limpa o fundo

            al_clear_to_color(BACKGROUND_COLOR);

            // Exibe a borda

            al_draw_scaled_bitmap(
                allegro->bitmap_playfield,
                0,
                0,
                170,
                330,
                (PLAYFIELD_X - BLOCK_LENGTH * 5 / 16) * allegro->scale + allegro->x_offset,
                (PLAYFIELD_Y - BLOCK_LENGTH * 5 / 16) * allegro->scale + allegro->y_offset,
                (PLAYFIELD_WIDTH + BLOCK_LENGTH * 5 / 8) * allegro->scale,
                (PLAYFIELD_HEIGHT + BLOCK_LENGTH * 5 / 8) * allegro->scale,
                0
            );

            // Exibe os botões

            for (int i = 0; i < 2; i++) {
                button_draw(allegro, pause_button[i], selected == i);
            }

            // Tutorial

            draw_keybinds(allegro, HEIGHT / 32.0);

            // Atualiza a tela

            al_flip_display();
        }
    }
}

void draw_keybinds(AllegroContext *allegro, int sprite_scaling)
{

    int sprite_x = 0;        // Coordenada X no sprite, sempre 0
    int sprite_y = 0;        // Coordenada Y no sprite, 0, 1, 2, 3, 4, 5 de acordo com o botão

    char keybind_names[8][15] = {
        "RIGHT",
        "LEFT",
        "DOWN",
        "SPACE",
        "Z X",
        "ESCAPE",
        "C",
        "F4"
    };

    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            al_draw_scaled_bitmap(
                allegro->bitmap_keybinds,
                sprite_x,
                sprite_y * 48,
                48,
                48,
                ((j * 4.5f * sprite_scaling) + 72) * allegro->scale + allegro->x_offset,
                (HEIGHT/10 + (i * (sprite_scaling + HEIGHT/10)) + 192) * allegro->scale + allegro->y_offset,
                2 * sprite_scaling * allegro->scale,
                2 * sprite_scaling * allegro->scale,
                0
            );

            al_draw_textf(
                allegro->font_small,
                WHITE,
                (((j * 4.5f * sprite_scaling) + (sprite_scaling)) + 72) * allegro->scale + allegro->x_offset,
                (HEIGHT/10 + (i * (sprite_scaling + HEIGHT/10)) + (HEIGHT / 12) + 176) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "%s",
                keybind_names[sprite_y]
            );

            sprite_y++; // define o botão de acordo com o sprite

            if(sprite_y > 7) break;
        }
    }
}

GameMode loss_screen(AllegroContext *allegro, Input *input, int points)
{
    while(1) {
        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        // Encerra o jogo

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return MODE_EXIT;
        }

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            allegro->redraw = 1;

            // Fullscreen

            if (input->f4_pressed) {
                toggle_fullscreen(allegro);
            }

            // Espera entrada do jogador

            if (input->escape_pressed || input->z_pressed || input->space_pressed) {
                input_pressed_flush(input);
                return MODE_PLAYFIELD;
            }

            input_pressed_flush(input);
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            // Bordas

            al_draw_filled_rectangle(
                (WIDTH/3 + WIDTH/18) * allegro->scale + allegro->x_offset,
                (HEIGHT/3 + HEIGHT/15) * allegro->scale + allegro->y_offset,
                (WIDTH - (WIDTH/3 + WIDTH/18)) * allegro->scale + allegro->x_offset,
                (HEIGHT - (HEIGHT/3 + HEIGHT/15)) * allegro->scale + allegro->y_offset,
                al_map_rgb(238, 48, 70)
            );

            al_draw_filled_rectangle(
                (WIDTH/3 + WIDTH/15) * allegro->scale + allegro->x_offset,
                (HEIGHT/3 + HEIGHT/12) * allegro->scale + allegro->y_offset,
                (WIDTH - (WIDTH/3 + WIDTH/15)) * allegro->scale + allegro->x_offset,
                (HEIGHT - (HEIGHT/3 + HEIGHT/12)) * allegro->scale + allegro->y_offset,
                al_map_rgb(166, 38, 84)
            );

            // Título e pontuação

            al_draw_text(
                allegro->font,
                WHITE,
                (WIDTH/2) * allegro->scale + allegro->x_offset,
                (HEIGHT/3 + HEIGHT/10) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "GAME OVER"
            );

            al_draw_textf(
                allegro->font,
                WHITE,
                (WIDTH/2) * allegro->scale + allegro->x_offset,
                (HEIGHT/2) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "SCORE: %04d", points
            );

            al_flip_display();
        }


    }
}

GameMode get_new_highscore(GlobalState *global, AllegroContext *allegro, int points)
{
    t_leaderboard *leaderboard = global->leaderboard;
    int difficulty = global->selected_difficulty;

    int i = 0;

    t_leaderboard_entry new_entry = {0};

    new_entry.difficulty = difficulty;
    new_entry.score = points;

    while (1) {
        al_wait_for_event(allegro->queue, &allegro->event);

        switch (allegro->event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:   // termina o jogo caso a janela seja fechada
                return MODE_EXIT;

            case ALLEGRO_EVENT_KEY_CHAR:
                if(i <= 10){

                if(allegro->event.keyboard.unichar > 47 && allegro->event.keyboard.unichar < 123)
                    new_entry.name[i++] = allegro->event.keyboard.unichar;
                }

                if(allegro->event.keyboard.unichar == 127 || allegro->event.keyboard.unichar == 8 && i > 0)
                new_entry.name[--i] = 0;

                if(allegro->event.keyboard.unichar == 13 && i > 0){

                    save_highscore(leaderboard, new_entry);
                    return MODE_PLAYFIELD;
                }

                break;

            case ALLEGRO_EVENT_TIMER:
                allegro->redraw = 1;
                break;
        }

        if (allegro->redraw == 1 && al_is_event_queue_empty(allegro->queue)) {
            allegro->redraw = 0;

            al_draw_filled_rounded_rectangle(
                (WIDTH/3 + WIDTH/28) * allegro->scale + allegro->x_offset,
                (HEIGHT/3 + HEIGHT/15) * allegro->scale + allegro->y_offset,
                (WIDTH - (WIDTH/3 + WIDTH/28)) * allegro->scale + allegro->x_offset,
                (HEIGHT - (HEIGHT/3 + HEIGHT/15)) * allegro->scale + allegro->y_offset,
                0,
                0,
                al_map_rgb(81, 179, 65)
            );

            al_draw_filled_rounded_rectangle(
                (WIDTH/3 + WIDTH/23) * allegro->scale + allegro->x_offset,
                (HEIGHT/3 + HEIGHT/12) * allegro->scale + allegro->y_offset,
                (WIDTH - (WIDTH/3 + WIDTH/23)) * allegro->scale + allegro->x_offset,
                (HEIGHT - (HEIGHT/3 + HEIGHT/12)) * allegro->scale + allegro->y_offset,
                0,
                0,
                al_map_rgb(23, 81, 69)
            );

            al_draw_textf(
                allegro->font_small,
                WHITE,
                WIDTH/2 * allegro->scale + allegro->x_offset,
                (HEIGHT/3 + HEIGHT/10) * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "NEW HIGHSCORE! %d",
                points
            );

            al_draw_textf(
                allegro->font_small,
                WHITE,
                WIDTH/2 * allegro->scale + allegro->x_offset,
                HEIGHT/2 * allegro->scale + allegro->y_offset,
                ALLEGRO_ALIGN_CENTER,
                "NAME: %s",
                new_entry.name
            );

            al_flip_display();
        }
    }

    return MODE_PLAYFIELD;
}
