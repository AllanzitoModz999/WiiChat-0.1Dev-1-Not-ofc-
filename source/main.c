#include <gccore.h>
#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ui.h"
#include "pointer.h"

// Estado principal do aplicativo
static bool running = true;

int main(int argc, char **argv)
{
    // Inicializa vídeo
    VIDEO_Init();

    // Inicializa Wii Remote
    WPAD_Init();

    // Ativa IR + Botões + Acelerômetro
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    // Inicializa GRRLIB
    GRRLIB_Init();

    // Inicializa módulos
    UI_Init();
    Pointer_Init();

    // Loop principal
    while (running)
    {
        // Atualiza controles
        WPAD_ScanPads();

        // Atualiza ponteiro IR
        Pointer_Update();

        // Botões pressionados
        u32 pressed = WPAD_ButtonsDown(0);

        // HOME = sair (depois vamos trocar pelo Menu HOME)
        if (pressed & WPAD_BUTTON_HOME)
        {
            running = false;
        }

        // Limpa a tela (branco)
        GRRLIB_FillScreen(0xFFFFFFFF);

        // Desenha interface
        UI_Draw();

        // Desenha ponteiro
        Pointer_Draw();

        // Atualiza tela
        GRRLIB_Render();

        VIDEO_WaitVSync();
    }

    // Finaliza módulos
    Pointer_Exit();
    GRRLIB_Exit();

    WPAD_Shutdown();

    // Retorna ao Menu do Wii
    SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);

    return 0;
}