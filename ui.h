#pragma once
#include <stdbool.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "core.h"

bool UI_Init(void);
bool UI_ProcessMessages(void);
void UI_UpdateInput(InputState* input);
void UI_Render(const GameState* state, const InputState* input);
void UI_Shutdown(void);