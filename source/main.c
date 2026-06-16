#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <network.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.0.100"
#define SERVER_PORT 3000

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

/* ───────── CHAT ───────── */
#define MAX_CHAT 6
#define MAX_LEN 64

char chat[MAX_CHAT][MAX_LEN];
char input[MAX_LEN];
int input_len = 0;

/* ───────── IR ───────── */
ir_t ir;
float px = 0, py = 0;

/* ───────── KEYBOARD ───────── */
char charset[] =
"ABCDEFGHIJKLMN"
"OPQRSTUVWXYZ__";

int cols = 14;

/* ───────── HOME MENU ───────── */
int show_menu = 0;
int menu_index = 0;

/* ───────── INIT ───────── */
void init()
{
    VIDEO_Init();
    WPAD_Init();
    net_init();

    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

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

/* ───────── CHAT ───────── */
void add_chat(const char *msg)
{
    for (int i = 1; i < MAX_CHAT; i++)
        strcpy(chat[i - 1], chat[i]);

    strncpy(chat[MAX_CHAT - 1], msg, MAX_LEN);
}

/* ───────── SEND ───────── */
void send_message(const char *msg)
{
    int sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) return;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (net_connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        net_close(sock);
        return;
    }

    char body[128];
    snprintf(body, sizeof(body),
        "SEND|Wii|%s", msg);

    net_send(sock, body, strlen(body), 0);
    net_close(sock);
}

/* ───────── FETCH ───────── */
void fetch_messages()
{
    int sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) return;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (net_connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        net_close(sock);
        return;
    }

    char req[] = "GET /messages HTTP/1.1\r\n\r\n";
    net_send(sock, req, strlen(req), 0);

    char buffer[512];
    int len = net_recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (len > 0)
    {
        buffer[len] = 0;
        add_chat("Server OK");
    }

    net_close(sock);
}

/* ───────── DRAW ───────── */
void draw_keyboard(int sel)
{
    printf("\nKEYBOARD:\n");

    for (int i = 0; i < (int)strlen(charset); i++)
    {
        if (i == sel)
            printf("[%c] ", charset[i]);
        else
            printf(" %c  ", charset[i]);

        if ((i + 1) % cols == 0)
            printf("\n");
    }
}

/* ───────── MAIN ───────── */
int main(int argc, char **argv)
{
    init();

    add_chat("WiiChat ONLINE");

    int frame = 0;

    while (1)
    {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);

        /* HOME MENU */
        if (pressed & WPAD_BUTTON_HOME)
            show_menu = !show_menu;

        if (show_menu)
        {
            printf("\x1b[2J");
            printf("HOME MENU\n\n");

            if (pressed & WPAD_BUTTON_A)
                menu_index = (menu_index + 1) % 2;

            if (menu_index == 0 && (pressed & WPAD_BUTTON_1))
                show_menu = 0;

            if (menu_index == 1 && (pressed & WPAD_BUTTON_1))
                exit(0);

            printf(menu_index == 0 ? "> Continue\n" : "Continue\n");
            printf(menu_index == 1 ? "> Exit\n" : "Exit\n");

            VIDEO_WaitVSync();
            continue;
        }

        /* IR */
        WPAD_IR(WPAD_CHAN_0, &ir);
        int sel = 0;

        if (ir.valid)
        {
            px = ir.x;
            py = ir.y;

            int col = px / 50;
            int row = py / 50;

            sel = row * cols + col;
        }

        /* INPUT */
        if (pressed & WPAD_BUTTON_A)
        {
            if (sel >= 0 && sel < (int)strlen(charset))
            {
                input[input_len++] = charset[sel];
                input[input_len] = 0;
            }
        }

        if (pressed & WPAD_BUTTON_B)
        {
            if (input_len > 0)
            {
                input_len--;
                input[input_len] = 0;
            }
        }

        if (pressed & WPAD_BUTTON_1)
        {
            send_message(input);
            add_chat(input);
            input[0] = 0;
            input_len = 0;
        }

        /* FETCH */
        if (frame % 120 == 0)
            fetch_messages();

        /* DRAW */
        printf("\x1b[2J");
        printf("WiiChat IR ONLINE\n\n");

        printf("CHAT:\n");
        for (int i = 0; i < MAX_CHAT; i++)
            printf(" > %s\n", chat[i]);

        printf("\nINPUT: %s\n", input);

        printf("\nIR POS: %.0f %.0f\n", px, py);

        draw_keyboard(sel);

        printf("\nHOME = menu | A = select | B = delete | 1 = send\n");

        frame++;
        VIDEO_WaitVSync();
    }

    return 0;
}