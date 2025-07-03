#pragma once

// Inclus�o das bibliotecas necess�rias para cada funcionalidade utilizada
#include <allegro5/allegro.h>              // Biblioteca principal do Allegro
#include <allegro5/allegro_font.h>         // Suporte para fontes bitmap
#include <allegro5/allegro_ttf.h>          // Suporte para fontes TrueType
#include <allegro5/allegro_primitives.h>   // Desenho de primitivas (formas geom�tricas)
#include <allegro5/allegro_audio.h>        // Sistema de �udio
#include <allegro5/allegro_acodec.h>       // Suporte a codecs de �udio
#include <allegro5/allegro_image.h>        // Carregamento de imagens
#include <allegro5/mouse.h>                // Suporte ao mouse
#include <allegro5/keyboard.h>             // Suporte ao teclado

// Macros contendo as dimens�es da janela

#define WIDTH  1080
#define HEIGHT 720

// struct contendo as estruturas empregadas pela Allegro

typedef struct AllegroContext {
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT event;

    ALLEGRO_FONT *font;
    ALLEGRO_FONT *font_small;

    ALLEGRO_BITMAP *bitmap_blocks;
    ALLEGRO_BITMAP *bitmap_playfield;
    ALLEGRO_BITMAP *bitmap_keybinds;

    int redraw;
} AllegroContext;

AllegroContext *allegro_init();
void allegro_free(AllegroContext *allegro);
