#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

/* ── Video globals ──────────────────────────────────────────────── */
static void   *xfb     = NULL;
static GXRModeObj *rmode = NULL;

/* ── Helpers ────────────────────────────────────────────────────── */
static void video_init(void)
{
    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb   = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight,
                 rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
}

/* ── Entry point ────────────────────────────────────────────────── */
int main(int argc, char **argv)
{
    video_init();
    WPAD_Init();

    /* ── Draw UI ───────────────────────────────────────────────── */
    printf("\x1b[2;0H");   /* row 2, col 0 */

    printf("  ┌─────────────────────────────────┐\n");
    printf("  │         WiiChat v0.1-dev1        │\n");
    printf("  │   Wii Homebrew Chat Terminal     │\n");
    printf("  └─────────────────────────────────┘\n");
    printf("\n");
    printf("  Status  : Idle\n");
    printf("  Channel : #general\n");
    printf("\n");
    printf("  ─────────────────────────────────────\n");
    printf("  Controls:\n");
    printf("    HOME  – Exit to loader\n");
    printf("  ─────────────────────────────────────\n");

    /* ── Main loop ─────────────────────────────────────────────── */
    while (1)
    {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);   /* Wiimote #0 */

        if (pressed & WPAD_BUTTON_HOME)
            exit(0);

        VIDEO_WaitVSync();
    }

    return 0;   /* unreachable */
}