#ifndef POINTER_H
#define POINTER_H

#include <wiiuse/wpad.h>

// Inicializa o cursor
void Pointer_Init(void);

// Atualiza a posição do cursor
void Pointer_Update(void);

// Desenha o cursor
void Pointer_Draw(void);

// Retorna posição atual
float Pointer_GetX(void);
float Pointer_GetY(void);

// Retorna se o IR está válido
bool Pointer_IsValid(void);

#endif