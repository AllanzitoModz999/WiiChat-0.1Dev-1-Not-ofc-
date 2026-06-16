#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include <network.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.18.119"
#define SERVER_PORT 3000

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

#define MAX_CHAT 6
#define MAX_LEN 64

char chat[MAX_CHAT][MAX_LEN];
int chat_count = 0;

char input[MAX_LEN];
int input_len = 0;

char charset[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int cursor = 0;

int frame = 0;
int blink = 1;

void init_video()
{
    VIDEO_Init();
    WPAD_Init();
    net_init();

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

void add_chat(const char *msg)
{
    for (int i = 1; i < MAX_CHAT; i++)
        strcpy(chat[i - 1], chat[i]);

    strncpy(chat[MAX_CHAT - 1], msg, MAX_LEN);
    chat[MAX_CHAT - 1][MAX_LEN - 1] = '\0';

    if (chat_count < MAX_CHAT)
        chat_count++;
}

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
        "{\"username\":\"Wii\",\"message\":\"%s\"}",
        msg);

    char req[512];
    snprintf(req, sizeof(req),
        "POST /send HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s",
        SERVER_IP,
        (int)strlen(body),
        body
    );

    net_send(sock, req, strlen(req), 0);
    net_close(sock);
}

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

    char req[256];
    snprintf(req, sizeof(req),
        "GET /messages HTTP/1.1\r\n"
        "Host: %s\r\n"
        "\r\n",
        SERVER_IP
    );

    net_send(sock, req, strlen(req), 0);

    char buffer[1024];
    int len = net_recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (len > 0)
    {
        buffer[len] = '\0';

        // versão simples: só mostra resposta bruta
        snprintf(chat[MAX_CHAT - 1], MAX_LEN, "Server OK");
    }

    net_close(sock);
}

char get_char()
{
    return charset[cursor];
}

void draw()
{
    printf("\x1b[2J");
    printf("\x1b[0;0H");

    printf("======== WiiChat ONLINE ========\n\n");

    printf("CHAT:\n");
    for (int i = 0; i < chat_count; i++)
        printf(" > %s\n", chat[i]);

    printf("\n-------------------------------\n");

    printf("INPUT: %s", input);
    if (blink) printf("_");

    printf("\n\nCHAR: ");

    for (int i = 0; i < (int)strlen(charset); i++)
    {
        if (i == cursor)
            printf("[%c] ", charset[i]);
        else
            printf(" %c  ", charset[i]);
    }

    printf("\n\nA=add  B=del  1=send  HOME=exit\n");
}

int main(int argc, char **argv)
{
    init_video();

    add_chat("System: WiiChat online");
    add_chat("System: Connected");

    while (1)
    {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);

        if (frame % 30 == 0)
            blink = !blink;

        // move cursor
        if (pressed & WPAD_BUTTON_RIGHT)
            cursor = (cursor + 1) % strlen(charset);

        if (pressed & WPAD_BUTTON_LEFT)
        {
            cursor--;
            if (cursor < 0) cursor = strlen(charset) - 1;
        }

        // add char
        if (pressed & WPAD_BUTTON_A)
        {
            if (input_len < MAX_LEN - 1)
            {
                input[input_len++] = get_char();
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

        // send
        if (pressed & WPAD_BUTTON_1)
        {
            if (input_len > 0)
            {
                send_message(input);
                add_chat(input);
                input[0] = '\0';
                input_len = 0;
            }
        }

        // fetch messages (polling)
        if (frame % 120 == 0)
        {
            fetch_messages();
        }

        draw();

        frame++;
        VIDEO_WaitVSync();
    }

    return 0;
}