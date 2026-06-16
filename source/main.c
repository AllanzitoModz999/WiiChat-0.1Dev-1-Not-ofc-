#include <stdio.h>
#include <string.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

#define MAX_MSGS 6
#define MAX_LEN  32

char messages[MAX_MSGS][MAX_LEN];
int msg_count = 0;

char input[MAX_LEN];
int input_len = 0;

char charset[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int cursor = 0;

int frame = 0;
int blink = 1;

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

void add_message(const char *msg)
{
    for (int i = 1; i < MAX_MSGS; i++)
        strcpy(messages[i - 1], messages[i]);

    strncpy(messages[MAX_MSGS - 1], msg, MAX_LEN);
    messages[MAX_MSGS - 1][MAX_LEN - 1] = '\0';

    if (msg_count < MAX_MSGS)
        msg_count++;
}

void draw_ui()
{
    printf("\x1b[2J");
    printf("\x1b[0;0H");

    printf("========================================\n");
    printf("           WiiChat v0.1-dev2           \n");
    printf("         CURSOR INPUT MODE             \n");
    printf("========================================\n\n");

    printf("CHAT:\n");
    for (int i = 0; i < msg_count; i++)
        printf(" > %s\n", messages[i]);

    printf("\n----------------------------------------\n");

    printf("INPUT: %s", input);

    if (blink)
        printf("_");

    printf("\n\nCHAR SELECTOR:\n");

    for (int i = 0; i < (int)strlen(charset); i++)
    {
        if (i == cursor)
            printf("[%c] ", charset[i]);
        else
            printf(" %c  ", charset[i]);
    }

    printf("\n\nControls:\n");
    printf(" A = select | B = delete | 1 = send | HOME = exit\n");
}

int main(int argc, char **argv)
{
    init_video();

    add_message("System: WiiChat online");
    add_message("System: Cursor input ready");

    while (1)
    {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);

        // blink cursor
        if (frame % 30 == 0)
            blink = !blink;

        // move cursor (D-Pad)
        if (pressed & WPAD_BUTTON_RIGHT)
        {
            cursor++;
            if (cursor >= (int)strlen(charset))
                cursor = 0;
        }

        if (pressed & WPAD_BUTTON_LEFT)
        {
            cursor--;
            if (cursor < 0)
                cursor = strlen(charset) - 1;
        }

        // select letter
        if (pressed & WPAD_BUTTON_A)
        {
            if (input_len < MAX_LEN - 1)
            {
                input[input_len++] = charset[cursor];
                input[input_len] = '\0';
            }
        }

        // delete
        if (pressed & WPAD_BUTTON_B)
        {
            if (input_len > 0)
            {
                input_len--;
                input[input_len] = '\0';
            }
        }

        // send message
        if (pressed & WPAD_BUTTON_1)
        {
            if (input_len > 0)
            {
                add_message(input);
                input[0] = '\0';
                input_len = 0;
            }
        }

        draw_ui();

        frame++;
        VIDEO_WaitVSync();
    }

    return 0;
}