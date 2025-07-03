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

typedef enum TetrominoType {
    TETROMINO_I=0,
    TETROMINO_J,
    TETROMINO_L,
    TETROMINO_O,
    TETROMINO_S,
    TETROMINO_T,
    TETROMINO_Z
} TetrominoType;

double fall_time_difficulty_table[3] = {1.0, 0.2, 0.08};

// Tabelas de referência para o algorítmo SRS de rotação.
// Rotações positivas são em sentido horário
// Os índices correspondem à variável state do tetrominó

static const int wall_kick_tests_3x3[4][4][2] = {
    // 0 >> 1 (+) ou 1 >> 0 (-)
    {
        {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}
    },

    // 1 >> 2 (+) ou 2 >> 1 (-)
    {
        {1, 0}, {1, -1}, {0, 2}, {1, 2}
    },

    // 2 >> 3 (+) ou 3 >> 2 (-)
    {
        {1, 0}, {1, 1}, {0, -2}, {1, -2}
    },

    // 3 >> 0 (+) ou 0 >> 3 (-)
    {
        {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}
    }
};

static const int wall_kick_tests_i[4][4][2] = {
    // 0 >> 1 (+) ou 1 >> 0 (-)
    {
        {-2, 0}, {1, 0}, {-2, -1}, {1, 2}
    },

    // 1 >> 2 (+) ou 2 >> 1 (-)
    {
        {-1, 0}, {2, 0}, {-1, 2}, {2, -1}
    },

    // 2 >> 3 (+) ou 3 >> 2 (-)
    {
        {2, 0}, {-1, 0}, {2, 1}, {-1, -2}
    },

    // 3 >> 0 (+) ou 0 >> 3 (-)
    {
        {1, 0}, {-2, 0}, {1, -2}, {-2, 1}
    }
};

typedef struct Tetromino {
    int x;
    int y;

    TetrominoType type;
    int state;

    int shape[16];
} Tetromino;

typedef struct Playfield {
    int board[200];

    unsigned int lines_cleared;
    unsigned int points;

    double fall_timer;
    double next_timer;

    double horizontal_move_timer;
    double fast_fall_timer;
} Playfield;

static Tetromino tetromino_rotate_counterclockwise(Tetromino *tetromino);
static Tetromino tetromino_rotate_clockwise(Tetromino *tetromino);

static int tetromino_collision_check(Playfield *playfield, Tetromino tetromino, int x_offset, int y_offset);
static int tetromino_next(Playfield *playfield, Tetromino *tetromino, Tetromino *updated_tetromino);

static void tetromino_wall_kick(Playfield *playfield, Tetromino *tetromino, Tetromino updated_tetromino, int rotation);

static void draw_playfield(AllegroContext *allegro, Playfield playfield, Tetromino tetromino);

static GameMode pause_menu(AllegroContext *allegro, Input *input);
static void draw_keybinds(AllegroContext *allegro, int sprite_scaling);

static void loss_screen(AllegroContext *allegro, Input *input, int points);

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

Tetromino tetromino_rotate_counterclockwise(Tetromino *tetromino)
{
    tetromino->state = (tetromino->state + 3) % 4;

    switch (tetromino->type) {
    case TETROMINO_O:
    {
        return;
    }
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
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

Tetromino tetromino_rotate_clockwise(Tetromino *tetromino)
{
    tetromino->state = (tetromino->state + 1) % 4;

    switch (tetromino->type) {
    case TETROMINO_O:
    {
        return;
    }
    case TETROMINO_J:
    case TETROMINO_L:
    case TETROMINO_S:
    case TETROMINO_Z:
    case TETROMINO_T:
    {
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
            if (tetromino.shape[i + j * 4] == 0) {
                continue;
            }

            int x = tetromino.x + i + x_offset;
            int y = tetromino.y + j + y_offset;

            if (x < 0 || x >= 10) {
                return 1;
            }

            if (y < 0) {
                continue;
            }

            if (y >= 20) {
                return 1;
            }

            if (playfield->board[x + y * 10] != 0) {
                return 1;
            }
        }
    }

    return 0;
}

int tetromino_next(Playfield *playfield, Tetromino *tetromino, Tetromino *updated_tetromino)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int color = tetromino->shape[i + j * 4];

            if (color == 0) {
                continue;
            }

            int x = tetromino->x + i;
            int y = tetromino->y + j;

            if (x < 0 || x >= 10 || y < 0 || y >= 20) {
                return 1;
            }

            playfield->board[x + y * 10] = color;
        }
    }

    *tetromino = tetromino_table[rand() % 7];

    // Verificando se há linhas preenchidas

    int lines_cleared = 0;
    int y_0;

    for (int y = 19; y >= 0; y--) {
        int x;
        for (x = 0; x < 10; x++) {
            if (playfield->board[x + y * 10] == 0) {
                break;
            }
        }

        // Linha preenchida
        if (x == 10) {
            lines_cleared++;
        } else if (lines_cleared > 0) {
            y_0 = y;
            break;
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

    playfield->lines_cleared += lines_cleared;
    playfield->points += lines_cleared * 100;

    *updated_tetromino = *tetromino;

    if (tetromino_collision_check(playfield, *updated_tetromino, 0, 0)) {
        return 1;
    }

    return 0;
}

void tetromino_wall_kick(Playfield *playfield, Tetromino *tetromino, Tetromino updated_tetromino, int rotation)
{
    if (tetromino->type == TETROMINO_O) {
        return;
    }

    if (!tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
        *tetromino = updated_tetromino;
        return;
    }

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
        int x_offset = tests[i][0] * rotation;
        int y_offset = tests[i][1] * rotation;

        if (!tetromino_collision_check(playfield, updated_tetromino, x_offset, y_offset)) {
            *tetromino = updated_tetromino;

            tetromino->x += x_offset;
            tetromino->y += y_offset;

            return;
        }
    }
}

void draw_playfield(AllegroContext *allegro, Playfield playfield, Tetromino tetromino)
{
    al_draw_scaled_bitmap(
        allegro->bitmap_playfield,
        0,
        0,
        170,
        330,
        PLAYFIELD_X - BLOCK_LENGTH * 5 / 16,
        PLAYFIELD_Y - BLOCK_LENGTH * 5 / 16,
        PLAYFIELD_WIDTH + BLOCK_LENGTH * 5 / 8,
        PLAYFIELD_HEIGHT + BLOCK_LENGTH * 5 / 8,
        0
    );

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            int color = playfield.board[i + 10 * j];

            if (color != 0) {
                al_draw_scaled_bitmap(
                    allegro->bitmap_blocks,
                    0,
                    16 * (color - 1),
                    16,
                    16,
                    PLAYFIELD_X + i * BLOCK_LENGTH,
                    PLAYFIELD_Y + j * BLOCK_LENGTH,
                    BLOCK_LENGTH,
                    BLOCK_LENGTH,
                    0
                );
            }
        }
    }

    Tetromino ghost_tetromino = tetromino;

    for (int i = 0; i < 20; i++) {
        if (tetromino_collision_check(&playfield, ghost_tetromino, 0, 1)) {
            break;
        }
        ghost_tetromino.y++;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int color = ghost_tetromino.shape[i + 4 * j];

            if (color != 0) {
                al_draw_filled_rectangle(
                    PLAYFIELD_X + (i + ghost_tetromino.x) * BLOCK_LENGTH,
                    PLAYFIELD_Y + (j + ghost_tetromino.y) * BLOCK_LENGTH,
                    PLAYFIELD_X + (i + ghost_tetromino.x + 1) * BLOCK_LENGTH,
                    PLAYFIELD_Y + (j + ghost_tetromino.y + 1) * BLOCK_LENGTH,
                    al_map_rgb(39, 37, 115)
                );
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int color = tetromino.shape[i + 4 * j];

            if (color != 0) {
                al_draw_scaled_bitmap(
                    allegro->bitmap_blocks,
                    0,
                    16 * (color - 1),
                    16,
                    16,
                    PLAYFIELD_X + (i + tetromino.x) * BLOCK_LENGTH,
                    PLAYFIELD_Y + (j + tetromino.y) * BLOCK_LENGTH,
                    BLOCK_LENGTH,
                    BLOCK_LENGTH,
                    0
                );
            }
        }
    }

    al_draw_textf(
        allegro->font,
        al_map_rgb(255,255,255),
        WIDTH - WIDTH/5,
        HEIGHT - HEIGHT/8,
        ALLEGRO_ALIGN_CENTER,"%04d points",
        playfield.points
    );
}

int update_playfield(Playfield *playfield, Tetromino *tetromino, Input *input, double delta_time)
{
    if (input->escape_pressed) {
        return MODE_EXIT;
    }

    int x_move = 0;
    int rotation = 0;

    Tetromino updated_tetromino = *tetromino;

    // Queda rápida

    if (input->space_pressed) {
        for (int i = 0; i < 20; i++) {
            if (tetromino_collision_check(playfield, updated_tetromino, 0, 1)) {
                *tetromino = updated_tetromino;
                break;
            }
            updated_tetromino.y++;
        }

        tetromino_next(playfield, tetromino, &updated_tetromino);
        updated_tetromino = *tetromino;
    }

    // Cheque do movimento vertical

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

    if (playfield->fall_timer <= 0.0) {
        updated_tetromino.y++;
        playfield->fall_timer = fall_time_difficulty_table[difficulty];

        if (tetromino_collision_check(playfield, updated_tetromino, 0, 0)) {
            updated_tetromino.y--;
        } else {
            playfield->next_timer = NEXT_TIME;
        }
    }

    if (tetromino_collision_check(playfield, updated_tetromino, 0, 1)) {
        playfield->next_timer -= delta_time;

        if (playfield->next_timer <= 0) {
            playfield->next_timer = NEXT_TIME;
            int loss = tetromino_next(playfield, tetromino, &updated_tetromino);

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

    if (rotation) {
        tetromino_wall_kick(playfield, tetromino, updated_tetromino, rotation);
    }

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

GameMode game_playfield(AllegroContext *allegro, Input *input)
{
    GameMode mode = MODE_PLAYFIELD;

    int show_tutorial = 1;

    double current_time;
    double last_time = al_get_time();
    double delta_time;

    Playfield playfield = {
        .board = {0},
        .lines_cleared = 0,
        .points = 0,

        .fall_timer = fall_time_difficulty_table[difficulty],
        .next_timer = 1.0,

        .horizontal_move_timer = 0.0,
        .fast_fall_timer = 0.0
    };

    Tetromino tetromino = tetromino_table[rand() % 7];

    while (mode == MODE_PLAYFIELD) {
        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        if (allegro->event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || input->escape_pressed) {
            input_pressed_flush(input);

            mode = pause_menu(allegro, input);

            continue;
        }

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            current_time = al_get_time();
            delta_time = current_time - last_time;

            int loss = update_playfield(&playfield, &tetromino, input, delta_time);
            input_pressed_flush(input);

            if (playfield.lines_cleared != 0) {
                show_tutorial = 0;
            }

            if (loss) {
                loss_screen(allegro, input, playfield.points);

                if (highscore_comparison(playfield.points)){
                    get_new_highscore(allegro, playfield.points);
                    debug_leaderboard();
                }

                playfield = (Playfield){
                    .board = {0},
                    .lines_cleared = 0,
                    .points = 0,

                    .fall_timer = fall_time_difficulty_table[difficulty],
                    .next_timer = 1.0,

                    .horizontal_move_timer = 0.0,
                    .fast_fall_timer = 0.0
                };
            }

            allegro->redraw = 1;

            last_time = current_time;
        }

        if (allegro->redraw && al_is_event_queue_empty(allegro->queue)) {
            al_clear_to_color(BACKGROUND_COLOR);
            draw_playfield(allegro, playfield, tetromino);

            if (show_tutorial) {
                draw_keybinds(allegro, HEIGHT / 32.0);
            }

            al_flip_display();
            allegro->redraw = 0;
        }
    }

    return mode;
}

GameMode pause_menu(AllegroContext *allegro, Input *input)
{
    int selected = 0;

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
        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        if (input->escape_pressed || input->x_pressed) {
            input_pressed_flush(input);
            return MODE_PLAYFIELD;
        }

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            if (input->down_pressed) {
                selected = 1;
            }
            if (input->up_pressed) {
                selected = 0;
            }

            if (input->space_pressed || input->z_pressed) {
                input_pressed_flush(input);
                switch (selected) {
                case 0:
                    return MODE_PLAYFIELD;
                case 1:
                    return MODE_MAIN_MENU;
                }
            }

            input_pressed_flush(input);
            allegro->redraw = 1;
        }

        if (allegro->redraw) {
            allegro->redraw = 0;

            al_clear_to_color(BACKGROUND_COLOR);

            al_draw_scaled_bitmap(
                allegro->bitmap_playfield,
                0,
                0,
                170,
                330,
                PLAYFIELD_X - BLOCK_LENGTH * 5 / 16,
                PLAYFIELD_Y - BLOCK_LENGTH * 5 / 16,
                PLAYFIELD_WIDTH + BLOCK_LENGTH * 5 / 8,
                PLAYFIELD_HEIGHT + BLOCK_LENGTH * 5 / 8,
                0
            );

            for (int i = 0; i < 2; i++) {
                button_draw(allegro, pause_button[i], selected == i);
            }

            al_flip_display();
        }
    }
}

void draw_keybinds(AllegroContext *allegro, int sprite_scaling)
{

    int sprite_x = 0;        // Coordenada X no sprite, sempre 0
    int sprite_y = 0;        // Coordenada Y no sprite, 0, 1, 2, 3, 4, 5 de acordo com o botão

    char keybind_names[6][15] = {
        "RIGHT",
        "LEFT",
        "DOWN",
        "SPACE",
        "Z X",
        "ESCAPE"
    };

    for(int i=0; i<2; i++){
        for(int j=3; j>0; j--){
            al_draw_scaled_bitmap(
                allegro->bitmap_keybinds,
                sprite_x,
                sprite_y * 48,
                48,
                48,
                WIDTH - (j * 4.5f * sprite_scaling),
                HEIGHT/10 + (i * (sprite_scaling + HEIGHT/10)),
                2 * sprite_scaling,
                2 * sprite_scaling,
                0
            );
                                            // desenha o botão baseado na escala definida

            al_draw_textf(
                allegro->font_small,
                WHITE,
                WIDTH - ((j * 4.5f * sprite_scaling) - (sprite_scaling)),
                HEIGHT/10 + (i * (sprite_scaling + HEIGHT/10)) + (HEIGHT / 12),
                ALLEGRO_ALIGN_CENTER,
                "%s",
                keybind_names[sprite_y]
            );

            sprite_y++; // define o botão de acordo com o sprite
        }
    }
}

void loss_screen(AllegroContext *allegro, Input *input, int points)
{
    al_draw_filled_rounded_rectangle(
        WIDTH/3 + WIDTH/18,
        HEIGHT/3 + HEIGHT/15,
        WIDTH - (WIDTH/3 + WIDTH/18),
        HEIGHT - (HEIGHT/3 + HEIGHT/15),
        0,
        0,
        al_map_rgb(238, 48, 70)
    );

    al_draw_filled_rounded_rectangle(
        WIDTH/3 + WIDTH/15,
        HEIGHT/3 + HEIGHT/12,
        WIDTH - (WIDTH/3 + WIDTH/15),
        HEIGHT - (HEIGHT/3 + HEIGHT/12),
        0,
        0,
        al_map_rgb(166, 38, 84)
    );

    al_draw_text(
        allegro->font,
        WHITE,
        WIDTH/2,
        HEIGHT/3 + HEIGHT/10,
        ALLEGRO_ALIGN_CENTER,
        "GAME OVER"
    );

    al_draw_textf(
        allegro->font,
        WHITE,
        WIDTH/2,
        HEIGHT/2,
        ALLEGRO_ALIGN_CENTER,
        "SCORE: %04d",
        points
    );

    al_flip_display();

    while(1) {
        al_wait_for_event(allegro->queue, &allegro->event);
        input_update(allegro, input);

        if (allegro->event.type == ALLEGRO_EVENT_TIMER) {
            allegro->redraw = 1;

            if (input->escape_pressed || input->z_pressed || input->space_pressed) {
                input_pressed_flush(input);
                return;
            }
        }
    }
}
