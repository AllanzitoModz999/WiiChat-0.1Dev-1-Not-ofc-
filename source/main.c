#include <gccore.h>
#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ui.h"
#include "pointer.h"

// Estado do programa
bool running = true;

int main(int argc, char **argv)
{
    // Inicializa vídeo
    VIDEO_Init();

    // Inicializa Wii Remote
    WPAD_Init();

    // Ativa botões + acelerômetro + IR
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    // Inicializa GRRLIB
    GRRLIB_Init();

    // Inicializa interface
    UI_Init();

    while (running)
    {
        // Atualiza controles
        WPAD_ScanPads();

        // Atualiza posição do ponteiro
        Pointer_Update();

        // Botões pressionados
        u32 pressed = WPAD_ButtonsDown(0);

        //-------------------------------------------------
        // HOME = sair do WiiChat
        //-------------------------------------------------
        if (pressed & WPAD_BUTTON_HOME)
        {
            running = false;
        }

        //-------------------------------------------------
        // Limpa a tela
        //-------------------------------------------------
        GRRLIB_FillScreen(0xFFFFFFFF);

        //-------------------------------------------------
        // Desenha interface
        //-------------------------------------------------
        UI_Draw();

        //-------------------------------------------------
        // Desenha cursor
        //-------------------------------------------------
        Pointer_Draw();

        //-------------------------------------------------
        // Renderiza
        //-------------------------------------------------
        GRRLIB_Render();

        VIDEO_WaitVSync();
    }

    //-----------------------------------------------------
    // Finalização
    //-----------------------------------------------------

    GRRLIB_Exit();

    WPAD_Shutdown();

    SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);

    return 0;
}