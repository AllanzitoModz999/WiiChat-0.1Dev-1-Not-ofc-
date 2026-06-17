#include <gccore.h>
#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <stdlib.h>

#include "ui.h"

int main(int argc, char **argv)
{
    VIDEO_Init();
    WPAD_Init();

    // Ativa o IR (ponteiro do Wii)
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    // Inicializa o GRRLIB
    GRRLIB_Init();

    // Inicializa a interface
    UI_Init();

    while (1)
    {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);

        // HOME = sair do aplicativo
        if (pressed & WPAD_BUTTON_HOME)
        {
            break;
        }

        // Desenha toda a interface
        UI_Draw();

        VIDEO_WaitVSync();
    }

    // Finaliza o GRRLIB
    GRRLIB_Exit();

    return 0;
}