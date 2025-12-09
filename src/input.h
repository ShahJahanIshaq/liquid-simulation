#pragma once
#include "world.h"
#include <stdbool.h>

typedef struct {
    CellType brush;
    float    tapRate;
} InputState;


void input_handle_events(InputState *st, World *w, bool *quitFlag);