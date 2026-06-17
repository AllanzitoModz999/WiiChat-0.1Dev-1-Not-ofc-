#include <grrlib.h>
#include "ui.h"

void UI_Init(void)
{
    // Futuramente carregar fontes e imagens
}

void UI_Draw(void)
{
    // Fundo branco
    GRRLIB_FillScreen(0xFFFFFFFF);

    // Barra superior azul
    GRRLIB_Rectangle(
        0,
        0,
        640,
        48,
        0x3B8DFFFF,
        true
    );

    // Painel do chat
    GRRLIB_Rectangle(
        20,
        60,
        600,
        300,
        0xF5F5F5FF,
        true
    );

    // Caixa de texto
    GRRLIB_Rectangle(
        20,
        380,
        460,
        60,
        0xFFFFFFFF,
        true
    );

    // Botão Enviar
    GRRLIB_Rectangle(
        500,
        380,
        120,
        60,
        0x4CAF50FF,
        true
    );

    GRRLIB_Render();
}