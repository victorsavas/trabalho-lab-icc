#include <allegro5/allegro.h>              // Biblioteca principal do Allegro
#include <allegro5/allegro_font.h>         // Suporte para fontes bitmap
#include <allegro5/allegro_ttf.h>          // Suporte para fontes TrueType
#include <allegro5/allegro_primitives.h>   // Desenho de primitivas (formas geométricas)
#include <allegro5/allegro_audio.h>        // Sistema de áudio
#include <allegro5/allegro_acodec.h>       // Suporte a codecs de áudio
#include <allegro5/allegro_image.h>        // Carregamento de imagens
#include <allegro5/mouse.h>                // Suporte ao mouse
#include <allegro5/keyboard.h>             // Suporte ao teclado

// Constantes de configuração da janela e do sprite
#define WIDTH 1920                         // Largura da janela
#define HEIGHT 1024                        // Altura da janela
#define SPRITE_SIZE 16                     // Tamanho (largura e altura) de cada frame do sprite
#define SPRITE_COLS 1                      // Número de colunas na spritesheet
#define SPRITE_ROWS 4                      // Número de linhas na spritesheet (diferentes movimentos)

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

    // Cria uma janela com as dimensões especificadas
    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);

    // Cria uma fila de eventos para tratar interações do usuário e um temporizador para controle de FPS
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60); // 60 quadros por segundo

    // Registra as fontes de eventos que a fila vai monitorar
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Carrega uma imagem contendo múltimos sprites (spritesheet)
    ALLEGRO_BITMAP *sprite_sheet = al_load_bitmap("../../sprites/tetris_sprite_16x16.png");
    al_convert_mask_to_alpha(sprite_sheet, al_map_rgb(0, 0, 0)); // Remove fundo magenta

    // Variáveis de controle da aplicação
    bool falling = true;      // Indica se a aplicação deve continuar executando
    bool redraw = true;       // Indica se a tela precisa ser redesenhada
    bool hover = false;       // Indica se o mouse está sobre o botão
    bool playing = false;     // Indica se o som está tocando
    int sprite_coordx= 0;
    int sprite_coordy= 100;
    int mouse_x = 0, mouse_y = 0; // Posição atual do mouse
    ALLEGRO_EVENT ev;         // Estrutura para eventos

    // Controle da animação
    int frame = 0;            // Frame atual da animação (coluna)
    int movement = 0;         // Linha do spritesheet representando o movimento atual
    double sprite_timer = 0.0;// Timer para troca de frames
    double sprite_delay = 0.15; // Tempo entre cada frame (150 ms)

    // Inicia o temporizador
    al_start_timer(timer);

    // Loop principal do jogo
    while (falling) {
        // Aguarda próximo evento na fila
        al_wait_for_event(queue, &ev);

        // Tratamento dos eventos recebidos
        switch (ev.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                falling = false;
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
            case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
                // Atualiza posição do mouse e detecta se está sobre o botão
                mouse_x = ev.mouse.x;
                mouse_y = ev.mouse.y;
                hover = (mouse_x >= 220 && mouse_x <= 420 && mouse_y >= 190 && mouse_y <= 250);
                break;


/*            case ALLEGRO_EVENT_KEY_DOWN:
                // Fecha o programa com ESC ou troca a linha da animação com SETAS
                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    falling = false;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_UP)
                    movement = (movement - 1 + SPRITE_ROWS) % SPRITE_ROWS;
                else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
                    movement = (movement + 1) % SPRITE_ROWS;
                break;
*/

            case ALLEGRO_EVENT_KEY_DOWN:

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    falling = false;
                else if (ev.keyboard.keycode ==  ALLEGRO_KEY_LEFT)
                    sprite_coordx = sprite_coordx - 10;
               else if (ev.keyboard.keycode ==  ALLEGRO_KEY_RIGHT)
                    sprite_coordx = sprite_coordx + 10;

                break;

            case ALLEGRO_EVENT_TIMER:
                // Atualiza a animação quando o tempo entre frames é atingido
                sprite_timer += 1.0 / 60.0;
                if (sprite_timer >= sprite_delay) {
                    frame = (frame + 1) % SPRITE_COLS; // Avança para o próximo frame
                    sprite_timer = 0.0;
                }
                redraw = true; // Marca que a tela precisa ser redesenhada
                break;
        }

        // Redesenho da tela
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;

            // Limpa a tela com uma cor escura
            al_clear_to_color(al_map_rgb(30, 30, 30));

            // Desenha a animação do sprite na parte inferior da tela

            int sprite_x = frame * SPRITE_SIZE;         // Coordenada X da imagem a ser exibida
            int sprite_y = movement * SPRITE_SIZE;      // Coordenada Y da imagem a ser exibida
            al_draw_bitmap_region(sprite_sheet, sprite_x, sprite_y, SPRITE_SIZE, SPRITE_SIZE,
                                  (WIDTH - SPRITE_SIZE)/2 + sprite_coordx, HEIGHT/2 + SPRITE_SIZE - (sprite_coordy = sprite_coordy - 1), 0);


            al_flip_display(); // Atualiza a tela com o que foi desenhado
        }
    }

    // Libera recursos alocados
    al_destroy_bitmap(sprite_sheet);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}
