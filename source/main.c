#include <stdio.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

static void init_video()
{
    VIDEO_Init();
    WPAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    console_init(xfb, 0, 0,
                 rmode->fbWidth,
                 rmode->xfbHeight,
                 rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
}

static void draw_ui(int frame)
{
    // “limpa tela”
    printf("\x1b[2J");
    printf("\x1b[0;0H");

    // topo estilo canal
    printf("========================================\n");
    printf("            WiiChat v0.1-dev1           \n");
    printf("        Clean UI Prototype Build        \n");
    printf("========================================\n\n");

    // área principal
    printf("  Status  : ONLINE\n");
    printf("  Server  : Local Test\n");
    printf("  Channel : #general\n\n");

    printf("  ------------------------------------\n");
    printf("  Welcome to WiiChat!\n");
    printf("  This is a UI prototype.\n");
    printf("  ------------------------------------\n\n");

    // “cards” simulados
    printf("  [ Chat Preview ]\n");
    printf("  > Allan: Hello World\n");
    printf("  > System: Connected\n\n");

    printf("  ------------------------------------\n");
    printf("  Controls:\n");
    printf("    HOME - Exit\n\n");

    printf("  Frame: %d\n", frame);
}

int main(int argc, char **argv)
{
    init_video();

    int frame = 0;

    while (1)
    {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);

        draw_ui(frame++);

        if (pressed & WPAD_BUTTON_HOME)
            break;

        VIDEO_WaitVSync();
    }

    return 0;
}