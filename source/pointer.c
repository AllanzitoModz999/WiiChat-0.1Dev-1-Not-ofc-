#include <grrlib.h>
#include <wiiuse/wpad.h>

#include "pointer.h"

ir_t ir;

void Pointer_Update(void)
{
    WPAD_IR(WPAD_CHAN_0, &ir);
}

void Pointer_Draw(void)
{
    if (!ir.valid)
        return;

    // Cursor temporário
    GRRLIB_Circle(
        ir.x,
        ir.y,
        8,
        0x000000FF,
        true
    );
}