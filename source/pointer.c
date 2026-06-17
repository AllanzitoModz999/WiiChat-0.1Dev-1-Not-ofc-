#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <stdbool.h>

#include "pointer.h"

// Arquivo gerado automaticamente pelo Makefile
#include "hand-pointer-blue.h"

static ir_t ir;
static GRRLIB_texImg *cursor = NULL;

void Pointer_Init(void)
{
    cursor = GRRLIB_LoadTexture(hand_pointer_blue_png);
}

void Pointer_Update(void)
{
    WPAD_IR(WPAD_CHAN_0, &ir);
}

void Pointer_Draw(void)
{
    if (!ir.valid || cursor == NULL)
        return;

    // Desenha a mão
    GRRLIB_DrawImg(
        ir.x,
        ir.y,
        cursor,
        0.0f,
        1.0f,
        1.0f,
        0xFFFFFFFF
    );
}

float Pointer_GetX(void)
{
    return ir.x;
}

float Pointer_GetY(void)
{
    return ir.y;
}

bool Pointer_IsValid(void)
{
    return ir.valid;
}