#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
GXRModeObj *rmode = NULL;

int main(int argc, char **argv)
{
    VIDEO_Init();
    WPAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);

    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    console_init(
        xfb,
        20, 20,
        rmode->fbWidth,
        rmode->xfbHeight,
        rmode->fbWidth * VI_DISPLAY_PIX_SZ
    );

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    while (1)
    {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);

        printf("\x1b[2J");
        printf("\n");
        printf("=====================================\n");
        printf("            WiiChat\n");
        printf("=====================================\n\n");
        printf("Status: Online\n");
        printf("Servidor: Offline\n\n");
        printf("Pressione HOME para sair.\n");

        if (pressed & WPAD_BUTTON_HOME)
            break;

        VIDEO_WaitVSync();
    }

    return 0;
}